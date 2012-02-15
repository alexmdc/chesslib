#include <CUnit/CUnit.h>

#include "../chess.h"
#include "../move.h"
#include "../unmove.h"
#include "../position.h"
#include "../generate.h"
#include "../fen.h"
#include "../carray.h"

#include "helpers.h"

static int initialize()
{
    chess_generate_init();
    return 0;
}

static void test_generate_moves()
{
    ChessMove expected_moves[] = {
        MV(A2,A3), MV(A2,A4),
        MV(B2,B3), MV(B2,B4),
        MV(C2,C3), MV(C2,C4),
        MV(D2,D3), MV(D2,D4),
        MV(E2,E3), MV(E2,E4),
        MV(F2,F3), MV(F2,F4),
        MV(G2,G3), MV(G2,G4),
        MV(H2,H3), MV(H2,H4),
        MV(B1,A3), MV(B1,C3),
        MV(G1,F3), MV(G1,H3),
    };
    ChessPosition* position = chess_position_new();
    ChessArray moves;

    chess_position_init(position);
    chess_array_init(&moves, sizeof(ChessMove));
    chess_generate_moves(position, &moves);
    ASSERT_SETS_EQUAL((ChessMove*)chess_array_data(&moves), chess_array_size(&moves),
                      expected_moves, sizeof(expected_moves) / sizeof(ChessMove));
    chess_array_cleanup(&moves);

    chess_position_destroy(position);
}

static void test_generate_moves2()
{
    ChessMove expected_moves[] = {
        MV(A2,A3), MV(A2,A4),
        MV(B2,B3), MV(B2,B4),
        MV(D4,D5), MV(D4,E5),
        MV(G2,G3),
        MV(H2,H3), MV(H2,H4),
        MV(C3,A4), MV(C3,B5), MV(C3,D5), MV(C3,E2), MV(C3,B1),
        MV(F3,D2), MV(F3,E5), MV(F3,G5), MV(F3,H4), MV(F3,G1),
        MV(C4,B3), MV(C4,B5), MV(C4,A6), MV(C4,D5), MV(C4,E6), MV(C4,F7), MV(C4,D3), MV(C4,E2), MV(C4,F1),
        MV(C1,D2), MV(C1,E3), MV(C1,F4), MV(C1,G5), MV(C1,H6),
        MV(A1,B1),
        MV(H1,G1), MV(H1,F1),
        MV(D1,D2), MV(D1,D3), MV(D1,E2),
        MV(E1,D2), MV(E1,E2), MV(E1,F1), MV(E1,G1),
    };
    ChessPosition* position = chess_position_new();
    ChessArray moves;

    chess_fen_load("r3kbnr/ppp1qppp/2np4/4p3/2BPP1b1/2N2N2/PPP2PPP/R1BQK2R w KQkq - 0 6", position);
    chess_array_init(&moves, sizeof(ChessMove));
    chess_generate_moves(position, &moves);
    ASSERT_SETS_EQUAL((ChessMove*)chess_array_data(&moves), chess_array_size(&moves),
                      expected_moves, sizeof(expected_moves) / sizeof(ChessMove));

    chess_position_destroy(position);
}

static void test_generate_moves3()
{
    ChessMove expected_moves[] = {
        MV(A7,A6), MV(A7,A5),
        MV(B7,B6), MV(B7,B5),
        MV(D6,D5),
        MV(E5,D4),
        MV(F7,F6), MV(F7,F5),
        MV(G7,G6), MV(G7,G5),
        MV(H7,H6), MV(H7,H5),
        MV(C6,A5), MV(C6,B4), MV(C6,D4), MV(C6,D8), MV(C6,B8),
        MV(G8,F6), MV(G8,H6),
        MV(G4,F3), MV(G4,H3), MV(G4,H5), MV(G4,F5), MV(G4,E6), MV(G4,D7), MV(G4,C8),
        MV(A8,B8), MV(A8,C8), MV(A8,D8),
        MV(E7,D8), MV(E7,D7), MV(E7,E6), MV(E7,F6), MV(E7,G5), MV(E7,H4),
        MV(E8,D8), MV(E8,D7), MV(E8,C8),
    };
    ChessPosition* position = chess_position_new();
    ChessArray moves;

    chess_fen_load("r3kbnr/ppp1qppp/2np4/4p3/2BPP1b1/2N2N2/PPP2PPP/R1BQ1RK1 b kq - 0 6", position);
    chess_array_init(&moves, sizeof(ChessMove));
    chess_generate_moves(position, &moves);
    ASSERT_SETS_EQUAL((ChessMove*)chess_array_data(&moves), chess_array_size(&moves),
                      expected_moves, sizeof(expected_moves) / sizeof(ChessMove));
    chess_array_cleanup(&moves);

    chess_position_destroy(position);
}

static void test_generate_moves4()
{
    ChessMove expected_moves[] = {
        MV(A2,A3), MV(A2,A4),
        MV(B2,B3), MV(B2,B4),
        MV(C2,C3),
        MV(E4,E5),
        MV(G2,G3), MV(G2,G4),
        MV(H2,H3), MV(H2,H4),
        MV(B1,A3), MV(B1,C3), MV(B1,D2),
        MV(F3,D2), MV(F3,D4), MV(F3,E5), MV(F3,G5), MV(F3,H4), MV(F3,G1),
        MV(C1,D2), MV(C1,E3), MV(C1,F4), MV(C1,G5), MV(C1,H6),
        MV(C4,B3), MV(C4,B5), MV(C4,A6), MV(C4,D5), MV(C4,E6), MV(C4,F7), MV(C4,D3), MV(C4,E2), MV(C4,F1),
        MV(H1,G1), MV(H1,F1),
        MV(D1,D2), MV(D1,D3), MV(D1,D4), MV(D1,E2),
        MV(E1,D2), MV(E1,E2), MV(E1,F1), MV(E1,G1),
    };
    ChessPosition* position = chess_position_new();
    ChessArray moves;

    chess_fen_load("r1bqk1nr/pppp1ppp/2n5/2b5/2BpP3/5N2/PPP2PPP/RNBQK2R w KQkq - 0 5", position);
    chess_array_init(&moves, sizeof(ChessMove));
    chess_generate_moves(position, &moves);
    ASSERT_SETS_EQUAL((ChessMove*)chess_array_data(&moves), chess_array_size(&moves),
                      expected_moves, sizeof(expected_moves) / sizeof(ChessMove));
    chess_array_cleanup(&moves);

    chess_position_destroy(position);
}

static void test_generate_moves5()
{
    ChessMove expected_moves[] = {
        MV(A7,A6),
        MV(B7,B6), MV(B7,B5),
        MV(C7,C6), MV(C7,C5),
        MV(D7,D6),
        MV(F7,F6), MV(F7,F5),
        MV(H7,H6), MV(H7,H5),
        MV(A5,B6), MV(A5,B4), MV(A5,C3), MV(A5,D2), MV(A5,E1),
        MV(A8,B8),
        MV(H8,G8), MV(H8,F8),
        MV(H4,H3), MV(H4,H2), MV(H4,H1), MV(H4,G3), MV(H4,F2), MV(H4,G4), MV(H4,F4), MV(H4,E4),
            MV(H4,D4), MV(H4,C4), MV(H4,G5), MV(H4,F6), MV(H4,E7), MV(H4,D8), MV(H4,H5), MV(H4,H6),
        MV(E8,D8), MV(E8,E7),
    };
    ChessPosition* position = chess_position_new();
    ChessArray moves;

    chess_fen_load("r1b1k2r/pppp1pBp/8/b2P4/2B4q/1Q6/P4PP1/R4RK1 b kq - 0 15", position);
    chess_array_init(&moves, sizeof(ChessMove));
    chess_generate_moves(position, &moves);
    ASSERT_SETS_EQUAL((ChessMove*)chess_array_data(&moves), chess_array_size(&moves),
                      expected_moves, sizeof(expected_moves) / sizeof(ChessMove));
    chess_array_cleanup(&moves);

    chess_position_destroy(position);
}

static void test_generate_moves6()
{
    ChessMove expected_moves[] = {
        MV(A7,A6), MV(A7,A5),
        MV(B7,B6), MV(B7,B5),
        MV(C7,C6),
        MV(G7,G6), MV(G7,G5),
        MV(H6,H5),
        MV(F8,D7), MV(F8,E6), MV(F8,G6), MV(F8,H7),
        MV(C8,D7), MV(C8,E6), MV(C8,F5), MV(C8,G4), MV(C8,H3),
        MV(E7,D6), MV(E7,C5), MV(E7,F6), MV(E7,G5), MV(E7,H4),
        MV(A8,B8),
        MV(D8,D7), MV(D8,D6), MV(D8,D5), MV(D8,D4),
        MV(G8,F7),
        MVP(E2,E1,KNIGHT), MVP(E2,E1,BISHOP), MVP(E2,E1,ROOK), MVP(E2,E1,QUEEN),
        MVP(E2,F1,KNIGHT), MVP(E2,F1,BISHOP), MVP(E2,F1,ROOK), MVP(E2,F1,QUEEN),
    };
    ChessPosition* position = chess_position_new();
    ChessArray moves;

    chess_fen_load("r1bqrnk1/ppp1bNp1/7p/2P5/3P4/3Q1N2/PPB1p1PP/R4RK1 b - - 3 16", position);
    chess_array_init(&moves, sizeof(ChessMove));
    chess_generate_moves(position, &moves);
    ASSERT_SETS_EQUAL((ChessMove*)chess_array_data(&moves), chess_array_size(&moves),
                      expected_moves, sizeof(expected_moves) / sizeof(ChessMove));
    chess_array_cleanup(&moves);

    chess_position_destroy(position);
}

void test_generate_add_tests()
{
    CU_Suite* suite = CU_add_suite("generate", (CU_InitializeFunc)initialize, NULL);
    CU_add_test(suite, "generate_moves", (CU_TestFunc)test_generate_moves);
    CU_add_test(suite, "generate_moves2", (CU_TestFunc)test_generate_moves2);
    CU_add_test(suite, "generate_moves3", (CU_TestFunc)test_generate_moves3);
    CU_add_test(suite, "generate_moves4", (CU_TestFunc)test_generate_moves4);
    CU_add_test(suite, "generate_moves5", (CU_TestFunc)test_generate_moves5);
    CU_add_test(suite, "generate_moves6", (CU_TestFunc)test_generate_moves6);
}
