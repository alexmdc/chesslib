#include <CUnit/CUnit.h>

#include "../variation.h"

#include "helpers.h"

static void test_new(void)
{
    ChessVariation* root = chess_variation_new();
    CU_ASSERT_TRUE(chess_variation_is_root(root));
    CU_ASSERT_EQUAL(root, chess_variation_root(root));
    CU_ASSERT_EQUAL(NULL, chess_variation_parent(root));
    CU_ASSERT_EQUAL(NULL, chess_variation_first_child(root));
    CU_ASSERT_EQUAL(NULL, chess_variation_left(root));
    CU_ASSERT_EQUAL(NULL, chess_variation_right(root));
    CU_ASSERT_STRING_EQUAL("", chess_string_data(chess_variation_comment(root)));
    CU_ASSERT_EQUAL(0, chess_variation_move(root));
    chess_variation_destroy(root);
}

static void test_annotations(void)
{
    ChessVariation* root, *variation;
    ChessAnnotation annotations[4] = { 0, 0, 0, 0 };

    root = chess_variation_new();
    variation = chess_variation_add_child(root, MV(E2,E4));
    CU_ASSERT_EQUAL(0, chess_variation_annotations(variation, annotations));

    chess_variation_add_annotation(variation, 3);
    CU_ASSERT_EQUAL(1, chess_variation_annotations(variation, annotations));
    CU_ASSERT_EQUAL(3, annotations[0]);

    chess_variation_add_annotation(variation, 7);
    CU_ASSERT_EQUAL(2, chess_variation_annotations(variation, annotations));
    CU_ASSERT_EQUAL(3, annotations[0]);
    CU_ASSERT_EQUAL(7, annotations[1]);

    /* Adding the same annotation twice should fail */
    chess_variation_add_annotation(variation, 3);
    CU_ASSERT_EQUAL(2, chess_variation_annotations(variation, annotations));
    CU_ASSERT_EQUAL(3, annotations[0]);
    CU_ASSERT_EQUAL(7, annotations[1]);

    chess_variation_add_annotation(variation, 128);
    chess_variation_add_annotation(variation, 255);
    CU_ASSERT_EQUAL(4, chess_variation_annotations(variation, annotations));
    CU_ASSERT_EQUAL(3, annotations[0]);
    CU_ASSERT_EQUAL(7, annotations[1]);
    CU_ASSERT_EQUAL(128, annotations[2]);
    CU_ASSERT_EQUAL(255, annotations[3]);

    /* Adding more than 4 should fail (for now) */
    chess_variation_add_annotation(variation, 12);
    CU_ASSERT_EQUAL(4, chess_variation_annotations(variation, annotations));
    CU_ASSERT_EQUAL(3, annotations[0]);
    CU_ASSERT_EQUAL(7, annotations[1]);
    CU_ASSERT_EQUAL(128, annotations[2]);
    CU_ASSERT_EQUAL(255, annotations[3]);

    /* Passing in a NULL should just give us the count */
    CU_ASSERT_EQUAL(4, chess_variation_annotations(variation, NULL));

    /* Try removing the last one */
    chess_variation_remove_annotation(variation, 255);
    CU_ASSERT_EQUAL(3, chess_variation_annotations(variation, annotations));
    CU_ASSERT_EQUAL(3, annotations[0]);
    CU_ASSERT_EQUAL(7, annotations[1]);
    CU_ASSERT_EQUAL(128, annotations[2]);

    /* Try removing the first one */
    chess_variation_remove_annotation(variation, 3);
    CU_ASSERT_EQUAL(2, chess_variation_annotations(variation, annotations));
    CU_ASSERT_EQUAL(7, annotations[0]);
    CU_ASSERT_EQUAL(128, annotations[1]);

    /* Try removing nonexistant one */
    chess_variation_remove_annotation(variation, 12);
    CU_ASSERT_EQUAL(2, chess_variation_annotations(variation, annotations));
    CU_ASSERT_EQUAL(7, annotations[0]);
    CU_ASSERT_EQUAL(128, annotations[1]);

    /* Try removing the rest */
    chess_variation_remove_annotation(variation, 7);
    chess_variation_remove_annotation(variation, 128);
    CU_ASSERT_EQUAL(0, chess_variation_annotations(variation, annotations));
}

static void test_add_child(void)
{
    ChessVariation* root, *child, *grandchild, *grandchild2;

    root = chess_variation_new();
    child = chess_variation_add_child(root, MV(E2,E4));
    CU_ASSERT_FALSE(chess_variation_is_root(child));
    CU_ASSERT_EQUAL(child, chess_variation_first_child(root));
    CU_ASSERT_EQUAL(root, chess_variation_root(child));
    CU_ASSERT_EQUAL(root, chess_variation_parent(child));
    CU_ASSERT_EQUAL(NULL, chess_variation_left(child));
    CU_ASSERT_EQUAL(NULL, chess_variation_right(child));
    CU_ASSERT_EQUAL(NULL, chess_variation_first_child(child));
    CU_ASSERT_EQUAL(MV(E2,E4), chess_variation_move(child));

    grandchild = chess_variation_add_child(child, MV(C7,C5));
    CU_ASSERT_FALSE(chess_variation_is_root(grandchild));
    CU_ASSERT_EQUAL(root, chess_variation_root(grandchild));
    CU_ASSERT_EQUAL(child, chess_variation_parent(grandchild));
    CU_ASSERT_EQUAL(grandchild, chess_variation_first_child(child));
    CU_ASSERT_EQUAL(MV(C7,C5), chess_variation_move(grandchild));
    CU_ASSERT_EQUAL(NULL, chess_variation_left(grandchild));
    CU_ASSERT_EQUAL(NULL, chess_variation_right(grandchild));
    CU_ASSERT_EQUAL(NULL, chess_variation_first_child(grandchild));

    /* Adding the same move again should use the existing variation */
    grandchild2 = chess_variation_add_child(child, MV(C7,C5));
    CU_ASSERT_EQUAL(grandchild, grandchild2);
    CU_ASSERT_EQUAL(NULL, chess_variation_left(grandchild));
    CU_ASSERT_EQUAL(NULL, chess_variation_right(grandchild));
    CU_ASSERT_EQUAL(NULL, chess_variation_first_child(grandchild));

    chess_variation_destroy(root);
}

static void test_add_sibling(void)
{
    ChessVariation* root, *child, *sibling, *sibling2;

    root = chess_variation_new();
    child = chess_variation_add_child(root, MV(E2,E4));
    sibling = chess_variation_add_child(root, MV(D2,D4));
    CU_ASSERT_FALSE(chess_variation_is_root(sibling));
    CU_ASSERT_EQUAL(root, chess_variation_root(sibling));
    CU_ASSERT_EQUAL(root, chess_variation_parent(sibling));
    CU_ASSERT_EQUAL(NULL, chess_variation_first_child(child));
    CU_ASSERT_EQUAL(NULL, chess_variation_first_child(sibling));
    CU_ASSERT_EQUAL(NULL, chess_variation_left(child));
    CU_ASSERT_EQUAL(sibling, chess_variation_right(child));
    CU_ASSERT_EQUAL(child, chess_variation_left(sibling));
    CU_ASSERT_EQUAL(NULL, chess_variation_right(sibling));
    CU_ASSERT_EQUAL(MV(D2,D4), chess_variation_move(sibling));

    sibling2 = chess_variation_add_child(root, MV(C2,C4));
    CU_ASSERT_FALSE(chess_variation_is_root(sibling2));
    CU_ASSERT_EQUAL(root, chess_variation_root(sibling2));
    CU_ASSERT_EQUAL(root, chess_variation_parent(sibling2));
    CU_ASSERT_EQUAL(NULL, chess_variation_first_child(sibling));
    CU_ASSERT_EQUAL(NULL, chess_variation_first_child(sibling2));
    CU_ASSERT_EQUAL(child, chess_variation_left(sibling));
    CU_ASSERT_EQUAL(sibling2, chess_variation_right(sibling));
    CU_ASSERT_EQUAL(sibling, chess_variation_left(sibling2));
    CU_ASSERT_EQUAL(NULL, chess_variation_right(sibling2));
    CU_ASSERT_EQUAL(MV(C2,C4), chess_variation_move(sibling2));

    /* Adding the same move should use existing variation */
    sibling2 = chess_variation_add_child(root, MV(D2,D4));
    CU_ASSERT_EQUAL(sibling, sibling2);

    chess_variation_destroy(root);
}

static void test_length(void)
{
    ChessVariation* root, *variation;

    root = chess_variation_new();
    CU_ASSERT_EQUAL(0, chess_variation_length(root));

    variation = chess_variation_add_child(root, MV(G1,F3));
    CU_ASSERT_EQUAL(1, chess_variation_length(root));
    chess_variation_add_child(variation, MV(D7,D5));
    CU_ASSERT_EQUAL(2, chess_variation_length(root));

    chess_variation_add_child(root, MV(E2,E4));
    CU_ASSERT_EQUAL(2, chess_variation_length(root));
    chess_variation_add_child(chess_variation_first_child(variation), MV(C2,C4));
    CU_ASSERT_EQUAL(3, chess_variation_length(root));
    chess_variation_destroy(root);
}

static void test_num_children(void)
{
    ChessVariation* root, *child, *grandchild;

    root = chess_variation_new();
    CU_ASSERT_EQUAL(0, chess_variation_num_children(root));

    child = chess_variation_add_child(root, MV(G1,F3));
    CU_ASSERT_EQUAL(1, chess_variation_num_children(root));
    CU_ASSERT_EQUAL(0, chess_variation_num_children(child));
    grandchild = chess_variation_add_child(child, MV(D7,D5));
    CU_ASSERT_EQUAL(1, chess_variation_num_children(root));
    CU_ASSERT_EQUAL(1, chess_variation_num_children(child))
    CU_ASSERT_EQUAL(0, chess_variation_num_children(grandchild));

    chess_variation_add_child(root, MV(E2,E4));
    CU_ASSERT_EQUAL(2, chess_variation_num_children(root));
    CU_ASSERT_EQUAL(1, chess_variation_num_children(child))
    CU_ASSERT_EQUAL(0, chess_variation_num_children(grandchild));

    chess_variation_add_child(child, MV(C2,C4));
    CU_ASSERT_EQUAL(2, chess_variation_num_children(root));
    CU_ASSERT_EQUAL(2, chess_variation_num_children(child))
    CU_ASSERT_EQUAL(0, chess_variation_num_children(grandchild));

    chess_variation_destroy(root);
}

static void test_destroy(void)
{
    ChessVariation* root, *child, *child2, *grandchild;

    root = chess_variation_new();
    chess_variation_destroy(root);

    root = chess_variation_new();
    child = chess_variation_add_child(root, MV(E2,E4));
    child2 = chess_variation_add_child(root, MV(D2,D4));
    grandchild = chess_variation_add_child(child, MV(C7,C5));
    chess_variation_add_child(child, MV(E7,E6));
    chess_variation_destroy(root);
}

static void test_truncate(void)
{
    ChessVariation* root, *child;

    root = chess_variation_new();
    chess_variation_truncate(root);
    CU_ASSERT_EQUAL(0, chess_variation_num_children(root));

    child = chess_variation_add_child(root, MV(E2,E4));
    CU_ASSERT_EQUAL(1, chess_variation_num_children(root));
    chess_variation_truncate(child);
    CU_ASSERT_EQUAL(1, chess_variation_num_children(root));
    chess_variation_truncate(root);
    CU_ASSERT_EQUAL(0, chess_variation_num_children(root));

    child = chess_variation_add_child(root, MV(E2,E4));
    chess_variation_add_child(child, MV(E7,E5));
    chess_variation_add_child(child, MV(D7,D5));
    CU_ASSERT_EQUAL(2, chess_variation_length(root));
    CU_ASSERT_EQUAL(2, chess_variation_num_children(child));
    chess_variation_truncate(child);
    CU_ASSERT_EQUAL(1, chess_variation_length(root));

    chess_variation_destroy(root);
}

static void test_delete(void)
{
    ChessVariation* root, *child, *child2, *child3, *grandchild;

    root = chess_variation_new();

    child = chess_variation_add_child(root, MV(E2,E4));
    CU_ASSERT_EQUAL(1, chess_variation_num_children(root));
    chess_variation_delete(child);
    CU_ASSERT_EQUAL(0, chess_variation_num_children(root));

    child = chess_variation_add_child(root, MV(E2,E4));
    grandchild = chess_variation_add_child(child, MV(E7,E5));
    chess_variation_add_child(child, MV(D7,D5));
    CU_ASSERT_EQUAL(grandchild, chess_variation_first_child(child));
    CU_ASSERT_EQUAL(2, chess_variation_num_children(child));
    chess_variation_delete(grandchild);
    CU_ASSERT_EQUAL(1, chess_variation_num_children(child));
    grandchild = chess_variation_first_child(child);
    CU_ASSERT_EQUAL(MV(D7,D5), chess_variation_move(grandchild));
    chess_variation_delete(grandchild);
    CU_ASSERT_EQUAL(0, chess_variation_num_children(child));

    child2 = chess_variation_add_child(root, MV(D2,D4));
    child3 = chess_variation_add_child(root, MV(C2,C4));
    CU_ASSERT_EQUAL(child2, chess_variation_right(child));
    CU_ASSERT_EQUAL(child, chess_variation_left(child2));
    CU_ASSERT_EQUAL(child3, chess_variation_right(child2));
    CU_ASSERT_EQUAL(child2, chess_variation_left(child3));
    chess_variation_delete(child2);
    CU_ASSERT_EQUAL(NULL, chess_variation_left(child));
    CU_ASSERT_EQUAL(child3, chess_variation_right(child));
    CU_ASSERT_EQUAL(child, chess_variation_left(child3));
    CU_ASSERT_EQUAL(NULL, chess_variation_right(child3));

    chess_variation_destroy(root);
}

static void test_promote(void)
{
    ChessVariation* root, *child1, *child2, *child3;
    root = chess_variation_new();

    child1 = chess_variation_add_child(root, MV(E2,E4));
    child2 = chess_variation_add_child(root, MV(D2,D4));
    CU_ASSERT_EQUAL(child1, chess_variation_first_child(root));

    chess_variation_promote(child2);
    CU_ASSERT_EQUAL(child2, chess_variation_first_child(root));
    CU_ASSERT_EQUAL(NULL, chess_variation_left(child2));
    CU_ASSERT_EQUAL(child1, chess_variation_right(child2));
    CU_ASSERT_EQUAL(child2, chess_variation_left(child1));
    CU_ASSERT_EQUAL(NULL, chess_variation_right(child1));

    child3 = chess_variation_add_child(root, MV(C2,C4));
    CU_ASSERT_EQUAL(child3, chess_variation_right(child1));
    CU_ASSERT_EQUAL(child1, chess_variation_left(child3));
    CU_ASSERT_EQUAL(NULL, chess_variation_right(child3));

    chess_variation_promote(child1);
    CU_ASSERT_EQUAL(child1, chess_variation_first_child(root));
    CU_ASSERT_EQUAL(NULL, chess_variation_left(child1));
    CU_ASSERT_EQUAL(child2, chess_variation_right(child1));
    CU_ASSERT_EQUAL(child1, chess_variation_left(child2));
    CU_ASSERT_EQUAL(child3, chess_variation_right(child2));
    CU_ASSERT_EQUAL(child2, chess_variation_left(child3));
    CU_ASSERT_EQUAL(NULL, chess_variation_right(child3));
}

void test_variation_add_tests(void)
{
    CU_Suite* suite = add_suite("variation");
    CU_add_test(suite, "new", (CU_TestFunc)test_new);
    CU_add_test(suite, "annotations", (CU_TestFunc)test_annotations);
    CU_add_test(suite, "add_child", (CU_TestFunc)test_add_child);
    CU_add_test(suite, "add_sibling", (CU_TestFunc)test_add_sibling);
    CU_add_test(suite, "length", (CU_TestFunc)test_length);
    CU_add_test(suite, "num_children", (CU_TestFunc)test_num_children);
    CU_add_test(suite, "destroy", (CU_TestFunc)test_destroy);
    CU_add_test(suite, "truncate", (CU_TestFunc)test_truncate);
    CU_add_test(suite, "promote", (CU_TestFunc)test_promote);
    CU_add_test(suite, "delete", (CU_TestFunc)test_delete);
}
