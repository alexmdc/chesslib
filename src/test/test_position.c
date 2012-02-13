#include <CUnit/CUnit.h>

#include "../chess.h"
#include "../move.h"
#include "../unmove.h"
#include "../position.h"
#include "../fen.h"

#include "helpers.h"

static void test_position_init()
{
    ChessPosition* position;
    
    position = chess_position_new();
    chess_position_destroy(position);

    position = chess_position_new();
    chess_position_init(position);

    CU_ASSERT_EQUAL(CHESS_PIECE_WHITE_ROOK, chess_position_piece(position, CHESS_SQUARE_A1));
    CU_ASSERT_EQUAL(CHESS_PIECE_WHITE_KING, chess_position_piece(position, CHESS_SQUARE_E1));
    CU_ASSERT_EQUAL(CHESS_PIECE_WHITE_PAWN, chess_position_piece(position, CHESS_SQUARE_C2));
    CU_ASSERT_EQUAL(CHESS_PIECE_NONE, chess_position_piece(position, CHESS_SQUARE_C3));
    CU_ASSERT_EQUAL(CHESS_PIECE_NONE, chess_position_piece(position, CHESS_SQUARE_G5));
    CU_ASSERT_EQUAL(CHESS_PIECE_BLACK_PAWN, chess_position_piece(position, CHESS_SQUARE_F7));
    CU_ASSERT_EQUAL(CHESS_PIECE_BLACK_KING, chess_position_piece(position, CHESS_SQUARE_E8));
    CU_ASSERT_EQUAL(CHESS_PIECE_BLACK_ROOK, chess_position_piece(position, CHESS_SQUARE_H8));

    CU_ASSERT_EQUAL(CHESS_COLOR_WHITE, chess_position_to_move(position));
    CU_ASSERT_EQUAL(CHESS_CASTLE_STATE_ALL, chess_position_castle(position));
    CU_ASSERT_EQUAL(CHESS_FILE_INVALID, chess_position_ep(position));
    CU_ASSERT_EQUAL(1, chess_position_move_num(position));

    chess_position_destroy(position);
}

static void test_position_make_move()
{
    ChessPosition* position = chess_position_new();
    chess_position_init(position);

    /* e4 */
    chess_position_make_move(position, MV(E2,E4));

    CU_ASSERT_EQUAL(CHESS_PIECE_NONE, chess_position_piece(position, CHESS_SQUARE_E2));
    CU_ASSERT_EQUAL(CHESS_PIECE_WHITE_PAWN, chess_position_piece(position, CHESS_SQUARE_E4));

    CU_ASSERT_EQUAL(CHESS_COLOR_BLACK, chess_position_to_move(position));
    CU_ASSERT_EQUAL(CHESS_CASTLE_STATE_ALL, chess_position_castle(position));
    CU_ASSERT_EQUAL(CHESS_FILE_E, chess_position_ep(position));
    CU_ASSERT_EQUAL(1, chess_position_move_num(position));

    /* Nf6 */
    chess_position_make_move(position, MV(G8,F6));

    CU_ASSERT_EQUAL(CHESS_PIECE_NONE, chess_position_piece(position, CHESS_SQUARE_G8));
    CU_ASSERT_EQUAL(CHESS_PIECE_BLACK_KNIGHT, chess_position_piece(position, CHESS_SQUARE_F6));

    CU_ASSERT_EQUAL(CHESS_COLOR_WHITE, chess_position_to_move(position));
    CU_ASSERT_EQUAL(CHESS_CASTLE_STATE_ALL, chess_position_castle(position));
    CU_ASSERT_EQUAL(CHESS_FILE_INVALID, chess_position_ep(position));
    CU_ASSERT_EQUAL(2, chess_position_move_num(position));

    /* Ke2 */
    chess_position_make_move(position, MV(E1,E2));

    CU_ASSERT_EQUAL(CHESS_PIECE_NONE, chess_position_piece(position, CHESS_SQUARE_E1));
    CU_ASSERT_EQUAL(CHESS_PIECE_WHITE_KING, chess_position_piece(position, CHESS_SQUARE_E2));

    CU_ASSERT_EQUAL(CHESS_COLOR_BLACK, chess_position_to_move(position));
    CU_ASSERT_EQUAL(CHESS_CASTLE_STATE_BKQ, chess_position_castle(position));
    CU_ASSERT_EQUAL(CHESS_FILE_INVALID, chess_position_ep(position));
    CU_ASSERT_EQUAL(2, chess_position_move_num(position));

    /* Rg8 */
    chess_position_make_move(position, MV(H8,G8));

    CU_ASSERT_EQUAL(CHESS_PIECE_NONE, chess_position_piece(position, CHESS_SQUARE_H8));
    CU_ASSERT_EQUAL(CHESS_PIECE_BLACK_ROOK, chess_position_piece(position, CHESS_SQUARE_G8));

    CU_ASSERT_EQUAL(CHESS_COLOR_WHITE, chess_position_to_move(position));
    CU_ASSERT_EQUAL(CHESS_CASTLE_STATE_BQ, chess_position_castle(position));
    CU_ASSERT_EQUAL(CHESS_FILE_INVALID, chess_position_ep(position));
    CU_ASSERT_EQUAL(3, chess_position_move_num(position));

    chess_position_destroy(position);
}

void test_position_check_result()
{
    ChessPosition* position = chess_position_new();
    chess_position_init(position);
    CU_ASSERT_EQUAL(CHESS_RESULT_NONE, chess_position_check_result(position));

    chess_fen_load("r1bk3r/p2pBpNp/n4n2/1p1NP2P/6P1/3P4/P1P1K3/q5b1 b - - 1 23", position);
    CU_ASSERT_EQUAL(CHESS_RESULT_WHITE_WINS, chess_position_check_result(position));

    chess_fen_load("8/8/8/5k2/5p2/7r/1R2K3/8 b - - 0 60", position);
    chess_position_make_move(position, MV(H3,H2));
    chess_position_make_move(position, MV(E2,F3));
    chess_position_make_move(position, MV(H2,B2));
    CU_ASSERT_EQUAL(CHESS_RESULT_DRAW, chess_position_check_result(position));

    chess_fen_load("6k1/6R1/8/1p2p2p/1P2Pn1P/5Pq1/4r3/7K b - - 0 50", position);
    chess_position_make_move(position, MV(G3,G7));
    CU_ASSERT_EQUAL(CHESS_RESULT_DRAW, chess_position_check_result(position));

    chess_fen_load("1Q6/5pk1/2p3p1/1p2N2p/1b5P/1bn5/2r3P1/2K5 w - - 16 42", position);
    CU_ASSERT_EQUAL(CHESS_RESULT_BLACK_WINS, chess_position_check_result(position));

    chess_fen_load("r1bq1r2/pp2n3/4N2k/3pPppP/1b1n2Q1/2N5/PP3PP1/R1B1K2R w KQ g6 0 15", position);
    chess_position_make_move(position, MV(H5,G6));
    CU_ASSERT_EQUAL(CHESS_RESULT_WHITE_WINS, chess_position_check_result(position));
}

void test_position_add_tests()
{
    CU_Suite* suite = CU_add_suite("position", NULL, NULL);
    CU_add_test(suite, "position_init", (CU_TestFunc)test_position_init);
    CU_add_test(suite, "position_make_move", (CU_TestFunc)test_position_make_move);
    CU_add_test(suite, "position_check_result", (CU_TestFunc)test_position_check_result);
}
