#include <CUnit/CUnit.h>

#include "../chess.h"
#include "../move.h"
#include "../cstring.h"
#include "../variation.h"
#include "../unmove.h"
#include "../position.h"

#include "helpers.h"

static void test_new()
{
    ChessVariation* variation = chess_variation_new(MV(E2,E4));
    CU_ASSERT_EQUAL(MV(E2,E4), chess_variation_move(variation));
    CU_ASSERT_STRING_EQUAL("", chess_string_data(chess_variation_comment(variation)));
    CU_ASSERT_EQUAL(NULL, chess_variation_parent(variation));
    CU_ASSERT_EQUAL(NULL, chess_variation_first_child(variation));
    CU_ASSERT_EQUAL(NULL, chess_variation_left(variation));
    CU_ASSERT_EQUAL(NULL, chess_variation_right(variation));
    chess_variation_destroy(variation);
}

static void test_length()
{
    ChessVariation* variation = NULL;
    CU_ASSERT_EQUAL(0, chess_variation_length(variation));

    variation = chess_variation_new(MV(G1,F3));
    CU_ASSERT_EQUAL(1, chess_variation_length(variation));
    chess_variation_add_child(variation, MV(D7,D5));
    CU_ASSERT_EQUAL(2, chess_variation_length(variation));
    chess_variation_add_sibling(variation, MV(E2,E4));
    CU_ASSERT_EQUAL(2, chess_variation_length(variation));
    chess_variation_add_child(chess_variation_first_child(variation), MV(C2,C4));
    CU_ASSERT_EQUAL(3, chess_variation_length(variation));
    chess_variation_destroy(variation);
}

static void test_add_child()
{
    ChessVariation* variation, *child, *child2;

    variation = chess_variation_add_child(NULL, MV(E2,E4));
    CU_ASSERT_EQUAL(NULL, chess_variation_parent(variation));
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

    chess_variation_destroy(variation);
}

static void test_add_sibling()
{
    ChessVariation* variation, *sibling, *sibling2;

    variation = chess_variation_new(MV(E2,E4));
    sibling = chess_variation_add_sibling(variation, MV(D2,D4));
    CU_ASSERT_EQUAL(sibling, chess_variation_right(variation));
    CU_ASSERT_EQUAL(NULL, chess_variation_parent(variation));
    CU_ASSERT_EQUAL(NULL, chess_variation_first_child(variation));
    CU_ASSERT_EQUAL(NULL, chess_variation_left(variation));
    CU_ASSERT_EQUAL(variation, chess_variation_left(sibling));
    CU_ASSERT_EQUAL(NULL, chess_variation_parent(sibling));
    CU_ASSERT_EQUAL(NULL, chess_variation_first_child(sibling));
    CU_ASSERT_EQUAL(NULL, chess_variation_right(sibling));
    CU_ASSERT_EQUAL(MV(D2,D4), chess_variation_move(sibling));

    sibling2 = chess_variation_add_sibling(variation, MV(C2,C4));
    CU_ASSERT_EQUAL(sibling2, chess_variation_right(sibling));
    CU_ASSERT_EQUAL(sibling, chess_variation_left(sibling2));
    CU_ASSERT_EQUAL(NULL, chess_variation_parent(sibling2));
    CU_ASSERT_EQUAL(NULL, chess_variation_first_child(sibling2));
    CU_ASSERT_EQUAL(NULL, chess_variation_right(sibling2));
    CU_ASSERT_EQUAL(MV(C2,C4), chess_variation_move(sibling2));

    chess_variation_destroy(variation);
}

static void test_destroy()
{
    ChessVariation* variation, *child, *sibling;

    chess_variation_destroy(NULL);

    variation = chess_variation_new(MV(E2,E4));
    child = chess_variation_add_child(variation, MV(C7,C5));
    CU_ASSERT_EQUAL(child, chess_variation_first_child(variation));
    chess_variation_destroy(child);
    CU_ASSERT_EQUAL(NULL, chess_variation_first_child(variation));

    sibling = chess_variation_add_sibling(variation, MV(D2,D4));
    CU_ASSERT_EQUAL(sibling, chess_variation_right(variation));
    chess_variation_destroy(sibling);
    CU_ASSERT_EQUAL(NULL, chess_variation_right(variation));

    child = chess_variation_add_child(variation, MV(C7,C5));
    sibling = chess_variation_add_sibling(child, MV(E7,E6));
    chess_variation_destroy(child);
    CU_ASSERT_EQUAL(NULL, chess_variation_first_child(variation));

    child = chess_variation_add_child(variation, MV(C7,C5));
    sibling = chess_variation_add_sibling(child, MV(E7,E6));
    chess_variation_destroy(sibling);
    CU_ASSERT_EQUAL(child, chess_variation_first_child(variation));
    CU_ASSERT_EQUAL(variation, chess_variation_parent(child));
    CU_ASSERT_EQUAL(NULL, chess_variation_right(child));

    chess_variation_destroy(variation);
}

void test_variation_add_tests()
{
    CU_Suite* suite = CU_add_suite("variation", NULL, NULL);
    CU_add_test(suite, "new", (CU_TestFunc)test_new);
    CU_add_test(suite, "length", (CU_TestFunc)test_length);
    CU_add_test(suite, "add_child", (CU_TestFunc)test_add_child);
    CU_add_test(suite, "add_sibling", (CU_TestFunc)test_add_sibling);
    CU_add_test(suite, "destroy", (CU_TestFunc)test_destroy);
}
