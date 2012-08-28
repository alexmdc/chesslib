#include <CUnit/CUnit.h>
#include <stdlib.h>
#include <assert.h>

#include "../reader.h"

#include "helpers.h"

static void test_buffer_reader(void)
{
    ChessBufferReader reader;
    const char* str = "SOME(cool)";

    chess_buffer_reader_init(&reader, str, strlen(str));
    CU_ASSERT_EQUAL(chess_reader_getc((ChessReader*)&reader), 'S');
    CU_ASSERT_EQUAL(chess_reader_getc((ChessReader*)&reader), 'O');
    CU_ASSERT_EQUAL(chess_reader_getc((ChessReader*)&reader), 'M');
    CU_ASSERT_EQUAL(chess_reader_getc((ChessReader*)&reader), 'E');

    chess_reader_ungetc((ChessReader*)&reader, '?');
    CU_ASSERT_EQUAL(chess_reader_getc((ChessReader*)&reader), '?');
    CU_ASSERT_EQUAL(chess_reader_getc((ChessReader*)&reader), '(');

    chess_reader_ungetc((ChessReader*)&reader, 'X');
    CU_ASSERT_EQUAL(chess_reader_getc((ChessReader*)&reader), 'X');
    CU_ASSERT_EQUAL(chess_reader_getc((ChessReader*)&reader), 'c');
    CU_ASSERT_EQUAL(chess_reader_getc((ChessReader*)&reader), 'o');
    CU_ASSERT_EQUAL(chess_reader_getc((ChessReader*)&reader), 'o');
    CU_ASSERT_EQUAL(chess_reader_getc((ChessReader*)&reader), 'l');
    CU_ASSERT_EQUAL(chess_reader_getc((ChessReader*)&reader), ')');

    CU_ASSERT_EQUAL(chess_reader_getc((ChessReader*)&reader), EOF);
    CU_ASSERT_EQUAL(chess_reader_getc((ChessReader*)&reader), EOF);

    chess_buffer_reader_cleanup(&reader);
}

static void test_file_reader(void)
{
    ChessFileReader reader;
    char* filename;
    FILE* file;

    filename = tmpnam(NULL);
    file = fopen(filename, "w");
    assert(file != NULL);

    /* First write some data to the file */
    fputs("key=VALUE", file);
    fclose(file);

    /* Reopen the file and try to read it */
    file = fopen(filename, "r");
    assert(file != NULL);

    chess_file_reader_init(&reader, file);
    CU_ASSERT_EQUAL(chess_reader_getc((ChessReader*)&reader), 'k');
    CU_ASSERT_EQUAL(chess_reader_getc((ChessReader*)&reader), 'e');
    CU_ASSERT_EQUAL(chess_reader_getc((ChessReader*)&reader), 'y');
    CU_ASSERT_EQUAL(chess_reader_getc((ChessReader*)&reader), '=');

    chess_reader_ungetc((ChessReader*)&reader, 'Q');
    CU_ASSERT_EQUAL(chess_reader_getc((ChessReader*)&reader), 'Q');
    CU_ASSERT_EQUAL(chess_reader_getc((ChessReader*)&reader), 'V');
    CU_ASSERT_EQUAL(chess_reader_getc((ChessReader*)&reader), 'A');

    chess_reader_ungetc((ChessReader*)&reader, '&');
    CU_ASSERT_EQUAL(chess_reader_getc((ChessReader*)&reader), '&');
    CU_ASSERT_EQUAL(chess_reader_getc((ChessReader*)&reader), 'L');
    CU_ASSERT_EQUAL(chess_reader_getc((ChessReader*)&reader), 'U');
    CU_ASSERT_EQUAL(chess_reader_getc((ChessReader*)&reader), 'E');

    CU_ASSERT_EQUAL(chess_reader_getc((ChessReader*)&reader), EOF);
    CU_ASSERT_EQUAL(chess_reader_getc((ChessReader*)&reader), EOF);

    chess_file_reader_cleanup(&reader);

    fclose(file);
    remove(filename);
}

void test_reader_add_tests(void)
{
    CU_Suite* suite = CU_add_suite("reader", NULL, NULL);
    CU_add_test(suite, "buffer_reader", (CU_TestFunc)test_buffer_reader);
    CU_add_test(suite, "file_reader", (CU_TestFunc)test_file_reader);
}
