#include <CUnit/CUnit.h>

#include "../cbuffer.h"

#include "helpers.h"

static void test_cbuffer_init(void)
{
    ChessBuffer buffer;

    chess_buffer_init(&buffer);
    CU_ASSERT_EQUAL(0, chess_buffer_size(&buffer));
    chess_buffer_cleanup(&buffer);
}

static void test_cbuffer_append(void)
{
    ChessBuffer buffer;

    chess_buffer_init(&buffer);

    chess_buffer_append_char(&buffer, 'A');
    CU_ASSERT_EQUAL(chess_buffer_size(&buffer), 1);
    CU_ASSERT_NSTRING_EQUAL(chess_buffer_data(&buffer), "A", 1);

    chess_buffer_append_char(&buffer, 'M');
    CU_ASSERT_EQUAL(chess_buffer_size(&buffer), 2);
    CU_ASSERT_NSTRING_EQUAL(chess_buffer_data(&buffer), "AM", 2);

    chess_buffer_append_string(&buffer, "opit");
    CU_ASSERT_EQUAL(chess_buffer_size(&buffer), 6);
    CU_ASSERT_NSTRING_EQUAL(chess_buffer_data(&buffer), "AMopit", 6);

    chess_buffer_append_string_size(&buffer, "47stop", 2);
    CU_ASSERT_EQUAL(chess_buffer_size(&buffer), 8);
    CU_ASSERT_NSTRING_EQUAL(chess_buffer_data(&buffer), "AMopit47", 8);

    chess_buffer_cleanup(&buffer);
}

static void test_cbuffer_set_size(void)
{
    ChessBuffer buffer;
    chess_buffer_init(&buffer);

    chess_buffer_set_size(&buffer, 1);
    CU_ASSERT_EQUAL(chess_buffer_size(&buffer), 1);
    chess_buffer_data(&buffer)[0] = 'A';

    chess_buffer_set_size(&buffer, 1000);
    CU_ASSERT_EQUAL(chess_buffer_size(&buffer), 1000);
    CU_ASSERT_EQUAL(chess_buffer_data(&buffer)[0], 'A');
    chess_buffer_data(&buffer)[999] = 'Z';

    chess_buffer_set_size(&buffer, 10000);
    CU_ASSERT_EQUAL(chess_buffer_size(&buffer), 10000);
    CU_ASSERT_EQUAL(chess_buffer_data(&buffer)[0], 'A');
    CU_ASSERT_EQUAL(chess_buffer_data(&buffer)[999], 'Z');
    chess_buffer_data(&buffer)[9999] = '!';

    chess_buffer_set_size(&buffer, 7);
    CU_ASSERT_EQUAL(chess_buffer_size(&buffer), 7);
    CU_ASSERT_EQUAL(chess_buffer_data(&buffer)[0], 'A');

    chess_buffer_cleanup(&buffer);
}

static void test_cbuffer_clear(void)
{
    ChessBuffer buffer;
    chess_buffer_init(&buffer);

    chess_buffer_clear(&buffer);
    CU_ASSERT_EQUAL(chess_buffer_size(&buffer), 0);

    chess_buffer_append_string(&buffer, "Hello, buffer!");
    CU_ASSERT_EQUAL(chess_buffer_size(&buffer), 14);

    chess_buffer_clear(&buffer);
    CU_ASSERT_EQUAL(chess_buffer_size(&buffer), 0);

    chess_buffer_cleanup(&buffer);
}

static void test_cbuffer_null_terminate(void)
{
    ChessBuffer buffer;
    chess_buffer_init(&buffer);

    chess_buffer_null_terminate(&buffer);
    CU_ASSERT_EQUAL(chess_buffer_size(&buffer), 0);
    CU_ASSERT_EQUAL(chess_buffer_data(&buffer)[0], '\0');

    chess_buffer_append_string(&buffer, "Hello, buffer!");
    CU_ASSERT_EQUAL(chess_buffer_size(&buffer), 14);

    chess_buffer_null_terminate(&buffer);
    CU_ASSERT_EQUAL(chess_buffer_size(&buffer), 14);
    CU_ASSERT_EQUAL(chess_buffer_data(&buffer)[14], '\0');

    chess_buffer_set_size(&buffer, 3);
    chess_buffer_null_terminate(&buffer);
    CU_ASSERT_EQUAL(chess_buffer_size(&buffer), 3);
    CU_ASSERT_EQUAL(chess_buffer_data(&buffer)[3], '\0');

    chess_buffer_cleanup(&buffer);
}

void test_cbuffer_add_tests(void)
{
    CU_Suite* suite = add_suite("cstring");
    CU_add_test(suite, "cbuffer_init", (CU_TestFunc)test_cbuffer_init);
    CU_add_test(suite, "cbuffer_append", (CU_TestFunc)test_cbuffer_append);
    CU_add_test(suite, "cbuffer_set_size", (CU_TestFunc)test_cbuffer_set_size);
    CU_add_test(suite, "cbuffer_clear", (CU_TestFunc)test_cbuffer_clear);
    CU_add_test(suite, "cbuffer_null_terminate", (CU_TestFunc)test_cbuffer_null_terminate);
}
