#include <CUnit/CUnit.h>

#include "../move.h"

static void test_move_promote_to_char(void)
{
    CU_ASSERT_EQUAL('n', chess_move_promote_to_char(CHESS_MOVE_PROMOTE_KNIGHT));
    CU_ASSERT_EQUAL('b', chess_move_promote_to_char(CHESS_MOVE_PROMOTE_BISHOP));
    CU_ASSERT_EQUAL('r', chess_move_promote_to_char(CHESS_MOVE_PROMOTE_ROOK));
    CU_ASSERT_EQUAL('q', chess_move_promote_to_char(CHESS_MOVE_PROMOTE_QUEEN));
}

static void test_move_promote_from_char(void)
{
    CU_ASSERT_EQUAL(CHESS_MOVE_PROMOTE_KNIGHT, chess_move_promote_from_char('n'));
    CU_ASSERT_EQUAL(CHESS_MOVE_PROMOTE_BISHOP, chess_move_promote_from_char('b'));
    CU_ASSERT_EQUAL(CHESS_MOVE_PROMOTE_ROOK, chess_move_promote_from_char('r'));
    CU_ASSERT_EQUAL(CHESS_MOVE_PROMOTE_QUEEN, chess_move_promote_from_char('q'));
    CU_ASSERT_EQUAL(CHESS_MOVE_PROMOTE_NONE, chess_move_promote_from_char('\0'));
    CU_ASSERT_EQUAL(CHESS_MOVE_PROMOTE_NONE, chess_move_promote_from_char('k'));
}

static void test_move_make(void)
{
    ChessMove move;

    move = chess_move_make(CHESS_SQUARE_H5, CHESS_SQUARE_F7);
    CU_ASSERT_EQUAL(CHESS_SQUARE_H5, chess_move_from(move));
    CU_ASSERT_EQUAL(CHESS_SQUARE_F7, chess_move_to(move));
    CU_ASSERT_EQUAL(CHESS_MOVE_PROMOTE_NONE, chess_move_promotes(move));

    move = chess_move_make_promote(CHESS_SQUARE_A7, CHESS_SQUARE_A8, CHESS_MOVE_PROMOTE_QUEEN);
    CU_ASSERT_EQUAL(CHESS_SQUARE_A7, chess_move_from(move));
    CU_ASSERT_EQUAL(CHESS_SQUARE_A8, chess_move_to(move));
    CU_ASSERT_EQUAL(CHESS_MOVE_PROMOTE_QUEEN, chess_move_promotes(move));

    move = chess_move_make_promote(CHESS_SQUARE_C2, CHESS_SQUARE_C1, CHESS_MOVE_PROMOTE_KNIGHT);
    CU_ASSERT_EQUAL(CHESS_SQUARE_C2, chess_move_from(move));
    CU_ASSERT_EQUAL(CHESS_SQUARE_C1, chess_move_to(move));
    CU_ASSERT_EQUAL(CHESS_MOVE_PROMOTE_KNIGHT, chess_move_promotes(move));
}

void test_move_add_tests(void)
{
    CU_Suite* suite = CU_add_suite("move", NULL, NULL);
    CU_add_test(suite, "move_promote_to_char", (CU_TestFunc)test_move_promote_to_char);
    CU_add_test(suite, "move_promote_from_char", (CU_TestFunc)test_move_promote_from_char);
    CU_add_test(suite, "move_make", (CU_TestFunc)test_move_make);
}
