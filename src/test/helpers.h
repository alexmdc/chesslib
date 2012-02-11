#ifndef CHESSLIB_TEST_HELPERS_H_
#define CHESSLIB_TEST_HELPERS_H_

#define ASSERT_IMPL(value, condition, file, line) CU_assertImplementation(value, line, condition, file, "", CU_FALSE)
#define ASSERT_FAIL(condition, file, line) CU_assertImplementation(CU_FALSE, line, condition, file, "", CU_FALSE)
#define ASSERT_PASS(condition, file, line) CU_assertImplementation(CU_TRUE, line, condition, file, "", CU_FALSE)

#define ASSERT_ARRAYS_EQUAL(larray, lsize, rarray, rsize) \
    assert_arrays_equal(larray, lsize, rarray, rsize, __FILE__, __LINE__)
#define ASSERT_SETS_EQUAL(lset, lsize, rset, rsize) \
    assert_sets_equal(lset, lsize, rset, rsize, __FILE__, __LINE__)

void assert_arrays_equal(const int* larray, int lsize, const int* rarray, int rsize, const char* file, unsigned int line);
void assert_sets_equal(const int* lset, int lsize, const int* rset, int rsize, const char* file, unsigned int line);

#endif /* CHESSLIB_TEST_HELPERS_H_ */
