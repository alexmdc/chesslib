#include <stdlib.h>
#include <assert.h>
#include <memory.h>

#include "chess.h"
#include "move.h"
#include "calloc.h"
#include "cstring.h"
#include "variation.h"

static ChessVariation* new_node(ChessVariation* root)
{
    ChessVariation* variation = chess_alloc(sizeof(ChessVariation));
    memset(variation, 0, sizeof(ChessVariation));
    variation->root = root;
    chess_string_init(&variation->comment);
    return variation;
}

static void free_node(ChessVariation* node)
{
    chess_string_cleanup(&node->comment);
    chess_free(node);
}

static void free_node_tree(ChessVariation* node)
{
    ChessVariation* child = NULL;
    ChessVariation* next = node;
    while (child != node)
    {
        child = next;

        /* Walk down to the bottom-rightmost node */
        while ((child->right && (child = child->right))
            || (child->first_child && (child = child->first_child)))
            ;

        assert(child->first_child == NULL && child->right == NULL);

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

typedef void(*NodeVisitor)(ChessVariation*, void*);
static void for_each_node(ChessVariation* start, NodeVisitor visitor, void* closure)
{
    /* Call visitor for every child node */
    ChessVariation* node = start->first_child;
    while (node != NULL)
    {
        visitor(node, closure);
        while (node->first_child != NULL)
        {
            node = node->first_child;
            visitor(node, closure);
        }

        /* No more children, move back until we reach start, or a node with a sibling
           to move down */
        while (node != start && node->right != NULL)
            node = node->parent;

        if (node == start)
            break;
        node = node->right;
    }
}

ChessVariation* chess_variation_new(void)
{
    /* Allocate a new root node */
    ChessVariation* variation = new_node(NULL);
    variation->root = variation;
    return variation;
}

void chess_variation_destroy(ChessVariation* variation)
{
    assert(chess_variation_is_root(variation));
    free_node_tree(variation);
}

ChessBoolean chess_variation_is_root(const ChessVariation* variation)
{
    assert(variation != NULL);
    return variation == variation->root;
}

size_t chess_variation_annotations(const ChessVariation* variation, ChessAnnotation* annotations)
{
    size_t n, max;
    assert(variation != NULL);
    max = sizeof(variation->annotations) / sizeof(ChessAnnotation);
    for (n = 0; n < max && variation->annotations[n]; ++n)
    {
        if (annotations != NULL)
            annotations[n] = variation->annotations[n];
    }
    return n;
}

size_t chess_variation_length(const ChessVariation* variation)
{
    size_t length = 0;

    assert(variation != NULL);
    while ((variation = variation->first_child))
        length++;

    return length;
}

size_t chess_variation_num_children(const ChessVariation* variation)
{
    size_t num = 0;
    assert(variation != NULL);
    for (variation = variation->first_child; variation != NULL; variation = variation->right)
        num++;
    return num;
}

ChessVariation* chess_variation_ply(ChessVariation* variation, size_t ply)
{
    assert(variation != NULL);

    while ((variation = variation->first_child) && ply > 0)
        ply--;
    return variation;
}

void chess_variation_add_annotation(ChessVariation* variation, ChessAnnotation annotation)
{
    size_t n, max;
    assert(variation != NULL && annotation != 0);
    max = sizeof(variation->annotations) / sizeof(ChessAnnotation);
    for (n = 0; n < max; ++n)
    {
        ChessAnnotation ann = variation->annotations[n];
        if (ann == 0)
        {
            variation->annotations[n] = annotation;
            break;
        }
        if (ann == annotation)
            break;
    }
}

void chess_variation_remove_annotation(ChessVariation* variation, ChessAnnotation annotation)
{
    size_t n, max;
    assert(variation != NULL && annotation != 0);
    max = sizeof(variation->annotations) / sizeof(ChessAnnotation);
    for (n = 0; n < max; ++n)
    {
        if (variation->annotations[n] == annotation)
        {
            /* Remove this annotation and shift the rest down */
            for (; n + 1 < max; ++n)
            {
                if (variation->annotations[n + 1] == 0)
                    break;
                variation->annotations[n] = variation->annotations[n + 1];
            }
            variation->annotations[n] = 0;
            return;
        }
    }
}

static ChessVariation* chess_variation_add_sibling(ChessVariation* variation, ChessMove move)
{
    ChessVariation* sibling;

    if (variation->move == move)
        return variation;

    while (variation->right)
    {
        variation = variation->right;
        if (variation->move == move)
            return variation;
    }

    sibling = new_node(variation->root);
    sibling->move = move;
    sibling->left = variation;
    sibling->parent = variation->parent;
    variation->right = sibling;
    return sibling;
}

ChessVariation* chess_variation_add_child(ChessVariation* variation, ChessMove move)
{
    ChessVariation* child;

    assert(variation != NULL);
    if (variation->first_child)
        return chess_variation_add_sibling(variation->first_child, move);

    child = new_node(variation->root);
    child->move = move;
    child->parent = variation;
    variation->first_child = child;
    return child;
}

static void set_node_root(ChessVariation* node, ChessVariation* root)
{
    node->root = root;
}

void chess_variation_attach_subvariation(ChessVariation* variation, ChessVariation* subvariation)
{
    ChessVariation* child, *attach_point;
    assert(variation != NULL);
    assert(chess_variation_is_root(subvariation));

    /* Set the root of all nodes in subvariation to variation->root */
    for_each_node(subvariation, (NodeVisitor)set_node_root, variation->root);

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
    /* Delete all subvariations */
    assert(variation != NULL);
    if (variation->first_child != NULL)
    {
        free_node_tree(variation->first_child);
        variation->first_child = NULL;
    }
}

void chess_variation_delete(ChessVariation* variation)
{
    /* Delete all subvariations, then remove the node itself */
    assert(!chess_variation_is_root(variation));
    chess_variation_truncate(variation);

    if (variation->left != NULL)
    {
        variation->left->right = variation->right;
        if (variation->right != NULL)
            variation->right->left = variation->left;
    }
    else
    {
        assert(variation->parent->first_child == variation);
        variation->parent->first_child = variation->right;
        if (variation->right != NULL)
            variation->right->left = NULL;
    }
    free_node(variation);
}

void chess_variation_promote(ChessVariation* variation)
{
    ChessVariation* sibling;
    assert(!chess_variation_is_root(variation));

    sibling = variation->left;
    if (sibling == NULL)
        return;

    if (variation->right != NULL)
        variation->right->left = sibling;

    sibling->right = variation->right;
    while (sibling->left != NULL)
        sibling = sibling->left;

    sibling->left = variation;
    variation->right = sibling;
    variation->left = NULL;
    variation->parent->first_child = variation;
}
