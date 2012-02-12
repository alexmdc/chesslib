#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "../chess.h"
#include "../move.h"
#include "../unmove.h"
#include "../position.h"
#include "../game.h"
#include "../generate.h"
#include "../fen.h"
#include "../pgn.h"
#include "../print.h"
#include "../parse.h"

static char* read_line(const char* prompt)
{
    char* buf, *c;
    size_t buf_size = 0;
    size_t chunk_size = 128;

    buf_size = chunk_size;
    buf = malloc(buf_size);

    fputs(prompt, stdout);

    c = buf;
    while ((c = fgets(c, chunk_size, stdin)))
    {
        c = strchr(buf, '\0');
        if (c > buf && *(c - 1) == '\n')
        {
            *(c - 1) = 0;
            return buf;
        }

        buf_size += chunk_size;
        buf = realloc(buf, buf_size);
    }
    return buf;
}

static ChessBoolean parse_line(char* s, char** cmd, char** args)
{
    while (isspace(*s))
        s++;
    *cmd = s;
    if (!*s)
        return CHESS_FALSE;

    while (*s && !isspace(*s))
        s++;

    *s++ = 0; /* Null terminate the cmd */

    /* Parse args */
    while (*s && isspace(*s))
        s++;
    *args = s;
    return CHESS_TRUE;
}

static void list_moves(const ChessGame* game)
{
    const ChessPosition* position = chess_game_position(game);
    ChessMove moves[100];
    char buf[1024];
    int n, i;

    n = chess_generate_moves(position, moves);
    for (i = 0; i < n; i++)
    {
        chess_print_move_san(moves[i], position, buf);
        printf("%s ", buf);
    }
    putchar('\n');
}

static void game_moves(const ChessGame* game)
{
    char buf[1024];
    chess_print_game_moves(game, buf);
    printf("%s\n", buf);
}

static void print_board(const ChessGame* game)
{
    const ChessPosition* position = chess_game_position(game);
    char buf[1024];
    chess_print_position(position, buf);
    fputs(buf, stdout);
}

static void load_fen(ChessGame* game, const char* fen)
{
    ChessPosition* position = chess_position_new();
    chess_fen_load(fen, position);
    chess_game_reset_position(game, position);
    print_board(game);
    chess_position_destroy(position);
}

static void save_pgn(const ChessGame* game)
{
    char buf[1024];
    chess_pgn_save(game, buf);
    printf("%s\n", buf);
}

static void undo_move(ChessGame* game)
{
    if (chess_game_ply(game) == 0)
    {
        puts("Error: no move to undo");
    }
    else
    {
        chess_game_undo_move(game);
        print_board(game);
    }
}

static void handle_move(ChessGame* game, const char* cmd)
{
    const ChessPosition* position = chess_game_position(game);
    ChessMove move = 0;
    ChessParseResult result = chess_parse_move(cmd, position, &move);
    char buf[10];

    if (result == CHESS_PARSE_ERROR)
    {
        printf("Error (unknown command): %s\n", cmd);
    }
    else if (result == CHESS_PARSE_ILLEGAL_MOVE)
    {
        printf("Error (illegal move): %s\n", cmd);
    }
    else if (result == CHESS_PARSE_AMBIGUOUS_MOVE)
    {
        printf("Error (ambiguous move): %s\n", cmd);
    }
    else
    {
        chess_print_move_san(move, position, buf);
        puts(buf);

        chess_game_make_move(game, move);
        print_board(game);

        ChessResult result = chess_game_result(game);
        if (result != CHESS_RESULT_NONE)
        {
            chess_print_result(result, buf);
            puts(buf);
        }
    }
}

static void set_event(ChessGame* game, const char* arg)
{
    if (strlen(arg) == 0)
        puts(chess_game_event(game));
    else
        chess_game_set_event(game, arg);
}

static void set_site(ChessGame* game, const char* arg)
{
    if (strlen(arg) == 0)
        puts(chess_game_site(game));
    else
        chess_game_set_site(game, arg);
}

static void set_date(ChessGame* game, const char* arg)
{
    if (strlen(arg) == 0)
        puts(chess_game_date(game));
    else
        chess_game_set_date(game, arg);
}

static void set_round(ChessGame* game, const char* arg)
{
    unsigned int round;

    if (strlen(arg) == 0)
        printf("%d\n", chess_game_round(game));
    else
    {
        if (sscanf(arg, "%d", &round) == 1)
            chess_game_set_round(game, round);
        else
            printf("Error (parse error): %s\n", arg);
    }
}

static void set_white(ChessGame* game, const char* arg)
{
    if (strlen(arg) == 0)
        puts(chess_game_white(game));
    else
        chess_game_set_white(game, arg);
}

static void set_black(ChessGame* game, const char* arg)
{
    if (strlen(arg) == 0)
        puts(chess_game_black(game));
    else
        chess_game_set_black(game, arg);
}

static void set_result(ChessGame* game, const char* arg)
{
    ChessResult result;
    char buf[10];

    if (strlen(arg) == 0)
    {
        result = chess_game_result(game);
        if (result == CHESS_RESULT_NONE)
            result = CHESS_RESULT_IN_PROGRESS;
        chess_print_result(result, buf);
        puts(buf);
    }
    else
    {
        if (!strcasecmp(arg, "white"))
        {
            chess_game_set_result(game, CHESS_RESULT_WHITE_WINS);
            game_moves(game);
        }
        else if (!strcasecmp(arg, "black"))
        {
            chess_game_set_result(game, CHESS_RESULT_BLACK_WINS);
            game_moves(game);
        }
        else if (!strcasecmp(arg, "draw"))
        {
            chess_game_set_result(game, CHESS_RESULT_DRAW);
            game_moves(game);
        }
        else if (!strcasecmp(arg, "*"))
        {
            chess_game_set_result(game, CHESS_RESULT_NONE);
            game_moves(game);
        }
        else
        {
            printf("Error (unknown result): %s\n", arg);
        }
    }

}

int main (int argc, const char* argv[])
{
    char *line, *cmd, *args;
    ChessGame* game;
    int quit = 0;

    chess_generate_init();

    game = chess_game_new();
    chess_game_reset(game);
    print_board(game);

    line = 0;

    for (;;)
    {
        if (line)
            free(line);

        if (quit)
            break;

        line = read_line("> ");
        if (!parse_line(line, &cmd, &args))
            continue;

        if (!strcmp(cmd, "quit") || !strcmp(cmd, "q"))
        {
            quit = 1;
        }
        else if (!strcmp(cmd, "new"))
        {
            chess_game_reset(game);
            print_board(game);
        }
        else if (!strcmp(cmd, "fen"))
        {
            load_fen(game, args);
        }
        else if (!strcmp(cmd, "pgn"))
        {
            save_pgn(game);
        }
        else if (!strcmp(cmd, "ls"))
        {
            list_moves(game);
        }
        else if (!strcmp(cmd, "moves"))
        {
            game_moves(game);
        }
        else if (!strcmp(cmd, "bd"))
        {
            print_board(game);
        }
        else if (!strcmp(cmd, "undo"))
        {
            undo_move(game);
        }
        else if (!strcmp(cmd, "event"))
        {
            set_event(game, args);
        }
        else if (!strcmp(cmd, "site"))
        {
            set_site(game, args);
        }
        else if (!strcmp(cmd, "date"))
        {
            set_date(game, args);
        }
        else if (!strcmp(cmd, "round"))
        {
            set_round(game, args);
        }
        else if (!strcmp(cmd, "white"))
        {
            set_white(game, args);
        }
        else if (!strcmp(cmd, "black"))
        {
            set_black(game, args);
        }
        else if (!strcmp(cmd, "result"))
        {
            set_result(game, args);
        }
        else
        {
            handle_move(game, cmd);
        }
    }

    chess_game_destroy(game);

    return 0;
}
