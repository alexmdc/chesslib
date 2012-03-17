#include <CUnit/CUnit.h>

#include "../variation.h"

#include "helpers.h"

static void test_new(void)
{
    ChessVariation* root = chess_variation_new();
    CU_ASSERT_EQUAL(NULL, chess_variation_parent(root));
    CU_ASSERT_EQUAL(NULL, chess_variation_first_child(root));
    CU_ASSERT_EQUAL(NULL, chess_variation_left(root));
    CU_ASSERT_EQUAL(NULL, chess_variation_right(root));
    CU_ASSERT_STRING_EQUAL("", chess_string_data(chess_variation_comment(root)));
    CU_ASSERT_EQUAL(0, chess_variation_move(root));
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
    chess_variation_destroy(variation);
}

static void test_add_child(void)
{
    ChessVariation* variation, *child, *child2;
    ChessVariation* root = chess_variation_new();

    variation = chess_variation_add_child(root, MV(E2,E4));
    CU_ASSERT_EQUAL(root, chess_variation_parent(variation));
    CU_ASSERT_EQUAL(MV(E2,E4), chess_variation_move(variation));

    child = chess_variation_add_child(variation, MV(C7,C5));
    CU_ASSERT_EQUAL(child, chess_variation_first_child(variation));
    CU_ASSERT_EQUAL(variation, chess_variation_parent(child));
    CU_ASSERT_EQUAL(MV(C7,C5), chess_variation_move(child));

    child2 = chess_variation_add_child(variation, MV(E7,E6));
    CU_ASSERT_EQUAL(child, chess_variation_first_child(variation));
    CU_ASSERT_EQUAL(variation, chess_variation_parent(child2));
    CU_ASSERT_EQUAL(child2, chess_variation_right(child));
    CU_ASSERT_EQUAL(child, chess_variation_left(child2));
    CU_ASSERT_EQUAL(MV(E7,E6), chess_variation_move(child2));

    chess_variation_destroy(root);
}

static void test_add_sibling(void)
{
    ChessVariation* variation, *sibling, *sibling2;
    ChessVariation* root = chess_variation_new();

    variation = chess_variation_add_child(root, MV(E2,E4));
    sibling = chess_variation_add_child(root, MV(D2,D4));
    CU_ASSERT_EQUAL(sibling, chess_variation_right(variation));
    CU_ASSERT_EQUAL(root, chess_variation_parent(variation));
    CU_ASSERT_EQUAL(NULL, chess_variation_first_child(variation));
    CU_ASSERT_EQUAL(NULL, chess_variation_left(variation));
    CU_ASSERT_EQUAL(variation, chess_variation_left(sibling));
    CU_ASSERT_EQUAL(root, chess_variation_parent(sibling));
    CU_ASSERT_EQUAL(NULL, chess_variation_first_child(sibling));
    CU_ASSERT_EQUAL(NULL, chess_variation_right(sibling));
    CU_ASSERT_EQUAL(MV(D2,D4), chess_variation_move(sibling));

    sibling2 = chess_variation_add_child(root, MV(C2,C4));
    CU_ASSERT_EQUAL(sibling2, chess_variation_right(sibling));
    CU_ASSERT_EQUAL(sibling, chess_variation_left(sibling2));
    CU_ASSERT_EQUAL(root, chess_variation_parent(sibling2));
    CU_ASSERT_EQUAL(NULL, chess_variation_first_child(sibling2));
    CU_ASSERT_EQUAL(NULL, chess_variation_right(sibling2));
    CU_ASSERT_EQUAL(MV(C2,C4), chess_variation_move(sibling2));

    chess_variation_destroy(root);
}

static void test_destroy(void)
{
    ChessVariation* root, *variation, *child, *sibling;

    root = chess_variation_new();
    chess_variation_destroy(root);

    root = chess_variation_new();
    variation = chess_variation_add_child(root, MV(E2,E4));
    child = chess_variation_add_child(variation, MV(C7,C5));
    CU_ASSERT_EQUAL(child, chess_variation_first_child(variation));
    chess_variation_destroy(child);
    CU_ASSERT_EQUAL(NULL, chess_variation_first_child(variation));

    sibling = chess_variation_add_child(root, MV(D2,D4));
    CU_ASSERT_EQUAL(sibling, chess_variation_right(variation));
    chess_variation_destroy(sibling);
    CU_ASSERT_EQUAL(NULL, chess_variation_right(variation));

    child = chess_variation_add_child(variation, MV(C7,C5));
    sibling = chess_variation_add_child(variation, MV(E7,E6));
    chess_variation_destroy(child);
    CU_ASSERT_EQUAL(NULL, chess_variation_first_child(variation));

    child = chess_variation_add_child(variation, MV(C7,C5));
    sibling = chess_variation_add_child(variation, MV(E7,E6));
    chess_variation_destroy(sibling);
    CU_ASSERT_EQUAL(child, chess_variation_first_child(variation));
    CU_ASSERT_EQUAL(variation, chess_variation_parent(child));
    CU_ASSERT_EQUAL(NULL, chess_variation_right(child));

    chess_variation_destroy(root);
}

void test_variation_add_tests(void)
{
    CU_Suite* suite = CU_add_suite("variation", NULL, NULL);
    CU_add_test(suite, "new", (CU_TestFunc)test_new);
    CU_add_test(suite, "length", (CU_TestFunc)test_length);
    CU_add_test(suite, "add_child", (CU_TestFunc)test_add_child);
    CU_add_test(suite, "add_sibling", (CU_TestFunc)test_add_sibling);
    CU_add_test(suite, "destroy", (CU_TestFunc)test_destroy);
}
