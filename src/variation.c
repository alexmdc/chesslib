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
    ChessVariation* child;
    ChessVariation* parent;

    assert(variation != NULL);

    if (variation->left)
        variation->left->right = 0;
    else if (variation->parent)
        variation->parent->first_child = 0;

    /* Destroy any siblings to the right of us */
    if (variation->right)
        chess_variation_destroy(variation->right);
    assert(variation->right == NULL);

    /* Destroy any node data */
    chess_string_cleanup(&variation->comment);

    /* Destroy all child variations */
    child = variation;
    while (child->first_child)
        child = child->first_child;

    while (child != variation)
    {
        parent = child->parent;
        chess_variation_destroy(child);
        child = parent;
    }

    free(variation);
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

    assert(variation != NULL);

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
