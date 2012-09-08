#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include <CUnit/CUnit.h>

#include "../chess.h"
#include "../move.h"
#include "../unmove.h"
#include "../position.h"
#include "../calloc.h"

#include "helpers.h"

static int int_compare(const void* lp, const void* rp)
{
    int l = *((int*)lp);
    int r = *((int*)rp);
    return (l < r) ? -1 : (l == r) ? 0 : 1;
}

void assert_arrays_equal(const int* larray, int lsize, const int* rarray, int rsize, const char* file, unsigned int line)
{
    int i;
    if (lsize != rsize)
    {
        ASSERT_FAIL("ASSERT_ARRAYS_EQUAL(length)", file, line);
        return;
    }
    ASSERT_PASS("ASSERT_ARRAYS_EQUAL(length)", file, line);

    for (i = 0; i < lsize; i++)
    {
        if (larray[i] != rarray[i])
        {
            ASSERT_FAIL("ASSERT_ARRAYS_EQUAL(data)", file, line);
            return;
        }
    }
    ASSERT_PASS("ASSERT_ARRAYS_EQUAL(data)", file, line);
}

void assert_sets_equal(const int* lset, int lsize, const int* rset, int rsize, const char* file, unsigned int line)
{
    size_t bytes, i;
    int* lset_sorted, *rset_sorted, pass;

    if (lsize != rsize)
    {
        ASSERT_FAIL("ASSERT_SETS_EQUAL(length)", file, line);
        return;
    }
    ASSERT_PASS("ASSERT_SETS_EQUAL(length)", file, line);

    /* Clone and sort left array */
    bytes = lsize * sizeof(int);
    lset_sorted = malloc(bytes);
    memcpy(lset_sorted, lset, bytes);
    qsort(lset_sorted, lsize, sizeof(int), int_compare);

    /* Clone and sort right array */
    rset_sorted = malloc(bytes);
    memcpy(rset_sorted, rset, bytes);
    qsort(rset_sorted, rsize, sizeof(int), int_compare);

    pass = 1;
    for (i = 0; i < lsize; i++)
    {
        if (lset_sorted[i] != rset_sorted[i])
        {
            pass = 0;
            break;
        }
    }
    ASSERT_IMPL(pass, "ASSERT_SETS_EQUAL(data)", file, line);

    free(lset_sorted);
    free(rset_sorted);
}

void assert_positions_equal(const ChessPosition* lposition, const ChessPosition* rposition, const char* file, unsigned int line)
{
    ChessSquare sq;

    for (sq = CHESS_SQUARE_A1; sq <= CHESS_SQUARE_H8; sq++)
    {
        if (chess_position_piece(lposition, sq) != chess_position_piece(rposition, sq))
        {
            ASSERT_FAIL("ASSERT_POSITIONS_EQUAL(pieces)", file, line);
            return;
        }
    }

    if (chess_position_to_move(lposition) != chess_position_to_move(rposition))
    {
        ASSERT_FAIL("ASSERT_POSITIONS_EQUAL(to_move)", file, line);
        return;
    }

    if (chess_position_castle(lposition) != chess_position_castle(rposition))
    {
        ASSERT_FAIL("ASSERT_POSITIONS_EQUAL(castle)", file, line);
        return;
    }

    if (chess_position_ep(lposition) != chess_position_ep(rposition))
    {
        ASSERT_FAIL("ASSERT_POSITIONS_EQUAL(ep)", file, line);
        return;
    }

    if (chess_position_fifty(lposition) != chess_position_fifty(rposition))
    {
        ASSERT_FAIL("ASSERT_POSITIONS_EQUAL(fifty)", file, line);
        return;
    }

    if (chess_position_move_num(lposition) != chess_position_move_num(rposition))
    {
        ASSERT_FAIL("ASSERT_POSITIONS_EQUAL(move_num)", file, line);
        return;
    }

    ASSERT_PASS("ASSERT_POSITIONS_EQUAL()", file, line);
}

void assert_buffer_value(ChessBufferWriter* writer, const char* str, const char* file, unsigned int line)
{
    size_t size = strlen(str);
    ASSERT_IMPL(chess_buffer_writer_size(writer) == size, "ASSERT_BUFFER_VALUE(size)", file, line);
    ASSERT_IMPL(!strncmp(chess_buffer_writer_data(writer), str, size), "ASSERT_BUFFER_VALUE(value)", file, line);
}

static int alloc_count;

static int init_suite(void)
{
    alloc_count = chess_alloc_count();
    return 0;
}

static int cleanup_suite(void)
{
    return (alloc_count != chess_alloc_count());
}

CU_Suite* add_suite(const char* name)
{
    return CU_add_suite(name, init_suite, cleanup_suite);
}
