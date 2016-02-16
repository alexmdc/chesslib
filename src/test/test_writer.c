#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <CUnit/CUnit.h>

#include "../calloc.h"
#include "../writer.h"

#include "helpers.h"

static void test_buffer_writer_init(void)
{
    ChessBufferWriter writer;
    chess_buffer_writer_init(&writer);
    CU_ASSERT_EQUAL(0, chess_buffer_writer_size(&writer));
    chess_buffer_writer_cleanup(&writer);
}

static void test_buffer_writer_write(void)
{
    ChessBufferWriter writer;
    char* bigstring;

    chess_buffer_writer_init(&writer);

    chess_writer_write_char((ChessWriter*)&writer, 'A');
    CU_ASSERT_EQUAL(1, chess_buffer_writer_size(&writer));
    CU_ASSERT_NSTRING_EQUAL("A", chess_buffer_writer_data(&writer), 1);

    chess_writer_write_char((ChessWriter*)&writer, '!');
    CU_ASSERT_EQUAL(2, chess_buffer_writer_size(&writer));
    CU_ASSERT_NSTRING_EQUAL("A!", chess_buffer_writer_data(&writer), 2);

    chess_writer_write_string((ChessWriter*)&writer, "(cake)");
    CU_ASSERT_EQUAL(8, chess_buffer_writer_size(&writer));
    CU_ASSERT_NSTRING_EQUAL("A!(cake)", chess_buffer_writer_data(&writer), 8);

    chess_writer_write_string_size((ChessWriter*)&writer, "LIONESS", 4);
    CU_ASSERT_EQUAL(12, chess_buffer_writer_size(&writer));
    CU_ASSERT_NSTRING_EQUAL("A!(cake)LION", chess_buffer_writer_data(&writer), 12);

    bigstring = malloc(10000);
    memset(bigstring, '?', 10000);
    chess_writer_write_string_size((ChessWriter*)&writer, bigstring, 10000);
    free(bigstring);
    CU_ASSERT_EQUAL(10012, chess_buffer_writer_size(&writer));
    CU_ASSERT_NSTRING_EQUAL("????????????", chess_buffer_writer_data(&writer) + 10000, 12);

    chess_buffer_writer_cleanup(&writer);
}

static void test_buffer_writer_clear(void)
{
    ChessBufferWriter writer;

    chess_buffer_writer_init(&writer);
    chess_writer_write_string((ChessWriter*)&writer, "This too shall pass.");
    CU_ASSERT_EQUAL(20, chess_buffer_writer_size(&writer));

    chess_buffer_writer_clear(&writer);
    CU_ASSERT_EQUAL(0, chess_buffer_writer_size(&writer));

    chess_buffer_writer_cleanup(&writer);
}

static void test_buffer_writer_detach(void)
{
    ChessBufferWriter writer;
    char* buffer;

    chess_buffer_writer_init(&writer);
    chess_writer_write_string((ChessWriter*)&writer, "This too shall pass.");
    buffer = chess_buffer_writer_detach_buffer(&writer);
    chess_buffer_writer_cleanup(&writer);

    CU_ASSERT_NSTRING_EQUAL("This too shall pass.", buffer, 20);

    chess_free(buffer);
}

static void test_file_writer_write(void)
{
    ChessFileWriter writer;
    int fd[2], ret;
    FILE* file;
    char data[128];

    /* Create a pipe so we don't need to use the filesystem */
    ret = pipe(fd);
    assert(ret == 0);

    /* Open the pipe for writing */
    file = fdopen(fd[1], "w");
    assert(file != NULL);

    /* Write some test data to the pipe */
    chess_file_writer_init(&writer, file);
    chess_writer_write_char((ChessWriter*)&writer, 'A');
    chess_writer_write_char((ChessWriter*)&writer, '!');
    chess_writer_write_string((ChessWriter*)&writer, "(cake)");
    chess_writer_write_string_size((ChessWriter*)&writer, "LIONESS", 4);
    chess_file_writer_cleanup(&writer);
    fclose(file);

    /* Read from the other side and check the data */
    file = fdopen(fd[0], "r");
    assert(file != NULL);

    CU_ASSERT(fgets(data, 128, file) != NULL);
    CU_ASSERT_STRING_EQUAL("A!(cake)LION", data);

    fclose(file);
}

void test_writer_add_tests(void)
{
    CU_Suite* suite = add_suite("writer");
    CU_add_test(suite, "buffer_writer_init", (CU_TestFunc)test_buffer_writer_init);
    CU_add_test(suite, "buffer_writer_write", (CU_TestFunc)test_buffer_writer_write);
    CU_add_test(suite, "buffer_writer_clear", (CU_TestFunc)test_buffer_writer_clear);
    CU_add_test(suite, "buffer_writer_detach", (CU_TestFunc)test_buffer_writer_detach);
    CU_add_test(suite, "file_writer_write", (CU_TestFunc)test_file_writer_write);
}
