#include <CUnit/CUnit.h>

#include "../chess.h"

static void test_boolean()
{
    CU_ASSERT_FALSE(CHESS_FALSE);
    CU_ASSERT_TRUE(CHESS_TRUE);
    CU_ASSERT_EQUAL(CHESS_TRUE, !CHESS_FALSE);
    CU_ASSERT_EQUAL(CHESS_FALSE, !CHESS_TRUE);
}

static void test_color()
{
    CU_ASSERT_EQUAL(CHESS_COLOR_BLACK, chess_color_other(CHESS_COLOR_WHITE));
    CU_ASSERT_EQUAL(CHESS_COLOR_WHITE, chess_color_other(CHESS_COLOR_BLACK));
}

static void test_piece_color()
{
    CU_ASSERT_EQUAL(CHESS_COLOR_WHITE, chess_piece_color(CHESS_PIECE_WHITE_PAWN));
    CU_ASSERT_EQUAL(CHESS_COLOR_BLACK, chess_piece_color(CHESS_PIECE_BLACK_PAWN));
    CU_ASSERT_EQUAL(CHESS_COLOR_WHITE, chess_piece_color(CHESS_PIECE_WHITE_KNIGHT));
    CU_ASSERT_EQUAL(CHESS_COLOR_BLACK, chess_piece_color(CHESS_PIECE_BLACK_KNIGHT));
    CU_ASSERT_EQUAL(CHESS_COLOR_WHITE, chess_piece_color(CHESS_PIECE_WHITE_BISHOP));
    CU_ASSERT_EQUAL(CHESS_COLOR_BLACK, chess_piece_color(CHESS_PIECE_BLACK_BISHOP));
    CU_ASSERT_EQUAL(CHESS_COLOR_WHITE, chess_piece_color(CHESS_PIECE_WHITE_ROOK));
    CU_ASSERT_EQUAL(CHESS_COLOR_BLACK, chess_piece_color(CHESS_PIECE_BLACK_ROOK));
    CU_ASSERT_EQUAL(CHESS_COLOR_WHITE, chess_piece_color(CHESS_PIECE_WHITE_QUEEN));
    CU_ASSERT_EQUAL(CHESS_COLOR_BLACK, chess_piece_color(CHESS_PIECE_BLACK_QUEEN));
    CU_ASSERT_EQUAL(CHESS_COLOR_WHITE, chess_piece_color(CHESS_PIECE_WHITE_KING));
    CU_ASSERT_EQUAL(CHESS_COLOR_BLACK, chess_piece_color(CHESS_PIECE_BLACK_KING));
}

static void test_piece_of_color()
{
    CU_ASSERT_EQUAL(CHESS_PIECE_WHITE_PAWN, chess_piece_of_color(CHESS_PIECE_WHITE_PAWN, CHESS_COLOR_WHITE));
    CU_ASSERT_EQUAL(CHESS_PIECE_BLACK_PAWN, chess_piece_of_color(CHESS_PIECE_WHITE_PAWN, CHESS_COLOR_BLACK));
    CU_ASSERT_EQUAL(CHESS_PIECE_WHITE_PAWN, chess_piece_of_color(CHESS_PIECE_BLACK_PAWN, CHESS_COLOR_WHITE));
    CU_ASSERT_EQUAL(CHESS_PIECE_BLACK_PAWN, chess_piece_of_color(CHESS_PIECE_BLACK_PAWN, CHESS_COLOR_BLACK));
    CU_ASSERT_EQUAL(CHESS_PIECE_WHITE_QUEEN, chess_piece_of_color(CHESS_PIECE_WHITE_QUEEN, CHESS_COLOR_WHITE));
    CU_ASSERT_EQUAL(CHESS_PIECE_BLACK_QUEEN, chess_piece_of_color(CHESS_PIECE_WHITE_QUEEN, CHESS_COLOR_BLACK));
    CU_ASSERT_EQUAL(CHESS_PIECE_WHITE_QUEEN, chess_piece_of_color(CHESS_PIECE_BLACK_QUEEN, CHESS_COLOR_WHITE));
    CU_ASSERT_EQUAL(CHESS_PIECE_BLACK_QUEEN, chess_piece_of_color(CHESS_PIECE_BLACK_QUEEN, CHESS_COLOR_BLACK));
}

static void test_piece_from_char()
{
    CU_ASSERT_EQUAL(CHESS_PIECE_NONE, chess_piece_from_char(' '));
    CU_ASSERT_EQUAL(CHESS_PIECE_NONE, chess_piece_from_char('?'));
    CU_ASSERT_EQUAL(CHESS_PIECE_WHITE_PAWN, chess_piece_from_char('P'));
    CU_ASSERT_EQUAL(CHESS_PIECE_BLACK_PAWN, chess_piece_from_char('p'));
    CU_ASSERT_EQUAL(CHESS_PIECE_WHITE_BISHOP, chess_piece_from_char('B'));
    CU_ASSERT_EQUAL(CHESS_PIECE_BLACK_ROOK, chess_piece_from_char('r'));
    CU_ASSERT_EQUAL(CHESS_PIECE_WHITE_QUEEN, chess_piece_from_char('Q'));
    CU_ASSERT_EQUAL(CHESS_PIECE_BLACK_KING, chess_piece_from_char('k'));
    CU_ASSERT_EQUAL(CHESS_PIECE_NONE, chess_piece_from_char(0));
}

void test_file_from_char()
{
    CU_ASSERT_EQUAL(CHESS_FILE_A, chess_file_from_char('a'));
    CU_ASSERT_EQUAL(CHESS_FILE_B, chess_file_from_char('b'));
    CU_ASSERT_EQUAL(CHESS_FILE_C, chess_file_from_char('c'));
    CU_ASSERT_EQUAL(CHESS_FILE_D, chess_file_from_char('d'));
    CU_ASSERT_EQUAL(CHESS_FILE_E, chess_file_from_char('e'));
    CU_ASSERT_EQUAL(CHESS_FILE_F, chess_file_from_char('f'));
    CU_ASSERT_EQUAL(CHESS_FILE_G, chess_file_from_char('g'));
    CU_ASSERT_EQUAL(CHESS_FILE_H, chess_file_from_char('h'));
    CU_ASSERT_EQUAL(CHESS_FILE_INVALID, chess_file_from_char('\0'));
    CU_ASSERT_EQUAL(CHESS_FILE_INVALID, chess_file_from_char('i'));
}

void test_rank_from_char()
{
    CU_ASSERT_EQUAL(CHESS_RANK_1, chess_rank_from_char('1'));
    CU_ASSERT_EQUAL(CHESS_RANK_2, chess_rank_from_char('2'));
    CU_ASSERT_EQUAL(CHESS_RANK_3, chess_rank_from_char('3'));
    CU_ASSERT_EQUAL(CHESS_RANK_4, chess_rank_from_char('4'));
    CU_ASSERT_EQUAL(CHESS_RANK_5, chess_rank_from_char('5'));
    CU_ASSERT_EQUAL(CHESS_RANK_6, chess_rank_from_char('6'));
    CU_ASSERT_EQUAL(CHESS_RANK_7, chess_rank_from_char('7'));
    CU_ASSERT_EQUAL(CHESS_RANK_8, chess_rank_from_char('8'));
    CU_ASSERT_EQUAL(CHESS_RANK_INVALID, chess_rank_from_char('\0'));
    CU_ASSERT_EQUAL(CHESS_RANK_INVALID, chess_rank_from_char('0'));
    CU_ASSERT_EQUAL(CHESS_RANK_INVALID, chess_rank_from_char('9'));
}

void test_file_to_char()
{
    CU_ASSERT_EQUAL('a', chess_file_to_char(CHESS_FILE_A));
    CU_ASSERT_EQUAL('b', chess_file_to_char(CHESS_FILE_B));
    CU_ASSERT_EQUAL('c', chess_file_to_char(CHESS_FILE_C));
    CU_ASSERT_EQUAL('d', chess_file_to_char(CHESS_FILE_D));
    CU_ASSERT_EQUAL('e', chess_file_to_char(CHESS_FILE_E));
    CU_ASSERT_EQUAL('f', chess_file_to_char(CHESS_FILE_F));
    CU_ASSERT_EQUAL('g', chess_file_to_char(CHESS_FILE_G));
    CU_ASSERT_EQUAL('h', chess_file_to_char(CHESS_FILE_H));
}

void test_rank_to_char()
{
    CU_ASSERT_EQUAL('1', chess_rank_to_char(CHESS_RANK_1));
    CU_ASSERT_EQUAL('2', chess_rank_to_char(CHESS_RANK_2));
    CU_ASSERT_EQUAL('3', chess_rank_to_char(CHESS_RANK_3));
    CU_ASSERT_EQUAL('4', chess_rank_to_char(CHESS_RANK_4));
    CU_ASSERT_EQUAL('5', chess_rank_to_char(CHESS_RANK_5));
    CU_ASSERT_EQUAL('6', chess_rank_to_char(CHESS_RANK_6));
    CU_ASSERT_EQUAL('7', chess_rank_to_char(CHESS_RANK_7));
    CU_ASSERT_EQUAL('8', chess_rank_to_char(CHESS_RANK_8));
}

void test_chess_add_tests()
{
    CU_Suite* suite = CU_add_suite("chess", NULL, NULL);
    CU_add_test(suite, "boolean", (CU_TestFunc)test_boolean);
    CU_add_test(suite, "color_other", (CU_TestFunc)test_color);
    CU_add_test(suite, "piece_color", (CU_TestFunc)test_piece_color);
    CU_add_test(suite, "piece_of_color", (CU_TestFunc)test_piece_of_color);
    CU_add_test(suite, "piece_from_char", (CU_TestFunc)test_piece_from_char);
    CU_add_test(suite, "file_from_char", (CU_TestFunc)test_file_from_char);
    CU_add_test(suite, "rank_from_char", (CU_TestFunc)test_rank_from_char);
    CU_add_test(suite, "file_to_char", (CU_TestFunc)test_file_to_char);
    CU_add_test(suite, "rank_to_char", (CU_TestFunc)test_rank_to_char);
}
