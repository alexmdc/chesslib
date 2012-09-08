#include <CUnit/CUnit.h>

#include "../print.h"

#include "helpers.h"

static void test_print_move(void)
{
    char buf[10];

    chess_print_move(MV(E2,E4), buf);
    CU_ASSERT_STRING_EQUAL("e2e4", buf);

    chess_print_move(MV(G8,F6), buf);
    CU_ASSERT_STRING_EQUAL("g8f6", buf);
}

static void test_print_move_san(void)
{
    char buf[10];
    ChessPosition position;
    ChessMove move;

    chess_position_init(&position);
    chess_print_move_san(CHESS_MOVE_NULL, &position, buf);
    CU_ASSERT_STRING_EQUAL("--", buf);

    chess_position_init_fen(&position, "rnbqkb1r/ppp2ppp/8/3pP3/3Qn3/5N2/PPP2PPP/RNB1KB1R w KQkq d6 0 6");
    chess_print_move_san(MV(E5,D6), &position, buf);
    CU_ASSERT_STRING_EQUAL("exd6", buf);

    chess_position_init_fen(&position, "r2qk2r/pbpnnpb1/1p1pp1pp/8/2PPP3/2N1BN2/PPQ1BPPP/R4RK1 w kq - 0 10");
    chess_print_move_san(MV(A1,D1), &position, buf);
    CU_ASSERT_STRING_EQUAL("Rad1", buf);

    chess_position_init_fen(&position, "r2qk2r/1p1bbp1p/1n2p1p1/pPnpP3/3N1P2/P2BB3/2P1NQPP/R4RK1 b kq - 7 17");
    chess_print_move_san(MV(B6,A4), &position, buf);
    CU_ASSERT_STRING_EQUAL("Nba4", buf);

    chess_position_init_fen(&position, "5rk1/3q2pp/p2bp3/1b1p2Q1/1p1Pp3/6PP/PP1B1rB1/1NR3RK b - - 3 23");
    chess_print_move_san(MV(F8,F5), &position, buf);
    CU_ASSERT_STRING_EQUAL("R8f5", buf);

    chess_position_init_fen(&position, "rnb1k1r1/pp2bpPP/4p3/3p4/6Q1/8/1pP1KP2/1q3BNR w q - 0 17");
    chess_print_move_san(chess_move_make_promote(CHESS_SQUARE_H7, CHESS_SQUARE_G8, CHESS_MOVE_PROMOTE_QUEEN), &position, buf);
    CU_ASSERT_STRING_EQUAL("hxg8=Q+", buf);

    chess_position_init_fen(&position, "6K1/7P/6k1/5p2/5P2/8/8/8 w - - 0 63");
    chess_print_move_san(MVP(H7,H8,ROOK), &position, buf);
    CU_ASSERT_STRING_EQUAL("h8=R", buf);

    chess_position_init_fen(&position, "r1bQ4/pp6/k1n4R/1qbp4/1Q6/4QK2/2q2P2/6N1 w - - 4 27");
    move = MV(E3,A3);
    chess_print_move_san(move, &position, buf);
    CU_ASSERT_STRING_EQUAL("Qea3+", buf);
    chess_position_make_move(&position, move);
    move = MV(C2,A4);
    chess_print_move_san(move, &position, buf);
    CU_ASSERT_STRING_EQUAL("Qca4", buf);
    chess_position_make_move(&position, move);
    move = MV(A3,A4);
    chess_print_move_san(move, &position, buf);
    CU_ASSERT_STRING_EQUAL("Qaxa4+", buf);

    /* TODO test ambiguous ep capture e.g. cxd6 vs exd6 */
}

static void test_print_game_moves(void)
{
    ChessGame* game;
    ChessGameIterator* iter;
    ChessBufferWriter writer;

    chess_buffer_writer_init(&writer);
    game = chess_game_new();
    chess_print_game_moves(game, (ChessWriter*)&writer);
    ASSERT_BUFFER_VALUE(&writer, "*");

    chess_buffer_writer_clear(&writer);
    iter = chess_game_get_iterator(game);
    chess_game_iterator_append_move(iter, MV(G1,F3));
    chess_game_iterator_append_move(iter, MV(D7,D5));
    chess_game_iterator_append_move(iter, MV(C2,C4));
    chess_print_game_moves(game, (ChessWriter*)&writer);
    ASSERT_BUFFER_VALUE(&writer, "1. Nf3 d5 2. c4 *");

    chess_buffer_writer_clear(&writer);
    chess_game_set_result(game, CHESS_RESULT_WHITE_WINS);
    chess_print_game_moves(game, (ChessWriter*)&writer);
    ASSERT_BUFFER_VALUE(&writer, "1. Nf3 d5 2. c4 1-0");
    chess_game_iterator_destroy(iter);

    chess_buffer_writer_clear(&writer);
    chess_game_reset_fen(game, "5k2/3b2p1/1p4qp/p1pPp1pn/P1P1P3/2PQ4/6PP/3BB1K1 b - - 1 26");
    iter = chess_game_get_iterator(game);
    chess_game_iterator_append_move(iter, MV(H5,F4));
    chess_game_iterator_append_move(iter, MV(D3,C2));
    chess_game_iterator_append_move(iter, MV(D7,A4));
    chess_game_set_result(game, CHESS_RESULT_BLACK_WINS);
    chess_print_game_moves(game, (ChessWriter*)&writer);
    ASSERT_BUFFER_VALUE(&writer, "26... Nf4 27. Qc2 Bxa4 0-1");

    chess_buffer_writer_cleanup(&writer);
    chess_game_iterator_destroy(iter);
    chess_game_destroy(game);
}

static void test_print_game_moves_nested(void)
{
    ChessGame* game;
    ChessGameIterator* iter;
    ChessBufferWriter writer;

    chess_buffer_writer_init(&writer);
    game = chess_game_new();
    iter = chess_game_get_iterator(game);

    chess_game_iterator_append_move(iter, MV(E2,E4));
    chess_game_iterator_append_move(iter, MV(E7,E5));
    chess_game_iterator_append_move(iter, MV(F2,F4));
    chess_game_iterator_step_back(iter);
    chess_game_iterator_append_move(iter, MV(G1,F3));
    chess_game_iterator_append_move(iter, MV(B8,C6));
    chess_game_iterator_step_back(iter);
    chess_game_iterator_append_move(iter, MV(G8,F6));
    chess_print_game_moves(game, (ChessWriter*)&writer);
    ASSERT_BUFFER_VALUE(&writer, "1. e4 e5 2. f4 (2. Nf3 Nc6 (2... Nf6)) *");

    chess_buffer_writer_clear(&writer);
    chess_game_iterator_step_to_start(iter);
    chess_game_iterator_append_move(iter, MV(D2,D4));
    chess_game_iterator_step_back(iter);
    chess_game_iterator_append_move(iter, MV(C2,C4));
    chess_game_iterator_append_move(iter, MV(C7,C5));
    chess_print_game_moves(game, (ChessWriter*)&writer);
    ASSERT_BUFFER_VALUE(&writer, "1. e4 (1. d4) (1. c4 c5) 1... e5 2. f4 (2. Nf3 Nc6 (2... Nf6)) *");

    chess_buffer_writer_clear(&writer);
    chess_game_iterator_step_to_start(iter);
    chess_game_iterator_step_to_end(iter);
    chess_game_iterator_append_move(iter, MV(E5,F4));
    chess_game_iterator_append_move(iter, MV(F1,C4));
    chess_print_game_moves(game, (ChessWriter*)&writer);
    ASSERT_BUFFER_VALUE(&writer, "1. e4 (1. d4) (1. c4 c5) 1... e5 2. f4 (2. Nf3 Nc6 (2... Nf6)) 2... exf4 3. Bc4 *");

    chess_buffer_writer_cleanup(&writer);
    chess_game_iterator_destroy(iter);
    chess_game_destroy(game);
}

static void test_print_game_moves_nags(void)
{
    ChessGame* game;
    ChessGameIterator* iter;
    ChessBufferWriter writer;

    chess_buffer_writer_init(&writer);
    game = chess_game_new();
    iter = chess_game_get_iterator(game);

    chess_game_iterator_append_move(iter, MV(E2,E4));
    chess_game_iterator_append_move(iter, MV(E7,E5));
    chess_variation_add_annotation(chess_game_iterator_variation(iter), 2);
    chess_game_iterator_append_move(iter, MV(F2,F4));
    chess_variation_add_annotation(chess_game_iterator_variation(iter), 1);
    chess_variation_add_annotation(chess_game_iterator_variation(iter), 13);
    chess_game_iterator_append_move(iter, MV(E5,F4));
    chess_game_iterator_step_back(iter);
    chess_game_iterator_step_back(iter);
    chess_game_iterator_append_move(iter, MV(G1,F3));
    chess_variation_add_annotation(chess_game_iterator_variation(iter), 20);
    chess_print_game_moves(game, (ChessWriter*)&writer);
    ASSERT_BUFFER_VALUE(&writer, "1. e4 e5 $2 2. f4 $1 $13 (2. Nf3 $20) 2... exf4 *");

    chess_buffer_writer_cleanup(&writer);
    chess_game_iterator_destroy(iter);
    chess_game_destroy(game);
}

static void test_print_result(void)
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

void test_print_add_tests(void)
{
    CU_Suite* suite = add_suite("print");
    CU_add_test(suite, "print_move", (CU_TestFunc)test_print_move);
    CU_add_test(suite, "print_move_san", (CU_TestFunc)test_print_move_san);
    CU_add_test(suite, "print_game_moves", (CU_TestFunc)test_print_game_moves);
    CU_add_test(suite, "print_game_moves_nested", (CU_TestFunc)test_print_game_moves_nested);
    CU_add_test(suite, "print_game_moves_nags", (CU_TestFunc)test_print_game_moves_nags);
    CU_add_test(suite, "print_result", (CU_TestFunc)test_print_result);
}
