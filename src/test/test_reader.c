#include <assert.h>
#include <stdio.h>
#include <unistd.h>

#include <CUnit/CUnit.h>

#include "../reader.h"

#include "helpers.h"

static void test_buffer_reader(void)
{
    ChessBufferReader reader;

    chess_buffer_reader_init(&reader, "SOME(cool)");
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
    int fd[2], ret;
    FILE* file;

    /* Create a pipe so we don't need to use the filesystem */
    ret = pipe(fd);
    assert(ret == 0);

    /* Write some test data to the pipe */
    file = fdopen(fd[1], "w");
    assert(file != NULL);
    fputs("key=VALUE", file);
    fclose(file);

    /* Open the other side and read back the data */
    file = fdopen(fd[0], "r");
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
}

void test_reader_add_tests(void)
{
    CU_Suite* suite = add_suite("reader");
    CU_add_test(suite, "buffer_reader", (CU_TestFunc)test_buffer_reader);
    CU_add_test(suite, "file_reader", (CU_TestFunc)test_file_reader);
}
