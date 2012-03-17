#include <stdlib.h>
#include <assert.h>
#include <memory.h>

#include "chess.h"
#include "move.h"
#include "cstring.h"
#include "variation.h"

struct ChessVariation
{
    ChessMove move;
    ChessString comment;
    struct ChessVariation* parent;
    struct ChessVariation* first_child;
    struct ChessVariation* left;
    struct ChessVariation* right;
};

ChessVariation* chess_variation_new(void)
{
    ChessVariation* variation = malloc(sizeof(ChessVariation));
    memset(variation, 0, sizeof(ChessVariation));
    chess_string_init(&variation->comment);
    return variation;
}

static void free_node(ChessVariation* node)
{
    chess_string_cleanup(&node->comment);
    free(node);
}

void chess_variation_destroy(ChessVariation* variation)
{
    ChessVariation* child = NULL;
    ChessVariation* next = variation;

    assert(variation != NULL);
    while (child != variation)
    {
        child = next;
        while (child->right)
            child = child->right;

        while (child->first_child)
            child = child->first_child;

        if (child->left)
        {
            next = child->left;
            next->right = NULL;
        }
        else if (child->parent)
        {
            next = child->parent;
            next->first_child = NULL;
        }

        free_node(child);
    }
}

ChessBoolean chess_variation_is_root(const ChessVariation* variation)
{
    assert(variation != NULL);
    return variation->parent == NULL;
}

ChessMove chess_variation_move(const ChessVariation* variation)
{
    assert(variation != NULL);
    return variation->move;
}

ChessString* chess_variation_comment(ChessVariation* variation)
{
    assert(variation != NULL);
    return &variation->comment;
}

ChessVariation* chess_variation_parent(ChessVariation* variation)
{
    assert(variation != NULL);
    return variation->parent;
}

ChessVariation* chess_variation_first_child(ChessVariation* variation)
{
    assert(variation != NULL);
    return variation->first_child;
}

ChessVariation* chess_variation_left(ChessVariation* variation)
{
    assert(variation != NULL);
    return variation->left;
}

ChessVariation* chess_variation_right(ChessVariation* variation)
{
    assert(variation != NULL);
    return variation->right;
}

size_t chess_variation_length(const ChessVariation* variation)
{
    size_t length = 0;

    assert(variation != NULL);
    while ((variation = variation->first_child))
        length++;

    return length;
}

ChessVariation* chess_variation_ply(ChessVariation* variation, size_t ply)
{
    assert(chess_variation_is_root(variation));

    while ((variation = variation->first_child) && ply > 0)
        ply--;
    return variation;
}

static ChessVariation* chess_variation_add_sibling(ChessVariation* variation, ChessMove move)
{
    ChessVariation* sibling;

    while (variation->right)
        variation = variation->right;

    sibling = chess_variation_new();
    sibling->move = move;
    variation->right = sibling;
    sibling->left = variation;
    sibling->parent = variation->parent;
    return sibling;
}

ChessVariation* chess_variation_add_child(ChessVariation* variation, ChessMove move)
{
    ChessVariation* child;

    assert(variation != NULL);
    if (variation->first_child)
        return chess_variation_add_sibling(variation->first_child, move);

    child = chess_variation_new();
    child->move = move;
    variation->first_child = child;
    child->parent = variation;
    return child;
}

void chess_variation_attach_subvariation(ChessVariation* variation, ChessVariation* subvariation)
{
    ChessVariation* child, *attach_point;
    assert(variation != NULL);
    assert(chess_variation_is_root(subvariation));

    attach_point = subvariation->first_child;
    free_node(subvariation);

    if (attach_point == NULL)
    {
        /* Nothing to do */
        return;
    }

    attach_point->parent = variation;

    child = variation->first_child;
    if (child == NULL)
    {
        /* Attach directly under variation */
        variation->first_child = attach_point;
        return;
    }

    /* Attach to the right of existing child */
    while (child->right != NULL)
        child = child->right;

    child->right = attach_point;
    attach_point->left = child;
}

void chess_variation_truncate(ChessVariation* variation)
{
    assert(variation != NULL);
    if (variation->first_child)
        chess_variation_destroy(variation->first_child);
}
