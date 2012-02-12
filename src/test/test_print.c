#include <CUnit/CUnit.h>

#include "../chess.h"
#include "../move.h"
#include "../unmove.h"
#include "../position.h"
#include "../game.h"
#include "../fen.h"
#include "../print.h"

static void test_print_move()
{
    char buf[10];

    chess_print_move(chess_move_make(CHESS_SQUARE_E2, CHESS_SQUARE_E4), buf);
    CU_ASSERT_STRING_EQUAL("e2e4", buf);

    chess_print_move(chess_move_make(CHESS_SQUARE_G8, CHESS_SQUARE_F6), buf);
    CU_ASSERT_STRING_EQUAL("g8f6", buf);
}

static void test_print_move_san()
{
    char buf[10];
    ChessPosition* position = chess_position_new();
    ChessMove move;

    chess_fen_load("rnbqkb1r/ppp2ppp/8/3pP3/3Qn3/5N2/PPP2PPP/RNB1KB1R w KQkq d6 0 6", position);
    chess_print_move_san(chess_move_make(CHESS_SQUARE_E5, CHESS_SQUARE_D6), position, buf);
    CU_ASSERT_STRING_EQUAL("exd6", buf);

    chess_fen_load("r2qk2r/pbpnnpb1/1p1pp1pp/8/2PPP3/2N1BN2/PPQ1BPPP/R4RK1 w kq - 0 10", position);
    chess_print_move_san(chess_move_make(CHESS_SQUARE_A1, CHESS_SQUARE_D1), position, buf);
    CU_ASSERT_STRING_EQUAL("Rad1", buf);

    chess_fen_load("r2qk2r/1p1bbp1p/1n2p1p1/pPnpP3/3N1P2/P2BB3/2P1NQPP/R4RK1 b kq - 7 17", position);
    chess_print_move_san(chess_move_make(CHESS_SQUARE_B6, CHESS_SQUARE_A4), position, buf);
    CU_ASSERT_STRING_EQUAL("Nba4", buf);

    chess_fen_load("5rk1/3q2pp/p2bp3/1b1p2Q1/1p1Pp3/6PP/PP1B1rB1/1NR3RK b - - 3 23", position);
    chess_print_move_san(chess_move_make(CHESS_SQUARE_F8, CHESS_SQUARE_F5), position, buf);
    CU_ASSERT_STRING_EQUAL("R8f5", buf);

    chess_fen_load("r1bQ4/pp6/k1n4R/1qbp4/1Q6/4QK2/2q2P2/6N1 w - - 4 27", position);
    move = chess_move_make(CHESS_SQUARE_E3, CHESS_SQUARE_A3);
    chess_print_move_san(move, position, buf);
    CU_ASSERT_STRING_EQUAL("Qea3+", buf);
    chess_position_make_move(position, move);
    move = chess_move_make(CHESS_SQUARE_C2, CHESS_SQUARE_A4);
    chess_print_move_san(move, position, buf);
    CU_ASSERT_STRING_EQUAL("Qca4", buf);
    chess_position_make_move(position, move);
    move = chess_move_make(CHESS_SQUARE_A3, CHESS_SQUARE_A4);
    chess_print_move_san(move, position, buf);
    CU_ASSERT_STRING_EQUAL("Qaxa4+", buf);

    /* TODO test ambiguous ep capture e.g. cxd6 vs exd6 */

    chess_position_destroy(position);
}

static void test_print_game_moves()
{
    ChessGame* game;
    ChessPosition* position;
    char buf[1024];
    
    game = chess_game_new();
    chess_game_reset(game);
    chess_print_game_moves(game, buf);
    CU_ASSERT_STRING_EQUAL("*", buf);
    
    chess_game_make_move(game, chess_move_make(CHESS_SQUARE_G1, CHESS_SQUARE_F3));
    chess_game_make_move(game, chess_move_make(CHESS_SQUARE_D7, CHESS_SQUARE_D5));
    chess_game_make_move(game, chess_move_make(CHESS_SQUARE_C2, CHESS_SQUARE_C4));
    chess_print_game_moves(game, buf);
    CU_ASSERT_STRING_EQUAL("1. Nf3 d5 2. c4 *", buf);
    
    chess_game_set_result(game, CHESS_RESULT_WHITE_WINS);
    chess_print_game_moves(game, buf);
    CU_ASSERT_STRING_EQUAL("1. Nf3 d5 2. c4 1-0", buf);

    position = chess_position_new();
    chess_fen_load("5k2/3b2p1/1p4qp/p1pPp1pn/P1P1P3/2PQ4/6PP/3BB1K1 b - - 1 26", position);
    chess_game_reset_position(game, position);
    chess_game_make_move(game, chess_move_make(CHESS_SQUARE_H5, CHESS_SQUARE_F4));
    chess_game_make_move(game, chess_move_make(CHESS_SQUARE_D3, CHESS_SQUARE_C2));
    chess_game_make_move(game, chess_move_make(CHESS_SQUARE_D7, CHESS_SQUARE_A4));
    chess_game_set_result(game, CHESS_RESULT_BLACK_WINS);
    chess_print_game_moves(game, buf);
    CU_ASSERT_STRING_EQUAL(buf, "26... Nf4 27. Qc2 Bxa4 0-1");

    chess_position_destroy(position);
    chess_game_destroy(game);
}

static void test_print_result()
{
    char buf[10];

    chess_print_result(CHESS_RESULT_WHITE_WINS, buf);
    CU_ASSERT_STRING_EQUAL("1-0", buf);
    chess_print_result(CHESS_RESULT_BLACK_WINS, buf);
    CU_ASSERT_STRING_EQUAL("0-1", buf);
    chess_print_result(CHESS_RESULT_DRAW, buf);
    CU_ASSERT_STRING_EQUAL("1/2-1/2", buf);
    chess_print_result(CHESS_RESULT_IN_PROGRESS, buf);
    CU_ASSERT_STRING_EQUAL("*", buf);
}

void test_print_add_tests()
{
    CU_Suite* suite = CU_add_suite("print", NULL, NULL);
    CU_add_test(suite, "print_move", (CU_TestFunc)test_print_move);
    CU_add_test(suite, "print_move_san", (CU_TestFunc)test_print_move_san);
    CU_add_test(suite, "print_game_moves", (CU_TestFunc)test_print_game_moves);
    CU_add_test(suite, "print_result", (CU_TestFunc)test_print_result);
}
