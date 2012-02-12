#include <stdio.h>

#include "chess.h"
#include "move.h"
#include "unmove.h"
#include "position.h"
#include "game.h"
#include "pgn.h"
#include "print.h"

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
