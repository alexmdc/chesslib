#include <assert.h>
#include <stdlib.h>
#include <memory.h>

#include "chess.h"
#include "move.h"
#include "unmove.h"
#include "position.h"
#include "cstring.h"
#include "variation.h"
#include "game.h"
#include "carray.h"
#include "cstring.h"
#include "variation.h"

struct ChessGame
{
    const ChessPosition* initial;
    ChessPosition* position;
    ChessVariation* root;
    ChessVariation* variation;
    ChessArray unmoves;
    ChessString event;
    ChessString site;
    ChessString date;
    unsigned int round;
    ChessString white;
    ChessString black;
    ChessResult result;
    ChessString result_text;
};

ChessGame* chess_game_new()
{
    ChessGame* game = malloc(sizeof(ChessGame));
    memset(game, 0, sizeof(ChessGame));
    chess_array_init(&game->unmoves, sizeof(ChessUnmove));
    chess_string_init(&game->event);
    chess_string_init(&game->site);
    chess_string_init(&game->date);
    chess_string_init(&game->white);
    chess_string_init(&game->black);
    chess_string_init(&game->result_text);
    return game;
};

void chess_game_destroy(ChessGame* game)
{
    if (game->initial)
        chess_position_destroy((ChessPosition*)game->initial);
    if (game->position)
        chess_position_destroy(game->position);
    if (game->root)
        chess_variation_destroy(game->root);
    chess_string_cleanup(&game->event);
    chess_string_cleanup(&game->site);
    chess_string_cleanup(&game->date);
    chess_string_cleanup(&game->white);
    chess_string_cleanup(&game->black);
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

ChessVariation* chess_game_variation(const ChessGame* game)
{
    return game->root;
}

size_t chess_game_ply(const ChessGame* game)
{
    return chess_variation_length(game->root);
}

ChessMove chess_game_move(const ChessGame* game, size_t ply)
{
    ChessVariation* variation = chess_variation_ply(game->root, ply);
    return chess_variation_move(variation);
}

const char* chess_game_event(const ChessGame* game)
{
    return chess_string_data(&game->event);
}

const char* chess_game_site(const ChessGame* game)
{
    return chess_string_data(&game->site);
}

const char* chess_game_date(const ChessGame* game)
{
    return chess_string_data(&game->date);
}

unsigned int chess_game_round(const ChessGame* game)
{
    return game->round;
}

const char* chess_game_white(const ChessGame* game)
{
    return chess_string_data(&game->white);
}

const char* chess_game_black(const ChessGame* game)
{
    return chess_string_data(&game->black);
}

ChessResult chess_game_result(const ChessGame* game)
{
    return (game->result == CHESS_RESULT_NONE) ? CHESS_RESULT_IN_PROGRESS : game->result;
}

const char* chess_game_result_text(const ChessGame* game)
{
    return chess_string_data(&game->result_text);
}

void chess_game_set_event(ChessGame* game, const char* value)
{
    chess_string_assign(&game->event, value);
}

void chess_game_set_site(ChessGame* game, const char* value)
{
    chess_string_assign(&game->site, value);
}

void chess_game_set_date(ChessGame* game, const char* value)
{
    chess_string_assign(&game->date, value);
}

void chess_game_set_round(ChessGame* game, unsigned int value)
{
    game->round = value;
}

void chess_game_set_white(ChessGame* game, const char* value)
{
    chess_string_assign(&game->white, value);
}

void chess_game_set_black(ChessGame* game, const char* value)
{
    chess_string_assign(&game->black, value);
}

void chess_game_set_result(ChessGame* game, ChessResult result)
{
    ChessResult boardResult = chess_position_check_result(game->position);
    if (boardResult != CHESS_RESULT_NONE)
        return;

    game->result = result;
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
    chess_variation_destroy(game->root);
    game->root = NULL;
    game->variation = NULL;
    game->result = chess_position_check_result(position);
    game->round = 0;
    chess_string_clear(&game->event);
    chess_string_clear(&game->site);
    chess_string_clear(&game->date);
    chess_string_clear(&game->white);
    chess_string_clear(&game->black);
    chess_string_clear(&game->result_text);
}

void chess_game_set_variation(ChessGame* game, ChessVariation* variation)
{
    assert(chess_variation_parent(variation) == NULL);
    assert(chess_variation_left(variation) == NULL);
    chess_variation_destroy(game->root);
    game->root = variation;
    game->variation = game->root;
}

void chess_game_make_move(ChessGame* game, ChessMove move)
{
    ChessUnmove unmove = chess_position_make_move(game->position, move);
    chess_array_push(&game->unmoves, &unmove);
    game->result = chess_position_check_result(game->position);
    game->variation = chess_variation_add_child(game->variation, move);
    if (game->root == NULL)
        game->root = game->variation;
}

void chess_game_undo_move(ChessGame* game)
{
    ChessVariation* variation;
    ChessUnmove unmove;
    chess_array_pop(&game->unmoves, &unmove);
    chess_position_undo_move(game->position, unmove);
    game->result = CHESS_RESULT_NONE;

    variation = game->variation;
    game->variation = chess_variation_parent(game->variation);
    chess_variation_destroy(variation);
    if (game->variation == NULL)
        game->root = NULL;
}
