#include <CUnit/CUnit.h>

#include "../cstring.h"

#include "helpers.h"

static void test_cstring_init(void)
{
    ChessString string;

    chess_string_init(&string);
    CU_ASSERT_EQUAL(0, string.size);
    CU_ASSERT_STRING_EQUAL("", string.data);
    chess_string_cleanup(&string);

    chess_string_init_assign(&string, "");
    CU_ASSERT_EQUAL(0, string.size);
    CU_ASSERT_STRING_EQUAL("", string.data);
    chess_string_cleanup(&string);

    chess_string_init_assign(&string, "White");
    CU_ASSERT_EQUAL(5, string.size);
    CU_ASSERT_STRING_EQUAL("White", string.data);
    chess_string_cleanup(&string);

    chess_string_init(&string);
    CU_ASSERT_EQUAL(0, string.size);
    CU_ASSERT_STRING_EQUAL("", string.data);
    chess_string_cleanup(&string);
}

static void test_cstring_clear(void)
{
    ChessString string;

    chess_string_init(&string);
    chess_string_clear(&string);
    CU_ASSERT_EQUAL(0, string.size);
    CU_ASSERT_STRING_EQUAL("", string.data);

    chess_string_init_assign(&string, "Black");
    CU_ASSERT_EQUAL(5, string.size);
    CU_ASSERT_STRING_EQUAL("Black", string.data);
    chess_string_clear(&string);
    CU_ASSERT_EQUAL(0, string.size);
    CU_ASSERT_STRING_EQUAL("", string.data);
    chess_string_cleanup(&string);
}

static void test_cstring_assign(void)
{
    ChessString string;

    chess_string_init(&string);
    chess_string_assign(&string, "Wilhelm Steinitz");
    CU_ASSERT_EQUAL(16, string.size);
    CU_ASSERT_STRING_EQUAL("Wilhelm Steinitz", string.data);

    chess_string_assign(&string, "Emanuel Lasker");
    CU_ASSERT_EQUAL(14, string.size);
    CU_ASSERT_STRING_EQUAL("Emanuel Lasker", string.data);

    chess_string_assign(&string, "");
    CU_ASSERT_EQUAL(0, string.size);
    CU_ASSERT_STRING_EQUAL("", string.data);
    chess_string_cleanup(&string);
}

void test_cstring_add_tests(void)
{
    CU_Suite* suite = add_suite("cstring");
    CU_add_test(suite, "cstring_init", (CU_TestFunc)test_cstring_init);
    CU_add_test(suite, "cstring_clear", (CU_TestFunc)test_cstring_clear);
    CU_add_test(suite, "cstring_assign", (CU_TestFunc)test_cstring_assign);
}
