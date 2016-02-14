#include <CUnit/CUnit.h>

#include "../fen.h"
#include "../position.h"

#include "helpers.h"

static void test_position_validate(void)
{
    ChessPosition position;
    memset(&position, 0, sizeof(ChessPosition));
    position.to_move = CHESS_COLOR_WHITE;
    position.ep = CHESS_FILE_INVALID;
    position.move_num = 1;

    /* Empty position is invalid */
    CU_ASSERT_EQUAL(CHESS_FALSE, chess_position_validate(&position));

    /* Black is missing a king */
    position.piece[CHESS_SQUARE_E1] = CHESS_PIECE_WHITE_KING;
    CU_ASSERT_EQUAL(CHESS_FALSE, chess_position_validate(&position));

    /* One king each is valid! */
    position.piece[CHESS_SQUARE_E8] = CHESS_PIECE_BLACK_KING;
    CU_ASSERT_EQUAL(CHESS_TRUE, chess_position_validate(&position));

    /* Now white is missing a king */
    position.piece[CHESS_SQUARE_E1] = CHESS_PIECE_NONE;
    CU_ASSERT_EQUAL(CHESS_FALSE, chess_position_validate(&position));

    /* Two kings is no good either */
    position.piece[CHESS_SQUARE_E1] = CHESS_PIECE_WHITE_KING;
    position.piece[CHESS_SQUARE_D3] = CHESS_PIECE_WHITE_KING;
    CU_ASSERT_EQUAL(CHESS_FALSE, chess_position_validate(&position));

    /* Now black has two kings */
    position.piece[CHESS_SQUARE_D3] = CHESS_PIECE_BLACK_KING;
    CU_ASSERT_EQUAL(CHESS_FALSE, chess_position_validate(&position));

    /* White pawn on first rank */
    position.piece[CHESS_SQUARE_D3] = CHESS_PIECE_NONE;
    position.piece[CHESS_SQUARE_B1] = CHESS_PIECE_WHITE_PAWN;
    CU_ASSERT_EQUAL(CHESS_FALSE, chess_position_validate(&position));

    /* Black pawn on first rank */
    position.piece[CHESS_SQUARE_B1] = CHESS_PIECE_BLACK_PAWN;
    CU_ASSERT_EQUAL(CHESS_FALSE, chess_position_validate(&position));

    /* Black pawn on last rank */
    position.piece[CHESS_SQUARE_B1] = CHESS_PIECE_NONE;
    position.piece[CHESS_SQUARE_B8] = CHESS_PIECE_BLACK_PAWN;
    CU_ASSERT_EQUAL(CHESS_FALSE, chess_position_validate(&position));

    /* White pawn on last rank */
    position.piece[CHESS_SQUARE_B8] = CHESS_PIECE_WHITE_PAWN;
    CU_ASSERT_EQUAL(CHESS_FALSE, chess_position_validate(&position));

    /* Pawns on valid squares */
    position.piece[CHESS_SQUARE_B8] = CHESS_PIECE_NONE;
    position.piece[CHESS_SQUARE_B2] = CHESS_PIECE_WHITE_PAWN;
    position.piece[CHESS_SQUARE_B7] = CHESS_PIECE_BLACK_PAWN;
    CU_ASSERT_EQUAL(CHESS_TRUE, chess_position_validate(&position));
}

static void test_position_validate_check(void)
{
    ChessPosition position;
    memset(&position, 0, sizeof(ChessPosition));
    position.to_move = CHESS_COLOR_WHITE;
    position.ep = CHESS_FILE_INVALID;
    position.move_num = 1;

    /* Two kings cannot be adjacent */
    position.piece[CHESS_SQUARE_D4] = CHESS_PIECE_WHITE_KING;
    position.piece[CHESS_SQUARE_E5] = CHESS_PIECE_BLACK_KING;
    CU_ASSERT_EQUAL(CHESS_FALSE, chess_position_validate(&position));

    /* They can be separated though */
    position.piece[CHESS_SQUARE_E5] = CHESS_PIECE_NONE;
    position.piece[CHESS_SQUARE_E6] = CHESS_PIECE_BLACK_KING;
    CU_ASSERT_EQUAL(CHESS_TRUE, chess_position_validate(&position));

    /* With white to move, black's king cannot be en prise */
    position.piece[CHESS_SQUARE_E4] = CHESS_PIECE_WHITE_ROOK;
    CU_ASSERT_EQUAL(CHESS_FALSE, chess_position_validate(&position));

    /* But with black to move it's okay */
    position.to_move = CHESS_COLOR_BLACK;
    CU_ASSERT_EQUAL(CHESS_TRUE, chess_position_validate(&position));

    /* Likewise white's king cannot be en prise */
    position.piece[CHESS_SQUARE_D6] = CHESS_PIECE_BLACK_ROOK;
    CU_ASSERT_EQUAL(CHESS_FALSE, chess_position_validate(&position));

    /* So let's block the check */
    position.piece[CHESS_SQUARE_D5] = CHESS_PIECE_WHITE_PAWN;
    CU_ASSERT_EQUAL(CHESS_TRUE, chess_position_validate(&position));
}

static void test_position_validate_castle(void)
{
    ChessPosition position;
    memset(&position, 0, sizeof(ChessPosition));
    position.to_move = CHESS_COLOR_WHITE;
    position.ep = CHESS_FILE_INVALID;
    position.move_num = 1;

    /* Valid position but no castling availability */
    position.piece[CHESS_SQUARE_E1] = CHESS_PIECE_WHITE_KING;
    position.piece[CHESS_SQUARE_E8] = CHESS_PIECE_BLACK_KING;
    position.castle = CHESS_CASTLE_STATE_ALL;
    CU_ASSERT_EQUAL(CHESS_TRUE, chess_position_validate(&position));
    CU_ASSERT_EQUAL(CHESS_CASTLE_STATE_NONE, position.castle);

    /* White can castle king-side */
    position.piece[CHESS_SQUARE_H1] = CHESS_PIECE_WHITE_ROOK;
    position.castle = CHESS_CASTLE_STATE_ALL;
    CU_ASSERT_EQUAL(CHESS_TRUE, chess_position_validate(&position));
    CU_ASSERT_EQUAL(CHESS_CASTLE_STATE_WK, position.castle);

    /* Black can castle king-side */
    position.piece[CHESS_SQUARE_H8] = CHESS_PIECE_BLACK_ROOK;
    position.castle = CHESS_CASTLE_STATE_ALL;
    CU_ASSERT_EQUAL(CHESS_TRUE, chess_position_validate(&position));
    CU_ASSERT_EQUAL(CHESS_CASTLE_STATE_WK | CHESS_CASTLE_STATE_BK, position.castle);

    /* White can castle queen-side */
    position.piece[CHESS_SQUARE_A1] = CHESS_PIECE_WHITE_ROOK;
    position.castle = CHESS_CASTLE_STATE_ALL;
    CU_ASSERT_EQUAL(CHESS_TRUE, chess_position_validate(&position));
    CU_ASSERT_EQUAL(CHESS_CASTLE_STATE_WKQ | CHESS_CASTLE_STATE_BK, position.castle);

    /* Black can castle queen-side */
    position.piece[CHESS_SQUARE_A8] = CHESS_PIECE_BLACK_ROOK;
    position.castle = CHESS_CASTLE_STATE_ALL;
    CU_ASSERT_EQUAL(CHESS_TRUE, chess_position_validate(&position));
    CU_ASSERT_EQUAL(CHESS_CASTLE_STATE_ALL, position.castle);

    /* Moving the black king means black can no longer castle */
    position.piece[CHESS_SQUARE_E8] = CHESS_PIECE_NONE;
    position.piece[CHESS_SQUARE_D8] = CHESS_PIECE_BLACK_KING;
    position.castle = CHESS_CASTLE_STATE_ALL;
    CU_ASSERT_EQUAL(CHESS_TRUE, chess_position_validate(&position));
    CU_ASSERT_EQUAL(CHESS_CASTLE_STATE_WKQ, position.castle);

    /* Moving the white king has the same effect on white */
    position.piece[CHESS_SQUARE_E1] = CHESS_PIECE_NONE;
    position.piece[CHESS_SQUARE_D1] = CHESS_PIECE_WHITE_KING;
    position.castle = CHESS_CASTLE_STATE_ALL;
    CU_ASSERT_EQUAL(CHESS_TRUE, chess_position_validate(&position));
    CU_ASSERT_EQUAL(CHESS_CASTLE_STATE_NONE, position.castle);
}

static void test_position_validate_ep(void)
{
    ChessPosition position;
    memset(&position, 0, sizeof(ChessPosition));
    position.to_move = CHESS_COLOR_WHITE;
    position.ep = CHESS_FILE_INVALID;
    position.move_num = 1;

    /* Valid position but no en-passant availability */
    position.piece[CHESS_SQUARE_E1] = CHESS_PIECE_WHITE_KING;
    position.piece[CHESS_SQUARE_E8] = CHESS_PIECE_BLACK_KING;
    position.ep = CHESS_FILE_E;
    CU_ASSERT_EQUAL(CHESS_TRUE, chess_position_validate(&position));
    CU_ASSERT_EQUAL(CHESS_FILE_INVALID, position.ep);

    /* Black's pawn on e5 could have moved from e7 */
    position.piece[CHESS_SQUARE_E5] = CHESS_PIECE_BLACK_PAWN;
    position.ep = CHESS_FILE_E;
    CU_ASSERT_EQUAL(CHESS_TRUE, chess_position_validate(&position));
    CU_ASSERT_EQUAL(CHESS_FILE_E, position.ep);

    /* But if black is to move that is no longer valid */
    position.to_move = CHESS_COLOR_BLACK;
    CU_ASSERT_EQUAL(CHESS_TRUE, chess_position_validate(&position));
    CU_ASSERT_EQUAL(CHESS_FILE_INVALID, position.ep);

    /* Unless white's pawn just moved to e4 */
    position.piece[CHESS_SQUARE_E4] = CHESS_PIECE_WHITE_PAWN;
    position.ep = CHESS_FILE_E;
    CU_ASSERT_EQUAL(CHESS_TRUE, chess_position_validate(&position));
    CU_ASSERT_EQUAL(CHESS_FILE_E, position.ep);

    /* If ep is set to a different file then it should be cleared */
    position.ep = CHESS_FILE_D;
    CU_ASSERT_EQUAL(CHESS_TRUE, chess_position_validate(&position));
    CU_ASSERT_EQUAL(CHESS_FILE_INVALID, position.ep);
}

static void test_position_make_move(void)
{
    ChessPosition position;
    chess_fen_load(CHESS_FEN_STARTING_POSITION, &position);

    /* e4 */
    chess_position_make_move(&position, MV(E2,E4));

    CU_ASSERT_EQUAL(CHESS_PIECE_NONE, position.piece[CHESS_SQUARE_E2]);
    CU_ASSERT_EQUAL(CHESS_PIECE_WHITE_PAWN, position.piece[CHESS_SQUARE_E4]);

    CU_ASSERT_EQUAL(CHESS_COLOR_BLACK, position.to_move);
    CU_ASSERT_EQUAL(CHESS_CASTLE_STATE_ALL, position.castle);
    CU_ASSERT_EQUAL(CHESS_FILE_E, position.ep);
    CU_ASSERT_EQUAL(1, position.move_num);

    /* Nf6 */
    chess_position_make_move(&position, MV(G8,F6));

    CU_ASSERT_EQUAL(CHESS_PIECE_NONE, position.piece[CHESS_SQUARE_G8]);
    CU_ASSERT_EQUAL(CHESS_PIECE_BLACK_KNIGHT, position.piece[CHESS_SQUARE_F6]);

    CU_ASSERT_EQUAL(CHESS_COLOR_WHITE, position.to_move);
    CU_ASSERT_EQUAL(CHESS_CASTLE_STATE_ALL, position.castle);
    CU_ASSERT_EQUAL(CHESS_FILE_INVALID, position.ep);
    CU_ASSERT_EQUAL(2, position.move_num);

    /* Ke2 */
    chess_position_make_move(&position, MV(E1,E2));

    CU_ASSERT_EQUAL(CHESS_PIECE_NONE, position.piece[CHESS_SQUARE_E1]);
    CU_ASSERT_EQUAL(CHESS_PIECE_WHITE_KING, position.piece[CHESS_SQUARE_E2]);

    CU_ASSERT_EQUAL(CHESS_COLOR_BLACK, position.to_move);
    CU_ASSERT_EQUAL(CHESS_CASTLE_STATE_BKQ, position.castle);
    CU_ASSERT_EQUAL(CHESS_FILE_INVALID, position.ep);
    CU_ASSERT_EQUAL(2, position.move_num);

    /* Rg8 */
    chess_position_make_move(&position, MV(H8,G8));

    CU_ASSERT_EQUAL(CHESS_PIECE_NONE, position.piece[CHESS_SQUARE_H8]);
    CU_ASSERT_EQUAL(CHESS_PIECE_BLACK_ROOK, position.piece[CHESS_SQUARE_G8]);

    CU_ASSERT_EQUAL(CHESS_COLOR_WHITE, position.to_move);
    CU_ASSERT_EQUAL(CHESS_CASTLE_STATE_BQ, position.castle);
    CU_ASSERT_EQUAL(CHESS_FILE_INVALID, position.ep);
    CU_ASSERT_EQUAL(3, position.move_num);
}

static void test_position_make_null_move(void)
{
    ChessSquare sq;
    ChessBoolean squares_equal;
    ChessPosition position;
    ChessPosition positions[2];
    ChessUnmove unmoves[4];

    chess_fen_load(CHESS_FEN_STARTING_POSITION, &position);

    /* Make null move for white */
    chess_position_copy(&position, &positions[0]);
    unmoves[0] = chess_position_make_move(&position, CHESS_MOVE_NULL);
    squares_equal = CHESS_TRUE;
    for (sq = CHESS_SQUARE_A1; sq <= CHESS_SQUARE_H8; sq++)
    {
        if (position.piece[sq] != positions[0].piece[sq])
        {
            squares_equal = CHESS_FALSE;
            break;
        }
    }
    CU_ASSERT(squares_equal);
    CU_ASSERT_EQUAL(CHESS_COLOR_BLACK, position.to_move);
    CU_ASSERT_EQUAL(CHESS_CASTLE_STATE_ALL, position.castle);
    CU_ASSERT_EQUAL(CHESS_FILE_INVALID, position.ep);
    CU_ASSERT_EQUAL(1, position.move_num);
    CU_ASSERT_EQUAL(1, position.fifty);

    /* Make normal moves */
    unmoves[1] = chess_position_make_move(&position, MV(B8,C6));
    unmoves[2] = chess_position_make_move(&position, MV(F2,F4));

    /* Make null move for black */
    chess_position_copy(&position, &positions[1]);
    unmoves[3] = chess_position_make_move(&position, CHESS_MOVE_NULL);
    squares_equal = CHESS_TRUE;
    for (sq = CHESS_SQUARE_A1; sq <= CHESS_SQUARE_H8; sq++)
    {
        if (position.piece[sq] != positions[1].piece[sq])
        {
            squares_equal = CHESS_FALSE;
            break;
        }
    }
    CU_ASSERT(squares_equal);
    CU_ASSERT_EQUAL(CHESS_COLOR_WHITE, position.to_move);
    CU_ASSERT_EQUAL(CHESS_CASTLE_STATE_ALL, position.castle);
    CU_ASSERT_EQUAL(CHESS_FILE_INVALID, position.ep);
    CU_ASSERT_EQUAL(3, position.move_num);
    CU_ASSERT_EQUAL(1, position.fifty);

    /* Check that undoing the moves works too */
    chess_position_undo_move(&position, unmoves[3]);
    ASSERT_POSITIONS_EQUAL(&position, &positions[1]);
    chess_position_undo_move(&position, unmoves[2]);
    chess_position_undo_move(&position, unmoves[1]);
    chess_position_undo_move(&position, unmoves[0]);
    ASSERT_POSITIONS_EQUAL(&position, &positions[0]);
}

void test_position_check_result(void)
{
    ChessPosition position;

    chess_fen_load(CHESS_FEN_STARTING_POSITION, &position);
    CU_ASSERT_EQUAL(CHESS_RESULT_NONE, chess_position_check_result(&position));

    chess_fen_load("r1bk3r/p2pBpNp/n4n2/1p1NP2P/6P1/3P4/P1P1K3/q5b1 b - - 1 23", &position);
    CU_ASSERT_EQUAL(CHESS_RESULT_WHITE_WINS, chess_position_check_result(&position));

    chess_fen_load("8/8/8/5k2/5p2/7r/1R2K3/8 b - - 0 60", &position);
    chess_position_make_move(&position, MV(H3,H2));
    chess_position_make_move(&position, MV(E2,F3));
    chess_position_make_move(&position, MV(H2,B2));
    CU_ASSERT_EQUAL(CHESS_RESULT_DRAW, chess_position_check_result(&position));

    chess_fen_load("6k1/6R1/8/1p2p2p/1P2Pn1P/5Pq1/4r3/7K b - - 0 50", &position);
    chess_position_make_move(&position, MV(G3,G7));
    CU_ASSERT_EQUAL(CHESS_RESULT_DRAW, chess_position_check_result(&position));

    chess_fen_load("1Q6/5pk1/2p3p1/1p2N2p/1b5P/1bn5/2r3P1/2K5 w - - 16 42", &position);
    CU_ASSERT_EQUAL(CHESS_RESULT_BLACK_WINS, chess_position_check_result(&position));

    chess_fen_load("r1bq1r2/pp2n3/4N2k/3pPppP/1b1n2Q1/2N5/PP3PP1/R1B1K2R w KQ g6 0 15", &position);
    chess_position_make_move(&position, MV(H5,G6));
    CU_ASSERT_EQUAL(CHESS_RESULT_WHITE_WINS, chess_position_check_result(&position));
}

void test_position_add_tests(void)
{
    CU_Suite* suite = add_suite("position");
    CU_add_test(suite, "position_validate", (CU_TestFunc)test_position_validate);
    CU_add_test(suite, "position_validate_check", (CU_TestFunc)test_position_validate_check);
    CU_add_test(suite, "position_validate_castle", (CU_TestFunc)test_position_validate_castle);
    CU_add_test(suite, "position_validate_ep", (CU_TestFunc)test_position_validate_ep);
    CU_add_test(suite, "position_make_null_move", (CU_TestFunc)test_position_make_null_move);
    CU_add_test(suite, "position_make_move", (CU_TestFunc)test_position_make_move);
    CU_add_test(suite, "position_check_result", (CU_TestFunc)test_position_check_result);
}
