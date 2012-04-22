#include <assert.h>
#include <stdlib.h>
#include <memory.h>

#include "game.h"
#include "cstring.h"
#include "carray.h"

struct ChessGame
{
    ChessPosition* initial_position;
    ChessVariation* root_variation;

    /* PGN tags */
    ChessString event;
    ChessString site;
    ChessString date;
    unsigned int round;
    ChessString white;
    ChessString black;
    ChessResult result;

    /* For iterating */
    ChessPosition* current_position;
    ChessVariation* current_variation;
    ChessArray unmoves;
};

ChessGame* chess_game_new(void)
{
    ChessGame* game = malloc(sizeof(ChessGame));
    memset(game, 0, sizeof(ChessGame));

    game->initial_position = chess_position_new();
    game->root_variation = chess_variation_new();

    chess_string_init(&game->event);
    chess_string_init(&game->site);
    chess_string_init(&game->date);
    chess_string_init(&game->white);
    chess_string_init(&game->black);

    game->current_position = chess_position_new();
    game->current_variation = game->root_variation;
    chess_array_init(&game->unmoves, sizeof(ChessUnmove));
    return game;
}

void chess_game_destroy(ChessGame* game)
{
    chess_position_destroy(game->initial_position);
    chess_variation_destroy(game->root_variation);

    chess_string_cleanup(&game->event);
    chess_string_cleanup(&game->site);
    chess_string_cleanup(&game->date);
    chess_string_cleanup(&game->white);
    chess_string_cleanup(&game->black);

    chess_position_destroy(game->current_position);
    chess_array_cleanup(&game->unmoves);
    free(game);
}

void chess_game_init(ChessGame* game)
{
    ChessPosition* start = chess_position_new();
    chess_position_init(start);
    chess_game_init_position(game, start);
    chess_position_destroy(start);
}

void chess_game_init_position(ChessGame* game, const ChessPosition* position)
{
    chess_position_copy(position, game->initial_position);
    chess_variation_truncate(game->root_variation);

    game->result = chess_position_check_result(position);
    game->round = 0;
    chess_string_clear(&game->event);
    chess_string_clear(&game->site);
    chess_string_clear(&game->date);
    chess_string_clear(&game->white);
    chess_string_clear(&game->black);

    chess_game_step_to_start(game);
}

void chess_game_set_root_variation(ChessGame* game, ChessVariation* variation)
{
    assert(chess_variation_is_root(variation));
    chess_variation_truncate(game->root_variation);
    chess_variation_attach_subvariation(game->root_variation, variation);
    game->current_variation = game->root_variation;
}

const ChessPosition* chess_game_initial_position(const ChessGame* game)
{
    return game->initial_position;
}

ChessVariation* chess_game_root_variation(const ChessGame* game)
{
    return game->root_variation;
}

size_t chess_game_ply(const ChessGame* game)
{
    /* TODO: More efficient implementation */
    return chess_variation_length(game->root_variation);
}

ChessMove chess_game_move_at_ply(const ChessGame* game, size_t ply)
{
    ChessVariation* variation = chess_variation_ply(game->root_variation, ply);
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
    ChessResult boardResult = chess_position_check_result(game->current_position);
    if (boardResult != CHESS_RESULT_NONE)
        return;

    game->result = result;
}

const ChessPosition* chess_game_current_position(const ChessGame* game)
{
    return game->current_position;
}

ChessVariation* chess_game_current_variation(const ChessGame* game)
{
    return game->current_variation;
}

ChessMove chess_game_current_move(const ChessGame* game)
{
    assert(!chess_variation_is_root(game->current_variation));
    return chess_variation_move(game->current_variation);
}

static void advance_current_position(ChessGame* game, ChessMove move)
{
    ChessUnmove unmove = chess_position_make_move(game->current_position, move);
    chess_array_push(&game->unmoves, &unmove);
    game->result = chess_position_check_result(game->current_position);
}

static void retreat_current_position(ChessGame* game)
{
    ChessUnmove unmove;
    chess_array_pop(&game->unmoves, &unmove);
    chess_position_undo_move(game->current_position, unmove);
    game->result = CHESS_RESULT_NONE;
}

void chess_game_append_move(ChessGame* game, ChessMove move)
{
    advance_current_position(game, move);
    game->current_variation = chess_variation_add_child(game->current_variation, move);
}

void chess_game_truncate_moves(ChessGame* game)
{
    chess_variation_truncate(game->current_variation);
}

void chess_game_step_forward(ChessGame* game)
{
    ChessVariation* next = chess_variation_first_child(game->current_variation);
    assert(next != NULL);
    advance_current_position(game, chess_variation_move(next));
    game->current_variation = next;
}

void chess_game_step_back(ChessGame* game)
{
    assert(!chess_variation_is_root(game->current_variation));
    game->current_variation = chess_variation_parent(game->current_variation);
    retreat_current_position(game);
}

void chess_game_step_to_start(ChessGame* game)
{
    chess_position_copy(game->initial_position, game->current_position);
    game->current_variation = game->root_variation;
    chess_array_clear(&game->unmoves);
}

void chess_game_step_to_end(ChessGame* game)
{
    ChessVariation* next = chess_variation_first_child(game->current_variation);
    while (next)
    {
        advance_current_position(game, chess_variation_move(next));
        game->current_variation = next;
        next = chess_variation_first_child(next);
    }
}

void chess_game_step_to_move(ChessGame* game, ChessVariation* variation)
{
    ChessArray moves;
    ChessMove move;

    assert(chess_variation_root(variation) == game->root_variation);

    chess_array_init(&moves, sizeof(ChessMove));

    while (variation != game->root_variation)
    {
        move = chess_variation_move(variation);
        chess_array_push(&moves, &move);
        variation = chess_variation_parent(variation);
    }

    chess_game_step_to_start(game);
    while (chess_array_size(&moves) > 0)
    {
        chess_array_pop(&moves, &move);
        advance_current_position(game, move);
        variation = chess_variation_add_child(variation, move);
    }
    game->current_variation = variation;

    chess_array_cleanup(&moves);
}
