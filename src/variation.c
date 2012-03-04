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

ChessVariation* chess_variation_new(ChessMove move)
{
    ChessVariation* variation = malloc(sizeof(ChessVariation));
    memset(variation, 0, sizeof(ChessVariation));
    variation->move = move;
    chess_string_init(&variation->comment);
    return variation;
}

void chess_variation_destroy(ChessVariation* variation)
{
    ChessVariation* child = NULL;
    ChessVariation* next = variation;

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

        chess_string_cleanup(&child->comment);
        free(child);
    }
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

    for (; variation; variation = variation->first_child)
        length++;

    return length;
}

ChessVariation* chess_variation_ply(ChessVariation* variation, size_t ply)
{
    for (; ply > 0 && variation; ply--)
        variation = variation->first_child;
    return variation;
}

ChessVariation* chess_variation_add_child(ChessVariation* variation, ChessMove move)
{
    ChessVariation* child;

    if (variation == NULL)
        return chess_variation_new(move);

    if (variation->first_child)
        return chess_variation_add_sibling(variation->first_child, move);

    child = chess_variation_new(move);
    variation->first_child = child;
    child->parent = variation;
    return child;
}

ChessVariation* chess_variation_add_sibling(ChessVariation* variation, ChessMove move)
{
    ChessVariation* sibling;

    assert(variation != NULL);

    while (variation->right)
        variation = variation->right;

    sibling = chess_variation_new(move);
    variation->right = sibling;
    sibling->left = variation;
    sibling->parent = variation->parent;
    return sibling;
}

void chess_variation_attach_subvariation(ChessVariation* variation, ChessVariation* subvariation)
{
    assert(variation != NULL);
    assert(subvariation->parent == NULL);
    assert(subvariation->left == NULL);

    while (variation->right != NULL)
        variation = variation->right;

    variation->right = subvariation;
    subvariation->left = variation;
    subvariation->parent = variation->parent;
}
