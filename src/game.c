#include <assert.h>
#include <stdlib.h>
#include <memory.h>

#include "chess.h"
#include "move.h"
#include "unmove.h"
#include "position.h"
#include "game.h"
#include "carray.h"
#include "cstring.h"

typedef struct
{
    ChessMove move;
    ChessUnmove unmove;
} GameMove;

struct ChessGame
{
    const ChessPosition* initial;
    ChessPosition* position;
    ChessArray moves;
    ChessResult result;
    ChessString result_text;
};

ChessGame* chess_game_new()
{
    ChessGame* game = malloc(sizeof(ChessGame));
    memset(game, 0, sizeof(ChessGame));
    chess_array_init(&game->moves, sizeof(GameMove));
    chess_string_init(&game->result_text);
    return game;
};

void chess_game_destroy(ChessGame* game)
{
    if (game->initial)
        chess_position_destroy((ChessPosition*)game->initial);
    if (game->position)
        chess_position_destroy(game->position);
    chess_array_cleanup(&game->moves);
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

size_t chess_game_ply(const ChessGame* game)
{
    return chess_array_size(&game->moves);
}

ChessMove chess_game_move(const ChessGame* game, size_t ply)
{
    const GameMove* gameMove = chess_array_elem(&game->moves, ply);
    return gameMove->move;
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
    chess_array_clear(&game->moves);
    game->result = chess_position_check_result(position);
    chess_string_clear(&game->result_text);
}

void chess_game_make_move(ChessGame* game, ChessMove move)
{
	ChessUnmove unmove = chess_position_make_move(game->position, move);
    GameMove gameMove = { move, unmove };
    chess_array_push(&game->moves, &gameMove);
    game->result = chess_position_check_result(game->position);
}

void chess_game_undo_move(ChessGame* game)
{
    GameMove gameMove;
    chess_array_pop(&game->moves, &gameMove);
    chess_position_undo_move(game->position, gameMove.unmove);
    game->result = CHESS_RESULT_NONE;
}

void chess_game_set_result(ChessGame* game, ChessResult result)
{
    ChessResult boardResult = chess_position_check_result(game->position);
    if (boardResult != CHESS_RESULT_NONE)
        return;

    game->result = result;
}
