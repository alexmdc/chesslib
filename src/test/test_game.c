#include <CUnit/CUnit.h>

#include "../chess.h"
#include "../move.h"
#include "../unmove.h"
#include "../position.h"
#include "../fen.h"
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
    ChessMove moves[] = {
        chess_move_make(CHESS_SQUARE_E2, CHESS_SQUARE_E4),
        chess_move_make(CHESS_SQUARE_D7, CHESS_SQUARE_D5)
    };

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
    chess_game_make_move(game, chess_move_make(CHESS_SQUARE_E2, CHESS_SQUARE_E4));
    chess_game_make_move(game, chess_move_make(CHESS_SQUARE_E7, CHESS_SQUARE_E5));
    chess_game_make_move(game, chess_move_make(CHESS_SQUARE_F1, CHESS_SQUARE_C4));
    chess_game_make_move(game, chess_move_make(CHESS_SQUARE_F8, CHESS_SQUARE_C5));
    chess_game_make_move(game, chess_move_make(CHESS_SQUARE_D1, CHESS_SQUARE_H5));
    chess_game_make_move(game, chess_move_make(CHESS_SQUARE_G8, CHESS_SQUARE_F6));
    chess_game_make_move(game, chess_move_make(CHESS_SQUARE_H5, CHESS_SQUARE_F7));
    CU_ASSERT_EQUAL(CHESS_RESULT_WHITE_WINS, chess_game_result(game));
    CU_ASSERT_EQUAL(7, chess_game_ply(game));

    chess_game_reset(game);
    chess_game_make_move(game, chess_move_make(CHESS_SQUARE_F2, CHESS_SQUARE_F4));
    chess_game_make_move(game, chess_move_make(CHESS_SQUARE_E7, CHESS_SQUARE_E5));
    chess_game_make_move(game, chess_move_make(CHESS_SQUARE_G2, CHESS_SQUARE_G4));
    chess_game_make_move(game, chess_move_make(CHESS_SQUARE_D8, CHESS_SQUARE_H4));
    CU_ASSERT_EQUAL(CHESS_RESULT_BLACK_WINS, chess_game_result(game));
    CU_ASSERT_EQUAL(4, chess_game_ply(game));

    chess_game_reset(game);
    chess_game_make_move(game, chess_move_make(CHESS_SQUARE_E2, CHESS_SQUARE_E3)); /* 1. e3 */
    chess_game_make_move(game, chess_move_make(CHESS_SQUARE_A7, CHESS_SQUARE_A5)); /*    a5 */
    chess_game_make_move(game, chess_move_make(CHESS_SQUARE_D1, CHESS_SQUARE_H5)); /* 2. Qh5 */
    chess_game_make_move(game, chess_move_make(CHESS_SQUARE_A8, CHESS_SQUARE_A6)); /*    Ra6 */
    chess_game_make_move(game, chess_move_make(CHESS_SQUARE_H5, CHESS_SQUARE_A5)); /* 3. Qxa5 */
    chess_game_make_move(game, chess_move_make(CHESS_SQUARE_H7, CHESS_SQUARE_H5)); /*    h5 */
    chess_game_make_move(game, chess_move_make(CHESS_SQUARE_H2, CHESS_SQUARE_H4)); /* 4. h4 */
    chess_game_make_move(game, chess_move_make(CHESS_SQUARE_A6, CHESS_SQUARE_H6)); /*    Rah6 */
    chess_game_make_move(game, chess_move_make(CHESS_SQUARE_A5, CHESS_SQUARE_C7)); /* 5. Qxc7 */
    chess_game_make_move(game, chess_move_make(CHESS_SQUARE_F7, CHESS_SQUARE_F6)); /*    f6 */
    chess_game_make_move(game, chess_move_make(CHESS_SQUARE_C7, CHESS_SQUARE_D7)); /* 6. Qxd7+ */
    chess_game_make_move(game, chess_move_make(CHESS_SQUARE_E8, CHESS_SQUARE_F7)); /*    Kf7 */
    chess_game_make_move(game, chess_move_make(CHESS_SQUARE_D7, CHESS_SQUARE_B7)); /* 7. Qxb7 */
    chess_game_make_move(game, chess_move_make(CHESS_SQUARE_D8, CHESS_SQUARE_D3)); /*    Qd3 */
    chess_game_make_move(game, chess_move_make(CHESS_SQUARE_B7, CHESS_SQUARE_B8)); /* 8. Qxb8 */
    chess_game_make_move(game, chess_move_make(CHESS_SQUARE_D3, CHESS_SQUARE_H7)); /*    Qh7 */
    chess_game_make_move(game, chess_move_make(CHESS_SQUARE_B8, CHESS_SQUARE_C8)); /* 9. Qxc8 */
    chess_game_make_move(game, chess_move_make(CHESS_SQUARE_F7, CHESS_SQUARE_G6)); /*    Kg6 */
    chess_game_make_move(game, chess_move_make(CHESS_SQUARE_C8, CHESS_SQUARE_E6)); /* 10. Qe6 stalemate */
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
    chess_game_make_move(game, chess_move_make(CHESS_SQUARE_E2, CHESS_SQUARE_E4));
    chess_game_set_result(game, CHESS_RESULT_WHITE_WINS);
    CU_ASSERT_EQUAL(CHESS_RESULT_WHITE_WINS, chess_game_result(game));

    chess_game_make_move(game, chess_move_make(CHESS_SQUARE_E7, CHESS_SQUARE_E5));
    CU_ASSERT_EQUAL(CHESS_RESULT_IN_PROGRESS, chess_game_result(game));

    chess_game_set_result(game, CHESS_RESULT_DRAW);
    CU_ASSERT_EQUAL(CHESS_RESULT_DRAW, chess_game_result(game));

    chess_game_undo_move(game);
    CU_ASSERT_EQUAL(CHESS_RESULT_IN_PROGRESS, chess_game_result(game));

    chess_game_make_move(game, chess_move_make(CHESS_SQUARE_E7, CHESS_SQUARE_E5));
    CU_ASSERT_EQUAL(CHESS_RESULT_IN_PROGRESS, chess_game_result(game));

    chess_game_make_move(game, chess_move_make(CHESS_SQUARE_F1, CHESS_SQUARE_A6));
    chess_game_make_move(game, chess_move_make(CHESS_SQUARE_B8, CHESS_SQUARE_A6));
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
