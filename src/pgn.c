#include <stdio.h>
#include <string.h>

#include "chess.h"
#include "move.h"
#include "unmove.h"
#include "position.h"
#include "cstring.h"
#include "variation.h"
#include "game.h"
#include "pgn.h"
#include "pgn-tokenizer.h"
#include "print.h"
#include "parse.h"
#include "variation.h"

static int append_tag(const char* name, const char* value, char* s)
{
    return sprintf(s, "[%s \"%s\"]\n", name, value);
}

void chess_pgn_save(const ChessGame* game, char* s)
{
    char buf[16];
    int round, n = 0;

    n += append_tag("Event", chess_game_event(game), s + n);
    n += append_tag("Site", chess_game_site(game), s + n);
    n += append_tag("Date", chess_game_date(game), s + n);
    round = chess_game_round(game);
    if (round > 0)
        sprintf(buf, "%d", chess_game_round(game));
    else
        buf[0] = '\0';
    n += append_tag("Round", buf, s + n);
    n += append_tag("White", chess_game_white(game), s + n);
    n += append_tag("Black", chess_game_black(game), s + n);
    chess_print_result(chess_game_result(game), buf);
    n += append_tag("Result", buf, s + n);
    s[n++] = '\n';

    n += chess_print_game_moves(game, s + n);
    s[n++] = '\n';
    s[n] = 0;
}

static void assign_tag(ChessGame* game, const ChessString* tag, const ChessString* value)
{
    const char* s = chess_string_data(tag);
    unsigned int round;

    if (strcmp(s, "Event") == 0)
    {
        chess_game_set_event(game, chess_string_data(value));
    }
    else if (strcmp(s, "Site") == 0)
    {
        chess_game_set_site(game, chess_string_data(value));
    }
    else if (strcmp(s, "Date") == 0)
    {
        chess_game_set_date(game, chess_string_data(value));
    }
    else if (strcmp(s, "White") == 0)
    {
        chess_game_set_white(game, chess_string_data(value));
    }
    else if (strcmp(s, "Black") == 0)
    {
        chess_game_set_black(game, chess_string_data(value));
    }
    else if (strcmp(s, "Round") == 0)
    {
        if (sscanf(chess_string_data(value), "%u", &round) > 0)
            chess_game_set_round(game, round);
    }
}

static ChessPgnLoadResult parse_tag(ChessPgnTokenizer* tokenizer, ChessGame* game)
{
    const ChessPgnToken* token;
    ChessString tag, value;
    ChessPgnLoadResult result = CHESS_PGN_LOAD_OK;

    chess_string_init(&tag);
    chess_string_init(&value);

    token = chess_pgn_tokenizer_next(tokenizer); /* L_BRACKET */
    token = chess_pgn_tokenizer_next(tokenizer);
    if (token->type != CHESS_PGN_TOKEN_SYMBOL)
    {
        result = CHESS_PGN_LOAD_UNEXPECTED_TOKEN;
        goto error;
    }
    chess_string_assign(&tag, chess_string_data(&token->data.string));

    token = chess_pgn_tokenizer_next(tokenizer);
    if (token->type != CHESS_PGN_TOKEN_STRING)
    {
        result = CHESS_PGN_LOAD_UNEXPECTED_TOKEN;
        goto error;
    }
    chess_string_assign(&value, chess_string_data(&token->data.string));

    token = chess_pgn_tokenizer_next(tokenizer);
    if (token->type != CHESS_PGN_TOKEN_R_BRACKET)
    {
        result = CHESS_PGN_LOAD_UNEXPECTED_TOKEN;
        goto error;
    }

    assign_tag(game, &tag, &value);

error:
    chess_string_cleanup(&tag);
    chess_string_cleanup(&value);
    return result;
}

static ChessPgnLoadResult parse_move(ChessPgnTokenizer* tokenizer,
    const ChessPosition* position, ChessMove* move)
{
    ChessParseResult result;
    const ChessPgnToken* token;

    token = chess_pgn_tokenizer_peek(tokenizer); /* SYMBOL */
    result = chess_parse_move(chess_string_data(&token->data.string), position, move);
    if (result != CHESS_PARSE_OK)
        return CHESS_PGN_LOAD_ILLEGAL_MOVE;

    chess_pgn_tokenizer_next(tokenizer);
    return CHESS_PGN_LOAD_OK;
}

static ChessPgnLoadResult parse_variation(ChessPgnTokenizer* tokenizer,
    const ChessPosition* position, ChessVariation** variation)
{
    const ChessPgnToken* token;
    ChessMove move;
    ChessPgnLoadResult result = CHESS_PGN_LOAD_OK;
    ChessPosition* current_position = chess_position_clone(position);
    ChessVariation* current_variation = NULL;

    while (result == CHESS_PGN_LOAD_OK)
    {
        token = chess_pgn_tokenizer_peek(tokenizer);
        switch (token->type)
        {
            case CHESS_PGN_TOKEN_NUMBER:
            case CHESS_PGN_TOKEN_PERIOD:
            case CHESS_PGN_TOKEN_COMMENT:
                /* Skip these tokens */
                chess_pgn_tokenizer_next(tokenizer);
                break;
            case CHESS_PGN_TOKEN_SYMBOL:
                result = parse_move(tokenizer, current_position, &move);
                if (result != CHESS_PGN_LOAD_OK)
                    break;

                chess_position_make_move(current_position, move);
                current_variation = chess_variation_add_child(current_variation, move);

                break;
            default:
                result = CHESS_PGN_LOAD_UNEXPECTED_TOKEN;
                break;
        }
    }

    while (current_variation != NULL && chess_variation_parent(current_variation))
        current_variation = chess_variation_parent(current_variation);

    chess_position_destroy(current_position);
    *variation = current_variation;
    return result;
}

static ChessPgnLoadResult parse_movetext(ChessPgnTokenizer* tokenizer,
    ChessVariation** variation, ChessResult* game_result)
{
    const ChessPgnToken* token;
    ChessPgnLoadResult result = CHESS_PGN_LOAD_OK;
    ChessPosition* position;

    position = chess_position_new();
    chess_position_init(position);
    parse_variation(tokenizer, position, variation);
    chess_position_destroy(position);

    token = chess_pgn_tokenizer_peek(tokenizer);
    switch (token->type)
    {
        case CHESS_PGN_TOKEN_ASTERISK:
            chess_pgn_tokenizer_next(tokenizer);
            *game_result = CHESS_RESULT_IN_PROGRESS;
            break;
        case CHESS_PGN_TOKEN_ONE_ZERO:
            chess_pgn_tokenizer_next(tokenizer);
            *game_result = CHESS_RESULT_WHITE_WINS;
            break;
        case CHESS_PGN_TOKEN_ZERO_ONE:
            chess_pgn_tokenizer_next(tokenizer);
            *game_result = CHESS_RESULT_BLACK_WINS;
            break;
        case CHESS_PGN_TOKEN_HALF_HALF:
            chess_pgn_tokenizer_next(tokenizer);
            *game_result = CHESS_RESULT_DRAW;
            break;
        default:
            result = CHESS_PGN_LOAD_UNEXPECTED_TOKEN;
            break;
    }

    return result;
}

static ChessPgnLoadResult parse_game(ChessPgnTokenizer* tokenizer, ChessGame* game)
{
    const ChessPgnToken* token;
    ChessVariation* variation;
    ChessResult game_result;
    ChessPgnLoadResult result;

    for (;;)
    {
        token = chess_pgn_tokenizer_peek(tokenizer);
        switch (token->type)
        {
            case CHESS_PGN_TOKEN_L_BRACKET:
                result = parse_tag(tokenizer, game);
                if (result != CHESS_PGN_LOAD_OK)
                    return result;
                break;
            default:
                result = parse_movetext(tokenizer, &variation, &game_result);
                if (result == CHESS_PGN_LOAD_OK)
                {
                    chess_game_set_variation(game, variation);
                    chess_game_set_result(game, game_result);
                }
                return result;
        }
    }
}

ChessPgnLoadResult chess_pgn_load(const char* s, ChessGame* game)
{
    ChessPgnTokenizer* tokenizer = chess_pgn_tokenizer_new(s);
    ChessPgnLoadResult result = parse_game(tokenizer, game);
    chess_pgn_tokenizer_destroy(tokenizer);
    return result;
}
