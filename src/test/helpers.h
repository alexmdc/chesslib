#ifndef CHESSLIB_TEST_HELPERS_H_
#define CHESSLIB_TEST_HELPERS_H_

#include "../position.h"

#define ASSERT_IMPL(value, condition, file, line) CU_assertImplementation(value, line, condition, file, "", CU_FALSE)
#define ASSERT_FAIL(condition, file, line) CU_assertImplementation(CU_FALSE, line, condition, file, "", CU_FALSE)
#define ASSERT_PASS(condition, file, line) CU_assertImplementation(CU_TRUE, line, condition, file, "", CU_FALSE)

#define ASSERT_ARRAYS_EQUAL(larray, lsize, rarray, rsize) \
    assert_arrays_equal(larray, lsize, rarray, rsize, __FILE__, __LINE__)
#define ASSERT_SETS_EQUAL(lset, lsize, rset, rsize) \
    assert_sets_equal(lset, lsize, rset, rsize, __FILE__, __LINE__)
#define ASSERT_POSITIONS_EQUAL(lposition, rposition) \
    assert_positions_equal(lposition, rposition, __FILE__, __LINE__)

void assert_arrays_equal(const int* larray, int lsize, const int* rarray, int rsize, const char* file, unsigned int line);
void assert_sets_equal(const int* lset, int lsize, const int* rset, int rsize, const char* file, unsigned int line);
void assert_positions_equal(const ChessPosition*, const ChessPosition*, const char* file, unsigned int line);

#define MV(f,t) chess_move_make(CHESS_SQUARE_ ## f, CHESS_SQUARE_ ## t)
#define MVP(f,t,p) chess_move_make_promote(CHESS_SQUARE_ ## f, CHESS_SQUARE_ ## t, CHESS_MOVE_PROMOTE_ ## p)

#endif /* CHESSLIB_TEST_HELPERS_H_ */
