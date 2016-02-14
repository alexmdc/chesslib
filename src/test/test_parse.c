#include <CUnit/CUnit.h>

#include "../parse.h"

#include "helpers.h"

static void test_parse_move(void)
{
    ChessPosition position;
    ChessMove move;

    /* Bug! Parsing this led to an incorrect "ambiguous move" return value */
    chess_position_init_fen(&position, "r4rk1/4bp2/p3pnp1/4B2p/qp1N1P2/7Q/1P4PP/3R1R1K w - - 0 25");
    CU_ASSERT_EQUAL(CHESS_PARSE_MOVE_OK, chess_parse_move("f5", &position, &move));
    CU_ASSERT_EQUAL(MV(F4,F5), move);

    /* Removing the pawn on f4 should actually lead to an ambiguous move */
    position.piece[CHESS_SQUARE_F4] = CHESS_PIECE_NONE;
    CU_ASSERT_EQUAL(CHESS_PARSE_MOVE_AMBIGUOUS, chess_parse_move("f5", &position, &move));

    /* Another bug - castling queenside is failing */
    chess_position_init_fen(&position, "r1b1k2r/1p1nbppp/pq1ppn2/6B1/4PP2/1NN2Q2/PPP3PP/R3KB1R w KQkq - 0 10");
    CU_ASSERT_EQUAL(CHESS_PARSE_MOVE_OK, chess_parse_move("O-O-O", &position, &move));
    CU_ASSERT_EQUAL(MV(E1,C1), move);

    /* Promotion is also failing when a different piece can move to the promotion square */
    chess_position_init_fen(&position, "6k1/8/2n5/p1Bp2N1/2pP4/1nP3P1/p5KP/5R2 b - - 0 44");
    CU_ASSERT_EQUAL(CHESS_PARSE_MOVE_OK, chess_parse_move("a1=Q", &position, &move));
    CU_ASSERT_EQUAL(MVP(A2,A1,QUEEN), move);

    /* Promotion to rook is failing */
    chess_position_init_fen(&position, "8/p6p/1p6/8/5Q2/P6P/1Pqp2P1/1k4K1 b - - 11 52");
    CU_ASSERT_EQUAL(CHESS_PARSE_MOVE_OK, chess_parse_move("d1=R+", &position, &move));
    CU_ASSERT_EQUAL(MVP(D2,D1,ROOK), move);
}

static void test_parse_null_move(void)
{
    ChessPosition position;
    ChessMove move;

    chess_position_init(&position);
    CU_ASSERT_EQUAL(CHESS_PARSE_MOVE_OK, chess_parse_move("--", &position, &move));
    CU_ASSERT_EQUAL(CHESS_MOVE_NULL, move);
    CU_ASSERT_EQUAL(CHESS_PARSE_MOVE_OK, chess_parse_move("--!?", &position, &move));
    CU_ASSERT_EQUAL(CHESS_MOVE_NULL, move);
    CU_ASSERT_EQUAL(CHESS_PARSE_MOVE_ERROR, chess_parse_move("---", &position, &move));
    CU_ASSERT_EQUAL(CHESS_PARSE_MOVE_ERROR, chess_parse_move("--=Q", &position, &move));
}

void test_parse_add_tests(void)
{
    CU_Suite* suite = add_suite("parse");
    CU_add_test(suite, "parse_move", (CU_TestFunc)test_parse_move);
    CU_add_test(suite, "parse_null_move", (CU_TestFunc)test_parse_null_move);
}
