#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include <CUnit/CUnit.h>

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
