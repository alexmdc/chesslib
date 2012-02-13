#include <CUnit/CUnit.h>

#include "../chess.h"
#include "../move.h"
#include "../unmove.h"
#include "../position.h"
#include "../fen.h"

#include "helpers.h"

enum {
    WP = CHESS_PIECE_WHITE_PAWN,
    BP = CHESS_PIECE_BLACK_PAWN,
    WN = CHESS_PIECE_WHITE_KNIGHT,
    BN = CHESS_PIECE_BLACK_KNIGHT,
    WB = CHESS_PIECE_WHITE_BISHOP,
    BB = CHESS_PIECE_BLACK_BISHOP,
    WR = CHESS_PIECE_WHITE_ROOK,
    BR = CHESS_PIECE_BLACK_ROOK,
    WQ = CHESS_PIECE_WHITE_QUEEN,
    BQ = CHESS_PIECE_BLACK_QUEEN,
    WK = CHESS_PIECE_WHITE_KING,
    BK = CHESS_PIECE_BLACK_KING
};

static void test_position_pieces(const ChessPosition* position, const ChessPiece* pieces,
    const char* file, unsigned int line)
{
    ChessSquare sq, qs;
    ChessFile f;
    ChessRank r;

    for (r = CHESS_RANK_1; r <= CHESS_RANK_8; r++)
    {
        for (f = CHESS_FILE_A; f <= CHESS_FILE_H; f++)
        {
            sq = chess_square_from_fr(f, r);
            qs = chess_square_from_fr(f, CHESS_RANK_8 - r);
            if (chess_position_piece(position, sq) != pieces[qs])
                {
                ASSERT_IMPL(0, "TEST_POSITION_PIECES()", file, line);
                return;
            }
        }
    }
}

static void test_position_meta(const ChessPosition* position, ChessColor to_move,
    ChessCastleState castle, ChessFile ep, int fifty, int move_num,
    const char* file, unsigned int line)
{
    ASSERT_IMPL(chess_position_to_move(position) == to_move, "TEST_POSITION_META(to_move)", file, line);
    ASSERT_IMPL(chess_position_castle(position) == castle, "TEST_POSITION_META(castle)", file, line);
    ASSERT_IMPL(chess_position_ep(position) == ep, "TEST_POSITION_META(ep)", file, line);
    ASSERT_IMPL(chess_position_fifty(position) == fifty, "TEST_POSITION_META(fifty)", file, line);
    ASSERT_IMPL(chess_position_move_num(position) == move_num, "TEST_POSITION_META(move_num)", file, line);
}

#define TEST_POSITION_PIECES(position, pieces) \
    test_position_pieces(position, pieces, __FILE__, __LINE__)

#define TEST_POSITION_META(position, to_move, castle, ep, fifty, move_num) \
    test_position_meta(position, to_move, castle, ep, fifty, move_num, __FILE__, __LINE__)

static void test_fen_load()
{
    const char* fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    const ChessPiece pieces[] = {
        BR, BN, BB, BQ, BK, BB, BN, BR,
        BP, BP, BP, BP, BP, BP, BP, BP,
        0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,
        WP, WP, WP, WP, WP, WP, WP, WP,
        WR, WN, WB, WQ, WK, WB, WN, WR,
    };
    ChessPosition* position = chess_position_new();

    fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    chess_fen_load(fen, position);
    TEST_POSITION_PIECES(position, pieces);
    TEST_POSITION_META(position, CHESS_COLOR_WHITE, CHESS_CASTLE_STATE_ALL, -1, 0, 1);
    chess_position_destroy(position);
}

static void test_fen_load2()
{
    const char* fen = "8/2k5/8/8/8/8/5K2/8 b - - 10 50";
    const ChessPiece pieces[] = {
        0,  0,  0,  0,  0,  0,  0,  0,
        0,  0, BK,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0, WK,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,
    };
    ChessPosition* position = chess_position_new();

    chess_fen_load(fen, position);
    TEST_POSITION_PIECES(position, pieces);
    TEST_POSITION_META(position, CHESS_COLOR_BLACK, CHESS_CASTLE_STATE_NONE, -1, 10, 50);
    chess_position_destroy(position);
}

static void test_fen_save()
{
    char fen[100];

    ChessPosition* position = chess_position_new();
    chess_position_init(position);
    chess_fen_save(position, fen);
    CU_ASSERT_STRING_EQUAL("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", fen);
    chess_position_destroy(position);
}

static void test_fen_save2()
{
    char fen[100];

    ChessPosition* position = chess_position_new();
    chess_position_init(position);
    chess_fen_load("r3r1k1/pp3pbp/1qp3p1/2B5/2BP2b1/Q1n2N2/P4PPP/3R1K1R b - - 0 17", position);
    chess_position_make_move(position, MV(G4, E6));
    chess_fen_save(position, fen);
    CU_ASSERT_STRING_EQUAL("r3r1k1/pp3pbp/1qp1b1p1/2B5/2BP4/Q1n2N2/P4PPP/3R1K1R w - - 1 18", fen);
    chess_position_destroy(position);
}

static void test_fen_save3()
{
    char fen[100];

    ChessPosition* position = chess_position_new();
    chess_position_init(position);
    chess_fen_load("r1bq1r2/pp2npp1/4p1k1/3pP1N1/1b1n2QP/2N5/PP3PP1/R1B1K2R b KQ - 1 12", position);
    chess_position_make_move(position, MV(F7, F5));
    chess_fen_save(position, fen);
    CU_ASSERT_STRING_EQUAL("r1bq1r2/pp2n1p1/4p1k1/3pPpN1/1b1n2QP/2N5/PP3PP1/R1B1K2R w KQ f6 0 13", fen);
    chess_position_destroy(position);
}

void test_fen_add_tests()
{
    CU_Suite* suite = CU_add_suite("fen", NULL, NULL);
    CU_add_test(suite, "fen_load", (CU_TestFunc)test_fen_load);
    CU_add_test(suite, "fen_load2", (CU_TestFunc)test_fen_load2);
    CU_add_test(suite, "fen_save", (CU_TestFunc)test_fen_save);
    CU_add_test(suite, "fen_save2", (CU_TestFunc)test_fen_save2);
    CU_add_test(suite, "fen_save2", (CU_TestFunc)test_fen_save3);
}
