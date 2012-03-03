#include <CUnit/CUnit.h>

#include "../chess.h"
#include "../move.h"
#include "../unmove.h"
#include "../position.h"
#include "../fen.h"
#include "../cstring.h"
#include "../variation.h"
#include "../game.h"

#include "helpers.h"

static void test_game_new()
{
    ChessPosition* start_position;
    ChessGame* game;

    game = chess_game_new();
    chess_game_destroy(game);

    game = chess_game_new();
    chess_game_reset(game);
    CU_ASSERT_EQUAL(0, chess_game_ply(game));
    CU_ASSERT_EQUAL(CHESS_RESULT_IN_PROGRESS, chess_game_result(game));

    start_position = chess_position_new();
    chess_position_init(start_position);
    CU_ASSERT_NOT_EQUAL(chess_game_initial_position(game), chess_game_position(game));
    ASSERT_POSITIONS_EQUAL(start_position, chess_game_initial_position(game));
    ASSERT_POSITIONS_EQUAL(start_position, chess_game_position(game));

    chess_position_destroy(start_position);
    chess_game_destroy(game);
}

static void test_game_move()
{
    ChessGame* game;
    ChessPosition* position;
    ChessMove moves[] = { MV(E2,E4), MV(D7,D5) };

    game = chess_game_new();
    chess_game_reset(game);
    chess_game_make_move(game, moves[0]);
    CU_ASSERT_EQUAL(1, chess_game_ply(game));
    chess_game_make_move(game, moves[1]);
    CU_ASSERT_EQUAL(2, chess_game_ply(game));

    position = chess_position_new();
    chess_position_init(position);
    ASSERT_POSITIONS_EQUAL(position, chess_game_initial_position(game));
    chess_position_make_move(position, moves[0]);
    chess_position_make_move(position, moves[1]);
    ASSERT_POSITIONS_EQUAL(position, chess_game_position(game));

    CU_ASSERT_EQUAL(moves[0], chess_game_move(game, 0));
    CU_ASSERT_EQUAL(moves[1], chess_game_move(game, 1));

    chess_game_undo_move(game);
    chess_game_undo_move(game);
    CU_ASSERT_EQUAL(0, chess_game_ply(game));
    ASSERT_POSITIONS_EQUAL(chess_game_initial_position(game), chess_game_position(game));

    chess_position_destroy(position);
    chess_game_destroy(game);
}

static void test_game_result()
{
    ChessGame* game;

    game = chess_game_new();
    chess_game_reset(game);
    CU_ASSERT_EQUAL(CHESS_RESULT_IN_PROGRESS, chess_game_result(game));
    chess_game_make_move(game, MV(E2,E4));
    chess_game_make_move(game, MV(E7,E5));
    chess_game_make_move(game, MV(F1,C4));
    chess_game_make_move(game, MV(F8,C5));
    chess_game_make_move(game, MV(D1,H5));
    chess_game_make_move(game, MV(G8,F6));
    chess_game_make_move(game, MV(H5,F7));
    CU_ASSERT_EQUAL(CHESS_RESULT_WHITE_WINS, chess_game_result(game));
    CU_ASSERT_EQUAL(7, chess_game_ply(game));

    chess_game_reset(game);
    chess_game_make_move(game, MV(F2,F4));
    chess_game_make_move(game, MV(E7,E5));
    chess_game_make_move(game, MV(G2,G4));
    chess_game_make_move(game, MV(D8,H4));
    CU_ASSERT_EQUAL(CHESS_RESULT_BLACK_WINS, chess_game_result(game));
    CU_ASSERT_EQUAL(4, chess_game_ply(game));

    chess_game_reset(game);
    chess_game_make_move(game, MV(E2,E3)); /* 1. e3 */
    chess_game_make_move(game, MV(A7,A5)); /*    a5 */
    chess_game_make_move(game, MV(D1,H5)); /* 2. Qh5 */
    chess_game_make_move(game, MV(A8,A6)); /*    Ra6 */
    chess_game_make_move(game, MV(H5,A5)); /* 3. Qxa5 */
    chess_game_make_move(game, MV(H7,H5)); /*    h5 */
    chess_game_make_move(game, MV(H2,H4)); /* 4. h4 */
    chess_game_make_move(game, MV(A6,H6)); /*    Rah6 */
    chess_game_make_move(game, MV(A5,C7)); /* 5. Qxc7 */
    chess_game_make_move(game, MV(F7,F6)); /*    f6 */
    chess_game_make_move(game, MV(C7,D7)); /* 6. Qxd7+ */
    chess_game_make_move(game, MV(E8,F7)); /*    Kf7 */
    chess_game_make_move(game, MV(D7,B7)); /* 7. Qxb7 */
    chess_game_make_move(game, MV(D8,D3)); /*    Qd3 */
    chess_game_make_move(game, MV(B7,B8)); /* 8. Qxb8 */
    chess_game_make_move(game, MV(D3,H7)); /*    Qh7 */
    chess_game_make_move(game, MV(B8,C8)); /* 9. Qxc8 */
    chess_game_make_move(game, MV(F7,G6)); /*    Kg6 */
    chess_game_make_move(game, MV(C8,E6)); /* 10. Qe6 stalemate */
    CU_ASSERT_EQUAL(CHESS_RESULT_DRAW, chess_game_result(game));
    CU_ASSERT_EQUAL(19, chess_game_ply(game));
    chess_game_undo_move(game);
    CU_ASSERT_EQUAL(CHESS_RESULT_IN_PROGRESS, chess_game_result(game));
    CU_ASSERT_EQUAL(18, chess_game_ply(game));

    chess_game_destroy(game);
}

static void test_game_set_result()
{
    ChessGame* game;

    game = chess_game_new();
    chess_game_reset(game);
    chess_game_make_move(game, MV(E2,E4));
    chess_game_set_result(game, CHESS_RESULT_WHITE_WINS);
    CU_ASSERT_EQUAL(CHESS_RESULT_WHITE_WINS, chess_game_result(game));

    chess_game_make_move(game, MV(E7,E5));
    CU_ASSERT_EQUAL(CHESS_RESULT_IN_PROGRESS, chess_game_result(game));

    chess_game_set_result(game, CHESS_RESULT_DRAW);
    CU_ASSERT_EQUAL(CHESS_RESULT_DRAW, chess_game_result(game));

    chess_game_undo_move(game);
    CU_ASSERT_EQUAL(CHESS_RESULT_IN_PROGRESS, chess_game_result(game));

    chess_game_make_move(game, MV(E7,E5));
    CU_ASSERT_EQUAL(CHESS_RESULT_IN_PROGRESS, chess_game_result(game));

    chess_game_make_move(game, MV(F1,A6));
    chess_game_make_move(game, MV(B8,A6));
    chess_game_set_result(game, CHESS_RESULT_BLACK_WINS);
    CU_ASSERT_EQUAL(CHESS_RESULT_BLACK_WINS, chess_game_result(game));

    chess_game_set_result(game, CHESS_RESULT_NONE);
    CU_ASSERT_EQUAL(CHESS_RESULT_IN_PROGRESS, chess_game_result(game));

    chess_game_destroy(game);
}

static void test_game_tags()
{
    ChessGame* game = chess_game_new();
    chess_game_reset(game);
    CU_ASSERT_STRING_EQUAL("", chess_game_event(game));
    CU_ASSERT_STRING_EQUAL("", chess_game_site(game));
    CU_ASSERT_STRING_EQUAL("", chess_game_date(game));
    CU_ASSERT_EQUAL(0, chess_game_round(game));
    CU_ASSERT_STRING_EQUAL("", chess_game_white(game));
    CU_ASSERT_STRING_EQUAL("", chess_game_black(game));
    CU_ASSERT_EQUAL(CHESS_RESULT_IN_PROGRESS, chess_game_result(game));

    chess_game_set_event(game, "F/S Return Match");
    chess_game_set_site(game, "Belgrade, Serbia JUG");
    chess_game_set_date(game, "1992.11.04");
    chess_game_set_round(game, 29);
    chess_game_set_white(game, "Fischer, Robert J.");
    chess_game_set_black(game, "Spassky, Boris V.");
    chess_game_set_result(game, CHESS_RESULT_DRAW);
    CU_ASSERT_STRING_EQUAL("F/S Return Match", chess_game_event(game));
    CU_ASSERT_STRING_EQUAL("Belgrade, Serbia JUG", chess_game_site(game));
    CU_ASSERT_STRING_EQUAL("1992.11.04", chess_game_date(game));
    CU_ASSERT_EQUAL(29, chess_game_round(game));
    CU_ASSERT_STRING_EQUAL("Fischer, Robert J.", chess_game_white(game));
    CU_ASSERT_STRING_EQUAL("Spassky, Boris V.", chess_game_black(game));
    CU_ASSERT_EQUAL(CHESS_RESULT_DRAW, chess_game_result(game));

    chess_game_reset(game);
    CU_ASSERT_STRING_EQUAL("", chess_game_event(game));
    CU_ASSERT_STRING_EQUAL("", chess_game_site(game));
    CU_ASSERT_STRING_EQUAL("", chess_game_date(game));
    CU_ASSERT_EQUAL(0, chess_game_round(game));
    CU_ASSERT_STRING_EQUAL("", chess_game_white(game));
    CU_ASSERT_STRING_EQUAL("", chess_game_black(game));
    CU_ASSERT_EQUAL(CHESS_RESULT_IN_PROGRESS, chess_game_result(game));

    chess_game_destroy(game);
}

void test_game_add_tests()
{
    CU_Suite* suite = CU_add_suite("game", NULL, NULL);
    CU_add_test(suite, "game_new", (CU_TestFunc)test_game_new);
    CU_add_test(suite, "game_move", (CU_TestFunc)test_game_move);
    CU_add_test(suite, "game_result", (CU_TestFunc)test_game_result);
    CU_add_test(suite, "game_set_result", (CU_TestFunc)test_game_set_result);
    CU_add_test(suite, "game_tags", (CU_TestFunc)test_game_tags);
}
