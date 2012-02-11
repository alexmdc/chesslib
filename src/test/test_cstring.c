#include <CUnit/CUnit.h>

#include "../cstring.h"

static void test_cstring_init()
{
    ChessString string;

    chess_string_init(&string);
    CU_ASSERT_EQUAL(0, chess_string_size(&string));
    CU_ASSERT_STRING_EQUAL("", chess_string_data(&string));
    chess_string_cleanup(&string);

    chess_string_init_assign(&string, "");
    CU_ASSERT_EQUAL(0, chess_string_size(&string));
    CU_ASSERT_STRING_EQUAL("", chess_string_data(&string));
    chess_string_cleanup(&string);

    chess_string_init_assign(&string, "White");
    CU_ASSERT_EQUAL(5, chess_string_size(&string));
    CU_ASSERT_STRING_EQUAL("White", chess_string_data(&string));
    chess_string_cleanup(&string);

    chess_string_init(&string);
    CU_ASSERT_EQUAL(0, chess_string_size(&string));
    CU_ASSERT_STRING_EQUAL("", chess_string_data(&string));
    chess_string_cleanup(&string);
}

static void test_cstring_clear()
{
    ChessString string;
    
    chess_string_init(&string);
    chess_string_clear(&string);
    CU_ASSERT_EQUAL(0, chess_string_size(&string));
    CU_ASSERT_STRING_EQUAL("", chess_string_data(&string));
    
    chess_string_init_assign(&string, "Black");
    CU_ASSERT_EQUAL(5, chess_string_size(&string));
    CU_ASSERT_STRING_EQUAL("Black", chess_string_data(&string));
    chess_string_clear(&string);
    CU_ASSERT_EQUAL(0, chess_string_size(&string));
    CU_ASSERT_STRING_EQUAL("", chess_string_data(&string));
    chess_string_cleanup(&string);
}

static void test_cstring_assign()
{
    ChessString string;
    
    chess_string_init(&string);
    chess_string_assign(&string, "Wilhelm Steinitz");
    CU_ASSERT_EQUAL(16, chess_string_size(&string));
    CU_ASSERT_STRING_EQUAL("Wilhelm Steinitz", chess_string_data(&string));

    chess_string_assign(&string, "Emanuel Lasker");
    CU_ASSERT_EQUAL(14, chess_string_size(&string));
    CU_ASSERT_STRING_EQUAL("Emanuel Lasker", chess_string_data(&string));

    chess_string_assign(&string, "");
    CU_ASSERT_EQUAL(0, chess_string_size(&string));
    CU_ASSERT_STRING_EQUAL("", chess_string_data(&string));
    chess_string_cleanup(&string);
}

void test_cstring_add_tests()
{
    CU_Suite* suite = CU_add_suite("cstring", NULL, NULL);
    CU_add_test(suite, "cstring_init", (CU_TestFunc)test_cstring_init);
    CU_add_test(suite, "cstring_clear", (CU_TestFunc)test_cstring_clear);
    CU_add_test(suite, "cstring_assign", (CU_TestFunc)test_cstring_assign);
}
