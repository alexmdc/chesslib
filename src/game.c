#include <assert.h>
#include <stdlib.h>
#include <memory.h>

#include "game.h"
#include "calloc.h"
#include "cstring.h"
#include "carray.h"

struct ExtraTag
{
    ChessString name;
    ChessString value;
    struct ExtraTag* next;
};
typedef struct ExtraTag ExtraTag;

struct ChessGame
{
    ChessPosition* initial_position;
    ChessVariation* root_variation;

    /* PGN tags */
    ChessString event;
    ChessString site;
    ChessString date;
    ChessString round;
    ChessString white;
    ChessString black;
    ChessResult result;
    ExtraTag* extra;
};

struct ChessGameIterator
{
    ChessGame* game;
    ChessVariation* variation;
    ChessPosition position;
    ChessArray unmoves;
};

ChessGame* chess_game_new(void)
{
    ChessPosition position;
    chess_position_init(&position);
    return chess_game_new_position(&position);
}

ChessGame* chess_game_new_position(const ChessPosition* position)
{
    ChessGame* game = chess_alloc(sizeof(ChessGame));
    memset(game, 0, sizeof(ChessGame));

    game->initial_position = chess_position_new();
    game->root_variation = chess_variation_new();

    chess_string_init(&game->event);
    chess_string_init(&game->site);
    chess_string_init(&game->date);
    chess_string_init(&game->round);
    chess_string_init(&game->white);
    chess_string_init(&game->black);

    return game;
}

ChessGame* chess_game_new_fen(const char* fen)
{
    ChessPosition position;
    chess_position_init_fen(&position, fen);
    return chess_game_new_position(&position);
}

static void cleanup_extra_tags(ChessGame* game)
{
    ExtraTag* extra, *next;
    for (next = game->extra; (extra = next) != NULL; next = extra->next)
    {
        chess_string_cleanup(&extra->name);
        chess_string_cleanup(&extra->value);
        chess_free(extra);
    }
}

void chess_game_destroy(ChessGame* game)
{
    assert(game != NULL);
    chess_position_destroy(game->initial_position);
    chess_variation_destroy(game->root_variation);

    chess_string_cleanup(&game->event);
    chess_string_cleanup(&game->site);
    chess_string_cleanup(&game->date);
    chess_string_cleanup(&game->round);
    chess_string_cleanup(&game->white);
    chess_string_cleanup(&game->black);
    cleanup_extra_tags(game);

    chess_free(game);
}

void chess_game_reset(ChessGame* game)
{
    ChessPosition position;
    chess_position_init(&position);
    chess_game_reset_position(game, &position);
}

void chess_game_reset_position(ChessGame* game, const ChessPosition* position)
{
    chess_position_copy(position, game->initial_position);
    chess_variation_truncate(game->root_variation);

    game->result = chess_position_check_result(position);
    chess_string_clear(&game->event);
    chess_string_clear(&game->site);
    chess_string_clear(&game->date);
    chess_string_clear(&game->round);
    chess_string_clear(&game->white);
    chess_string_clear(&game->black);
    cleanup_extra_tags(game);
    game->extra = NULL;
}

void chess_game_reset_fen(ChessGame* game, const char* fen)
{
    ChessPosition position;
    chess_position_init_fen(&position, fen);
    chess_game_reset_position(game, &position);
}

void chess_game_set_root_variation(ChessGame* game, ChessVariation* variation)
{
    assert(chess_variation_is_root(variation));
    chess_variation_truncate(game->root_variation);
    chess_variation_attach_subvariation(game->root_variation, variation);
}

void chess_game_set_initial_position(ChessGame* game, const ChessPosition* position)
{
    chess_position_copy(position, game->initial_position);
    chess_variation_truncate(game->root_variation);
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

const char* chess_game_round(const ChessGame* game)
{
    return chess_string_data(&game->round);
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

void chess_game_set_round(ChessGame* game, const char* value)
{
    chess_string_assign(&game->round, value);
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
    game->result = result;
}

void chess_game_set_tag(ChessGame* game, const char* name, const char* value)
{
    /* Check STR tags */
    if (strcasecmp(name, "Event") == 0)
        chess_game_set_event(game, value);
    else if (strcasecmp(name, "Site") == 0)
        chess_game_set_site(game, value);
    else if (strcasecmp(name, "Date") == 0)
        chess_game_set_date(game, value);
    else if (strcasecmp(name, "Round") == 0)
        chess_game_set_round(game, value);
    else if (strcasecmp(name, "White") == 0)
        chess_game_set_white(game, value);
    else if (strcasecmp(name, "Black") == 0)
        chess_game_set_black(game, value);
    else if (strcasecmp(name, "Result") == 0)
    {
        if (strcasecmp(value, "1-0") == 0)
            chess_game_set_result(game, CHESS_RESULT_WHITE_WINS);
        else if (strcasecmp(value, "0-1") == 0)
            chess_game_set_result(game, CHESS_RESULT_BLACK_WINS);
        else if (strcasecmp(value, "1/2-1/2") == 0)
            chess_game_set_result(game, CHESS_RESULT_DRAW);
        else if (strcasecmp(value, "*") == 0)
            chess_game_set_result(game, CHESS_RESULT_IN_PROGRESS);
        return;
    }
    else
    {
        /* Maybe an extra tag */
        ExtraTag* extra = game->extra;
        ExtraTag* last = extra;
        while (extra != NULL)
        {
            if (strcasecmp(name, chess_string_data(&extra->name)) == 0)
            {
                chess_string_assign(&extra->value, value);
                return;
            }
            last = extra;
            extra = extra->next;
        }

        /* Add it as a new tag */
        extra = chess_alloc(sizeof(ExtraTag));
        chess_string_init_assign(&extra->name, name);
        chess_string_init_assign(&extra->value, value);
        extra->next = NULL;

        if (last != NULL)
            last->next = extra;
        else
            game->extra = extra;
    }
}

void chess_game_remove_tag(ChessGame* game, const char* name)
{
    /* Check STR tags */
    if (strcasecmp(name, "Event") == 0)
        chess_game_set_event(game, "");
    else if (strcasecmp(name, "Site") == 0)
        chess_game_set_site(game, "");
    else if (strcasecmp(name, "Date") == 0)
        chess_game_set_date(game, "");
    else if (strcasecmp(name, "Round") == 0)
        chess_game_set_round(game, "");
    else if (strcasecmp(name, "White") == 0)
        chess_game_set_white(game, "");
    else if (strcasecmp(name, "Black") == 0)
        chess_game_set_black(game, "");
    else if (strcasecmp(name, "Result") == 0)
        chess_game_set_result(game, CHESS_RESULT_NONE);
    else
    {
        /* Maybe an extra tag */
        ExtraTag* extra = game->extra;
        ExtraTag* last = NULL;
        while (extra != NULL)
        {
            if (strcasecmp(name, chess_string_data(&extra->name)) == 0)
                break;

            last = extra;
            extra = extra->next;
        }

        if (extra == NULL)
            return; /* Not found */

        if (last != NULL)
        {
            last->next = extra->next;
        }
        else
        {
            game->extra = extra->next;
        }
        chess_free(extra);
    }
}

const char* chess_game_tag_value(ChessGame* game, const char* name)
{
    ExtraTag* extra;

    /* Check STR tags */
    if (strcasecmp(name, "Event") == 0)
        return chess_game_event(game);
    else if (strcasecmp(name, "Site") == 0)
        return chess_game_site(game);
    else if (strcasecmp(name, "Date") == 0)
        return chess_game_date(game);
    else if (strcasecmp(name, "Round") == 0)
        return chess_game_round(game);
    else if (strcasecmp(name, "White") == 0)
        return chess_game_white(game);
    else if (strcasecmp(name, "Black") == 0)
        return chess_game_black(game);
    else if (strcasecmp(name, "Result") == 0)
    {
        switch (chess_game_result(game)) {
            case CHESS_RESULT_WHITE_WINS:
                return "1-0";
            case CHESS_RESULT_BLACK_WINS:
                return "0-1";
            case CHESS_RESULT_DRAW:
                return "1/2-1/2";
            case CHESS_RESULT_IN_PROGRESS:
                return "*";
            default:
                return "";
        }
    }
    else
    {
        /* Maybe an extra tag */
        for (extra = game->extra; extra != NULL; extra = extra->next)
        {
            if (strcasecmp(name, chess_string_data(&extra->name)) == 0)
                return chess_string_data(&extra->value);
        }

        return NULL;
    }
}

ChessGameIterator* chess_game_get_iterator(ChessGame* game)
{
    ChessGameIterator* iter = chess_alloc(sizeof(ChessGameIterator));
    memset(iter, 0, sizeof(ChessGameIterator));
    iter->game = game;
    chess_position_copy(game->initial_position, &iter->position);
    iter->variation = game->root_variation;
    chess_array_init(&iter->unmoves, sizeof(ChessUnmove));
    return iter;
}

void chess_game_iterator_destroy(ChessGameIterator* iter)
{
    chess_array_cleanup(&iter->unmoves);
    chess_free(iter);
}

ChessGame* chess_game_iterator_game(const ChessGameIterator* iter)
{
    return iter->game;
}

const ChessPosition* chess_game_iterator_position(const ChessGameIterator* iter)
{
    return &iter->position;
}

ChessVariation* chess_game_iterator_variation(ChessGameIterator* iter)
{
    return iter->variation;
}

ChessMove chess_game_iterator_move(const ChessGameIterator* iter)
{
    assert(!chess_variation_is_root(iter->variation));
    return chess_variation_move(iter->variation);
}

size_t chess_game_iterator_ply(const ChessGameIterator* iter)
{
    return chess_array_size(&iter->unmoves);
}

ChessResult chess_game_iterator_check_result(const ChessGameIterator* iter)
{
    return chess_position_check_result(&iter->position);
}

static void advance_current_position(ChessGameIterator* iter, ChessMove move)
{
    ChessUnmove unmove = chess_position_make_move(&iter->position, move);
    chess_array_push(&iter->unmoves, &unmove);
}

static void retreat_current_position(ChessGameIterator* iter)
{
    ChessUnmove unmove;
    chess_array_pop(&iter->unmoves, &unmove);
    chess_position_undo_move(&iter->position, unmove);
}

void chess_game_iterator_append_move(ChessGameIterator* iter, ChessMove move)
{
    advance_current_position(iter, move);
    iter->variation = chess_variation_add_child(iter->variation, move);
}

void chess_game_iterator_truncate_moves(ChessGameIterator* iter)
{
    chess_variation_truncate(iter->variation);
}

void chess_game_iterator_step_forward(ChessGameIterator* iter)
{
    ChessVariation* next = chess_variation_first_child(iter->variation);
    assert(next != NULL);
    advance_current_position(iter, chess_variation_move(next));
    iter->variation = next;
}

void chess_game_iterator_step_back(ChessGameIterator* iter)
{
    assert(!chess_variation_is_root(iter->variation));
    iter->variation = chess_variation_parent(iter->variation);
    retreat_current_position(iter);
}

void chess_game_iterator_step_to_start(ChessGameIterator* iter)
{
    chess_position_copy(iter->game->initial_position, &iter->position);
    iter->variation = iter->game->root_variation;
    chess_array_clear(&iter->unmoves);
}

void chess_game_iterator_step_to_end(ChessGameIterator* iter)
{
    ChessVariation* next = chess_variation_first_child(iter->variation);
    while (next)
    {
        advance_current_position(iter, chess_variation_move(next));
        iter->variation = next;
        next = chess_variation_first_child(next);
    }
}

void chess_game_iterator_step_to_move(ChessGameIterator* iter, ChessVariation* variation)
{
    ChessArray moves;
    ChessMove move;

    assert(chess_variation_root(variation) == iter->game->root_variation);

    chess_array_init(&moves, sizeof(ChessMove));

    while (variation != iter->game->root_variation)
    {
        move = chess_variation_move(variation);
        chess_array_push(&moves, &move);
        variation = chess_variation_parent(variation);
    }

    chess_game_iterator_step_to_start(iter);
    while (chess_array_size(&moves) > 0)
    {
        chess_array_pop(&moves, &move);
        advance_current_position(iter, move);
        variation = chess_variation_add_child(variation, move);
    }
    iter->variation = variation;

    chess_array_cleanup(&moves);
}

ChessGameTagIterator chess_game_get_tag_iterator(ChessGame* game)
{
    ChessGameTagIterator iter;
    iter.game = game;
    iter.index = -1;
    iter.extra = NULL;
    return iter;
}

const char* chess_game_tag_iterator_name(const ChessGameTagIterator* iter)
{
    if (iter->extra)
        return chess_string_data(&((ExtraTag*)iter->extra)->name);

    switch (iter->index)
    {
        case 0: return "Event";
        case 1: return "Site";
        case 2: return "Date";
        case 3: return "Round";
        case 4: return "White";
        case 5: return "Black";
        case 6: return "Result";
        default: return NULL;
    }
}

const char* chess_game_tag_iterator_value(const ChessGameTagIterator* iter)
{
    if (iter->extra)
        return chess_string_data(&((ExtraTag*)iter->extra)->value);

    switch (iter->index)
    {
        case 0: return chess_game_event(iter->game);
        case 1: return chess_game_site(iter->game);
        case 2: return chess_game_date(iter->game);
        case 3: return chess_game_round(iter->game);
        case 4: return chess_game_white(iter->game);
        case 5: return chess_game_black(iter->game);
        case 6: return chess_game_tag_value(iter->game, "Result");
        default: return NULL;
    }
}

ChessBoolean chess_game_tag_iterator_next(ChessGameTagIterator* iter)
{
    ++iter->index;

    if (iter->index < 7)
        return CHESS_TRUE;
    else if (iter->index == 7)
        return (iter->extra = iter->game->extra) != NULL;
    else
        return (iter->extra != NULL)
            && (iter->extra = ((ExtraTag*)iter->extra)->next) != NULL;
}
