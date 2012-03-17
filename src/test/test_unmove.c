#include <CUnit/CUnit.h>

#include "../unmove.h"

static void test_unmove_make(void)
{
    ChessUnmove unmove;

    /* Basic move */
    unmove = chess_unmove_make(CHESS_SQUARE_E2, CHESS_SQUARE_E4, CHESS_UNMOVE_CAPTURED_NONE,
        CHESS_FALSE, CHESS_UNMOVE_EP_NONE, CHESS_CASTLE_STATE_ALL, 0);
    CU_ASSERT_EQUAL(CHESS_SQUARE_E2, chess_unmove_from(unmove));
    CU_ASSERT_EQUAL(CHESS_SQUARE_E4, chess_unmove_to(unmove));
    CU_ASSERT_EQUAL(CHESS_UNMOVE_CAPTURED_NONE, chess_unmove_captured(unmove));
    CU_ASSERT_EQUAL(CHESS_FALSE, chess_unmove_promotion(unmove));
    CU_ASSERT_EQUAL(CHESS_UNMOVE_EP_NONE, chess_unmove_ep(unmove));
    CU_ASSERT_EQUAL(CHESS_CASTLE_STATE_ALL, chess_unmove_castle(unmove));
    CU_ASSERT_EQUAL(0, chess_unmove_fifty(unmove));

    /* Capture */
    unmove = chess_unmove_make(CHESS_SQUARE_H5, CHESS_SQUARE_F7, CHESS_UNMOVE_CAPTURED_PAWN,
        CHESS_FALSE, CHESS_FALSE, CHESS_CASTLE_STATE_WKQ, 3);
    CU_ASSERT_EQUAL(CHESS_SQUARE_H5, chess_unmove_from(unmove));
    CU_ASSERT_EQUAL(CHESS_SQUARE_F7, chess_unmove_to(unmove));
    CU_ASSERT_EQUAL(CHESS_UNMOVE_CAPTURED_PAWN, chess_unmove_captured(unmove));
    CU_ASSERT_EQUAL(CHESS_FALSE, chess_unmove_promotion(unmove));
    CU_ASSERT_EQUAL(CHESS_FALSE, chess_unmove_ep(unmove));
    CU_ASSERT_EQUAL(CHESS_CASTLE_STATE_WKQ, chess_unmove_castle(unmove));
    CU_ASSERT_EQUAL(3, chess_unmove_fifty(unmove));

    /* Promotion, en passant available */
    unmove = chess_unmove_make(CHESS_SQUARE_G7, CHESS_SQUARE_H8, CHESS_UNMOVE_CAPTURED_ROOK,
        CHESS_TRUE, CHESS_UNMOVE_EP_AVAILABLE + CHESS_FILE_H, CHESS_CASTLE_STATE_BKQ, 7);
    CU_ASSERT_EQUAL(CHESS_SQUARE_G7, chess_unmove_from(unmove));
    CU_ASSERT_EQUAL(CHESS_SQUARE_H8, chess_unmove_to(unmove));
    CU_ASSERT_EQUAL(CHESS_UNMOVE_CAPTURED_ROOK, chess_unmove_captured(unmove));
    CU_ASSERT_EQUAL(CHESS_TRUE, chess_unmove_promotion(unmove));
    CU_ASSERT_EQUAL(CHESS_UNMOVE_EP_AVAILABLE + CHESS_FILE_H, chess_unmove_ep(unmove));
    CU_ASSERT_EQUAL(CHESS_CASTLE_STATE_BKQ, chess_unmove_castle(unmove));
    CU_ASSERT_EQUAL(7, chess_unmove_fifty(unmove));

    /* En passant */
    unmove = chess_unmove_make(CHESS_SQUARE_G4, CHESS_SQUARE_F3, CHESS_UNMOVE_CAPTURED_NONE,
        CHESS_FALSE, CHESS_UNMOVE_EP_CAPTURE, CHESS_CASTLE_STATE_NONE, 0);
    CU_ASSERT_EQUAL(CHESS_SQUARE_G4, chess_unmove_from(unmove));
    CU_ASSERT_EQUAL(CHESS_SQUARE_F3, chess_unmove_to(unmove));
    CU_ASSERT_EQUAL(CHESS_UNMOVE_CAPTURED_NONE, chess_unmove_captured(unmove));
    CU_ASSERT_EQUAL(CHESS_FALSE, chess_unmove_promotion(unmove));
    CU_ASSERT_EQUAL(CHESS_UNMOVE_EP_CAPTURE, chess_unmove_ep(unmove));
    CU_ASSERT_EQUAL(CHESS_CASTLE_STATE_NONE, chess_unmove_castle(unmove));
    CU_ASSERT_EQUAL(0, chess_unmove_fifty(unmove));

    /* Max value for fifty (8 bits = 0-255) */
    unmove = chess_unmove_make(CHESS_SQUARE_E4, CHESS_SQUARE_D5, CHESS_UNMOVE_CAPTURED_NONE,
        CHESS_FALSE, CHESS_FALSE, CHESS_CASTLE_STATE_NONE, 255);
    CU_ASSERT_EQUAL(CHESS_SQUARE_E4, chess_unmove_from(unmove));
    CU_ASSERT_EQUAL(CHESS_SQUARE_D5, chess_unmove_to(unmove));
    CU_ASSERT_EQUAL(CHESS_UNMOVE_CAPTURED_NONE, chess_unmove_captured(unmove));
    CU_ASSERT_EQUAL(CHESS_FALSE, chess_unmove_promotion(unmove));
    CU_ASSERT_EQUAL(CHESS_FALSE, chess_unmove_ep(unmove));
    CU_ASSERT_EQUAL(CHESS_CASTLE_STATE_NONE, chess_unmove_castle(unmove));
    CU_ASSERT_EQUAL(255, chess_unmove_fifty(unmove));
}

void test_unmove_add_tests(void)
{
    CU_Suite* suite = CU_add_suite("unmove", NULL, NULL);
    CU_add_test(suite, "unmove_make", (CU_TestFunc)test_unmove_make);
}
