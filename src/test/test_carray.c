#include <CUnit/CUnit.h>

#include "../chess.h"
#include "../move.h"
#include "../unmove.h"
#include "../position.h"
#include "../carray.h"

#include "helpers.h"

static void test_carray_init()
{
    ChessArray array;
    
    chess_array_init(&array, sizeof(int));
    CU_ASSERT_EQUAL(0, chess_array_size(&array));
    chess_array_cleanup(&array);
}

static void test_carray_push()
{
    const int values[] = { 2, 3, 5, 7, 11, 13, 17, 19, 23, 29 };
    const int* value;
    const size_t values_size = sizeof(values) / sizeof(int);
    int i;
    ChessArray array;

    chess_array_init(&array, sizeof(int));

    for (i = 0; i < values_size; i++)
    {
        chess_array_push(&array, &values[i]);
        CU_ASSERT_EQUAL(i + 1, chess_array_size(&array))
        value = chess_array_elem(&array, i);
        CU_ASSERT_EQUAL(values[i], *value);
    }

    value = chess_array_data(&array);
    ASSERT_ARRAYS_EQUAL(values, values_size, value, chess_array_size(&array));

    chess_array_cleanup(&array);
}

static void test_carray_pop()
{
    const double values[] = { 10.0, 5.0, 2.5, 1.25, 0.625 };
    double value;
    const size_t values_size = sizeof(values) / sizeof(double);
    int i;
    ChessArray array;
    
    chess_array_init(&array, sizeof(double));
    
    for (i = 0; i < values_size; i++)
    {
        chess_array_push(&array, &values[i]);
    }
    CU_ASSERT_EQUAL(values_size, chess_array_size(&array));
    
    for (i = values_size - 1; i >= 0; i--)
    {
        chess_array_pop(&array, &value);
        CU_ASSERT_EQUAL(i, chess_array_size(&array));
        CU_ASSERT_EQUAL(values[i], value);
    }

    chess_array_cleanup(&array);
}

static void test_carray_clear()
{
    const int values[] = { 1, 4, 9, 16, 25 };
    ChessArray array;
    
    chess_array_init(&array, sizeof(int));
    chess_array_clear(&array);
    CU_ASSERT_EQUAL(0, chess_array_size(&array));

    chess_array_push(&array, &values[0]);
    chess_array_push(&array, &values[1]);
    CU_ASSERT_EQUAL(2, chess_array_size(&array));
    chess_array_clear(&array);
    CU_ASSERT_EQUAL(0, chess_array_size(&array));

    chess_array_push(&array, &values[2]);
    chess_array_push(&array, &values[3]);
    chess_array_push(&array, &values[4]);
    CU_ASSERT_EQUAL(3, chess_array_size(&array));
    chess_array_clear(&array);
    CU_ASSERT_EQUAL(0, chess_array_size(&array));

    chess_array_cleanup(&array);
}

static void test_carray_set_elem()
{
    const char values[] = "Rabbit";
    char value;
    ChessArray array;
    
    chess_array_init(&array, sizeof(char));
    chess_array_push(&array, &values[0]);
    chess_array_push(&array, &values[1]);
    chess_array_push(&array, &values[2]);
    chess_array_push(&array, &values[3]);
    chess_array_push(&array, &values[4]);
    chess_array_push(&array, &values[5]);
    chess_array_push(&array, &values[6]);
    CU_ASSERT_STRING_EQUAL("Rabbit", (const char*)chess_array_data(&array));

    value = 'W';
    chess_array_set_elem(&array, 0, &value);
    CU_ASSERT_STRING_EQUAL("Wabbit", (const char*)chess_array_data(&array));

    value = 'd';
    chess_array_set_elem(&array, 5, &value);
    CU_ASSERT_STRING_EQUAL("Wabbid", (const char*)chess_array_data(&array));

    value = 'z';
    chess_array_set_elem(&array, 6, &value);
    CU_ASSERT_NSTRING_EQUAL("Wabbidz", (const char*)chess_array_data(&array), 7);
    
    value = '\0';
    chess_array_set_elem(&array, 3, &value);
    CU_ASSERT_STRING_EQUAL("Wab", (const char*)chess_array_data(&array));
}

void test_carray_add_tests()
{
    CU_Suite* suite = CU_add_suite("carray", NULL, NULL);
    CU_add_test(suite, "carray_init", (CU_TestFunc)test_carray_init);
    CU_add_test(suite, "carray_push", (CU_TestFunc)test_carray_push);
    CU_add_test(suite, "carray_pop", (CU_TestFunc)test_carray_pop);
    CU_add_test(suite, "carray_clear", (CU_TestFunc)test_carray_clear);
    CU_add_test(suite, "carray_set_elem", (CU_TestFunc)test_carray_set_elem);
}
