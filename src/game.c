#include <assert.h>
#include <stdlib.h>
#include <memory.h>

#include "chess.h"
#include "move.h"
#include "unmove.h"
#include "position.h"
#include "game.h"
#include "cstring.h"

struct ChessGame
{
    const ChessPosition* initial;
    ChessPosition* position;
    ChessMove moves[1024];
    ChessUnmove history[1024];
    int ply;
    ChessResult result;
    ChessString result_text;
};

ChessGame* chess_game_new()
{
    ChessGame* game = malloc(sizeof(ChessGame));
    memset(game, 0, sizeof(ChessGame));
    chess_string_init(&game->result_text);
    return game;
};

void chess_game_destroy(ChessGame* game)
{
    if (game->initial)
        chess_position_destroy((ChessPosition*)game->initial);
    if (game->position)
        chess_position_destroy(game->position);
    chess_string_cleanup(&game->result_text);
    free(game);
};

const ChessPosition* chess_game_position(const ChessGame* game)
{
    return game->position;
}

const ChessPosition* chess_game_initial_position(const ChessGame* game)
{
    return game->initial;
}

int chess_game_ply(const ChessGame* game)
{
    return game->ply;
}

ChessMove chess_game_move(const ChessGame* game, int ply)
{
    assert(ply >= 0 && ply < game->ply);
    return game->moves[ply];
}

ChessResult chess_game_result(const ChessGame* game)
{
    return game->result;
}

const char* chess_game_result_text(const ChessGame* game)
{
    return chess_string_data(&game->result_text);
}

void chess_game_reset(ChessGame* game)
{
    ChessPosition* start = chess_position_new();
    chess_position_init(start);
    chess_game_reset_position(game, start);
    chess_position_destroy(start);
}

void chess_game_reset_position(ChessGame* game, const ChessPosition* position)
{
    if (game->initial)
        chess_position_destroy((ChessPosition*)game->initial);
    if (game->position)
        chess_position_destroy(game->position);
    game->initial = chess_position_clone(position);
    game->position = chess_position_clone(position);
    game->ply = 0;
    game->result = chess_position_check_result(position);
    chess_string_clear(&game->result_text);
}

void chess_game_make_move(ChessGame* game, ChessMove move)
{
    game->moves[game->ply] = move;
    game->history[game->ply] = chess_position_make_move(game->position, move);
    game->ply++;
    game->result = chess_position_check_result(game->position);
}

void chess_game_undo_move(ChessGame* game)
{
    assert(game->ply > 0);
    game->ply--;
    chess_position_undo_move(game->position, game->history[game->ply]);
    game->result = CHESS_RESULT_NONE;
}

void chess_game_set_result(ChessGame* game, ChessResult result)
{
    ChessResult boardResult = chess_position_check_result(game->position);
    if (boardResult != CHESS_RESULT_NONE)
        return;

    game->result = result;
}
