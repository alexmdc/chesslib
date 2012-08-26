#include <CUnit/CUnit.h>

#include "../game.h"

#include "helpers.h"

static void test_game_new(void)
{
    ChessPosition* start_position;
    ChessGame* game;

    game = chess_game_new();
    chess_game_destroy(game);

    game = chess_game_new();
    chess_game_init(game);
    CU_ASSERT_EQUAL(0, chess_game_ply(game));
    CU_ASSERT_EQUAL(CHESS_RESULT_IN_PROGRESS, chess_game_result(game));

    start_position = chess_position_new();
    chess_position_init(start_position);
    ASSERT_POSITIONS_EQUAL(start_position, chess_game_initial_position(game));

    chess_position_destroy(start_position);
    chess_game_destroy(game);
}

static void test_game_move(void)
{
    ChessGame* game;
    ChessGameIterator* iter;
    ChessPosition* position;
    ChessMove moves[] = { MV(E2,E4), MV(D7,D5) };

    game = chess_game_new();
    chess_game_init(game);
    iter = chess_game_get_iterator(game);
    chess_game_iterator_append_move(iter, moves[0]);
    CU_ASSERT_EQUAL(1, chess_game_ply(game));
    chess_game_iterator_append_move(iter, moves[1]);
    CU_ASSERT_EQUAL(2, chess_game_ply(game));

    position = chess_position_new();
    chess_position_init(position);
    ASSERT_POSITIONS_EQUAL(position, chess_game_initial_position(game));
    chess_position_make_move(position, moves[0]);
    chess_position_make_move(position, moves[1]);
    ASSERT_POSITIONS_EQUAL(position, chess_game_iterator_position(iter));

    CU_ASSERT_EQUAL(moves[0], chess_game_move_at_ply(game, 0));
    CU_ASSERT_EQUAL(moves[1], chess_game_move_at_ply(game, 1));

    chess_game_iterator_step_to_start(iter);
    CU_ASSERT_EQUAL(2, chess_game_ply(game));
    chess_game_iterator_truncate_moves(iter);
    CU_ASSERT_EQUAL(0, chess_game_ply(game));
    ASSERT_POSITIONS_EQUAL(chess_game_initial_position(game), chess_game_iterator_position(iter));

    chess_position_destroy(position);
    chess_game_iterator_destroy(iter);
    chess_game_destroy(game);
}

static void test_game_result(void)
{
    ChessGame* game;
    ChessGameIterator* iter;

    game = chess_game_new();
    chess_game_init(game);
    CU_ASSERT_EQUAL(CHESS_RESULT_IN_PROGRESS, chess_game_result(game));

    iter = chess_game_get_iterator(game);
    chess_game_iterator_append_move(iter, MV(E2,E4));
    chess_game_iterator_append_move(iter, MV(E7,E5));
    chess_game_iterator_append_move(iter, MV(F1,C4));
    chess_game_iterator_append_move(iter, MV(F8,C5));
    chess_game_iterator_append_move(iter, MV(D1,H5));
    chess_game_iterator_append_move(iter, MV(G8,F6));
    chess_game_iterator_append_move(iter, MV(H5,F7));
    CU_ASSERT_EQUAL(CHESS_RESULT_WHITE_WINS, chess_game_result(game));
    CU_ASSERT_EQUAL(7, chess_game_ply(game));
    chess_game_iterator_destroy(iter);

    chess_game_init(game);
    iter = chess_game_get_iterator(game);
    chess_game_iterator_append_move(iter, MV(F2,F4));
    chess_game_iterator_append_move(iter, MV(E7,E5));
    chess_game_iterator_append_move(iter, MV(G2,G4));
    chess_game_iterator_append_move(iter, MV(D8,H4));
    CU_ASSERT_EQUAL(CHESS_RESULT_BLACK_WINS, chess_game_result(game));
    CU_ASSERT_EQUAL(4, chess_game_ply(game));
    chess_game_iterator_destroy(iter);

    chess_game_init(game);
    iter = chess_game_get_iterator(game);
    chess_game_iterator_append_move(iter, MV(E2,E3)); /* 1. e3 */
    chess_game_iterator_append_move(iter, MV(A7,A5)); /*    a5 */
    chess_game_iterator_append_move(iter, MV(D1,H5)); /* 2. Qh5 */
    chess_game_iterator_append_move(iter, MV(A8,A6)); /*    Ra6 */
    chess_game_iterator_append_move(iter, MV(H5,A5)); /* 3. Qxa5 */
    chess_game_iterator_append_move(iter, MV(H7,H5)); /*    h5 */
    chess_game_iterator_append_move(iter, MV(H2,H4)); /* 4. h4 */
    chess_game_iterator_append_move(iter, MV(A6,H6)); /*    Rah6 */
    chess_game_iterator_append_move(iter, MV(A5,C7)); /* 5. Qxc7 */
    chess_game_iterator_append_move(iter, MV(F7,F6)); /*    f6 */
    chess_game_iterator_append_move(iter, MV(C7,D7)); /* 6. Qxd7+ */
    chess_game_iterator_append_move(iter, MV(E8,F7)); /*    Kf7 */
    chess_game_iterator_append_move(iter, MV(D7,B7)); /* 7. Qxb7 */
    chess_game_iterator_append_move(iter, MV(D8,D3)); /*    Qd3 */
    chess_game_iterator_append_move(iter, MV(B7,B8)); /* 8. Qxb8 */
    chess_game_iterator_append_move(iter, MV(D3,H7)); /*    Qh7 */
    chess_game_iterator_append_move(iter, MV(B8,C8)); /* 9. Qxc8 */
    chess_game_iterator_append_move(iter, MV(F7,G6)); /*    Kg6 */
    chess_game_iterator_append_move(iter, MV(C8,E6)); /* 10. Qe6 stalemate */
    CU_ASSERT_EQUAL(CHESS_RESULT_DRAW, chess_game_result(game));
    CU_ASSERT_EQUAL(19, chess_game_ply(game));
    chess_game_iterator_step_back(iter);
    chess_game_iterator_truncate_moves(iter);
    CU_ASSERT_EQUAL(CHESS_RESULT_IN_PROGRESS, chess_game_result(game));
    CU_ASSERT_EQUAL(18, chess_game_ply(game));
    chess_game_iterator_destroy(iter);

    chess_game_destroy(game);
}

static void test_game_set_result(void)
{
    ChessGame* game;
    ChessGameIterator* iter;

    game = chess_game_new();
    chess_game_init(game);
    iter = chess_game_get_iterator(game);

    chess_game_iterator_append_move(iter, MV(E2,E4));
    chess_game_set_result(game, CHESS_RESULT_WHITE_WINS);
    CU_ASSERT_EQUAL(CHESS_RESULT_WHITE_WINS, chess_game_result(game));

    chess_game_iterator_append_move(iter, MV(E7,E5));
    CU_ASSERT_EQUAL(CHESS_RESULT_IN_PROGRESS, chess_game_result(game));

    chess_game_set_result(game, CHESS_RESULT_DRAW);
    CU_ASSERT_EQUAL(CHESS_RESULT_DRAW, chess_game_result(game));

    chess_game_iterator_step_back(iter);
    chess_game_iterator_truncate_moves(iter);
    CU_ASSERT_EQUAL(CHESS_RESULT_IN_PROGRESS, chess_game_result(game));

    chess_game_iterator_append_move(iter, MV(E7,E5));
    CU_ASSERT_EQUAL(CHESS_RESULT_IN_PROGRESS, chess_game_result(game));

    chess_game_iterator_append_move(iter, MV(F1,A6));
    chess_game_iterator_append_move(iter, MV(B8,A6));
    chess_game_set_result(game, CHESS_RESULT_BLACK_WINS);
    CU_ASSERT_EQUAL(CHESS_RESULT_BLACK_WINS, chess_game_result(game));

    chess_game_set_result(game, CHESS_RESULT_NONE);
    CU_ASSERT_EQUAL(CHESS_RESULT_IN_PROGRESS, chess_game_result(game));

    chess_game_iterator_destroy(iter);
    chess_game_destroy(game);
}

static void test_game_tags(void)
{
    ChessGame* game = chess_game_new();
    chess_game_init(game);
    CU_ASSERT_STRING_EQUAL("", chess_game_event(game));
    CU_ASSERT_STRING_EQUAL("", chess_game_site(game));
    CU_ASSERT_STRING_EQUAL("", chess_game_date(game));
    CU_ASSERT_STRING_EQUAL("", chess_game_round(game));
    CU_ASSERT_STRING_EQUAL("", chess_game_white(game));
    CU_ASSERT_STRING_EQUAL("", chess_game_black(game));
    CU_ASSERT_EQUAL(CHESS_RESULT_IN_PROGRESS, chess_game_result(game));

    chess_game_set_event(game, "F/S Return Match");
    chess_game_set_site(game, "Belgrade, Serbia JUG");
    chess_game_set_date(game, "1992.11.04");
    chess_game_set_round(game, "29");
    chess_game_set_white(game, "Fischer, Robert J.");
    chess_game_set_black(game, "Spassky, Boris V.");
    chess_game_set_result(game, CHESS_RESULT_DRAW);
    CU_ASSERT_STRING_EQUAL("F/S Return Match", chess_game_event(game));
    CU_ASSERT_STRING_EQUAL("Belgrade, Serbia JUG", chess_game_site(game));
    CU_ASSERT_STRING_EQUAL("1992.11.04", chess_game_date(game));
    CU_ASSERT_STRING_EQUAL("29", chess_game_round(game));
    CU_ASSERT_STRING_EQUAL("Fischer, Robert J.", chess_game_white(game));
    CU_ASSERT_STRING_EQUAL("Spassky, Boris V.", chess_game_black(game));
    CU_ASSERT_EQUAL(CHESS_RESULT_DRAW, chess_game_result(game));

    chess_game_init(game);
    CU_ASSERT_STRING_EQUAL("", chess_game_event(game));
    CU_ASSERT_STRING_EQUAL("", chess_game_site(game));
    CU_ASSERT_STRING_EQUAL("", chess_game_date(game));
    CU_ASSERT_STRING_EQUAL("", chess_game_round(game));
    CU_ASSERT_STRING_EQUAL("", chess_game_white(game));
    CU_ASSERT_STRING_EQUAL("", chess_game_black(game));
    CU_ASSERT_EQUAL(CHESS_RESULT_IN_PROGRESS, chess_game_result(game));

    chess_game_destroy(game);
}

static void test_game_extra_tags(void)
{
    ChessGame* game;

    game = chess_game_new();
    chess_game_init(game);

    CU_ASSERT_EQUAL(NULL, chess_game_tag_value(game, "Annotator"));
    CU_ASSERT_EQUAL(NULL, chess_game_tag_value(game, "ECO"));
    CU_ASSERT_EQUAL(NULL, chess_game_tag_value(game, "PlyCount"));

    chess_game_set_tag(game, "Annotator", "Fritz");
    CU_ASSERT_STRING_EQUAL("Fritz", chess_game_tag_value(game, "Annotator"));
    CU_ASSERT_EQUAL(NULL, chess_game_tag_value(game, "ECO"));
    CU_ASSERT_EQUAL(NULL, chess_game_tag_value(game, "PlyCount"));

    chess_game_set_tag(game, "ECO", "B47");
    CU_ASSERT_STRING_EQUAL("B47", chess_game_tag_value(game, "ECO"));
    CU_ASSERT_STRING_EQUAL("Fritz", chess_game_tag_value(game, "Annotator"));
    CU_ASSERT_EQUAL(NULL, chess_game_tag_value(game, "PlyCount"));

    chess_game_set_tag(game, "PlyCount", "65");
    CU_ASSERT_STRING_EQUAL("B47", chess_game_tag_value(game, "ECO"));
    CU_ASSERT_STRING_EQUAL("Fritz", chess_game_tag_value(game, "Annotator"));
    CU_ASSERT_STRING_EQUAL("65", chess_game_tag_value(game, "PlyCount"));

    chess_game_set_tag(game, "ECO", "C80");
    CU_ASSERT_STRING_EQUAL("C80", chess_game_tag_value(game, "ECO"));
    CU_ASSERT_STRING_EQUAL("Fritz", chess_game_tag_value(game, "Annotator"));
    CU_ASSERT_STRING_EQUAL("65", chess_game_tag_value(game, "PlyCount"));

    chess_game_set_tag(game, "Annotator", "Junior");
    CU_ASSERT_STRING_EQUAL("C80", chess_game_tag_value(game, "ECO"));
    CU_ASSERT_STRING_EQUAL("Junior", chess_game_tag_value(game, "Annotator"));
    CU_ASSERT_STRING_EQUAL("65", chess_game_tag_value(game, "PlyCount"));

    chess_game_remove_tag(game, "Annotator");
    CU_ASSERT_EQUAL(NULL, chess_game_tag_value(game, "Annotator"));
    CU_ASSERT_STRING_EQUAL("C80", chess_game_tag_value(game, "ECO"));
    CU_ASSERT_STRING_EQUAL("65", chess_game_tag_value(game, "PlyCount"));

    chess_game_remove_tag(game, "PlyCount");
    CU_ASSERT_EQUAL(NULL, chess_game_tag_value(game, "Annotator"));
    CU_ASSERT_STRING_EQUAL("C80", chess_game_tag_value(game, "ECO"));
    CU_ASSERT_EQUAL(NULL, chess_game_tag_value(game, "PlyCount"));

    chess_game_remove_tag(game, "ECO");
    CU_ASSERT_EQUAL(NULL, chess_game_tag_value(game, "Annotator"));
    CU_ASSERT_EQUAL(NULL, chess_game_tag_value(game, "ECO"));
    CU_ASSERT_EQUAL(NULL, chess_game_tag_value(game, "PlyCount"));

    chess_game_destroy(game);

    /* Check tags are properly cleaned up on delete */
    game = chess_game_new();
    chess_game_init(game);
    chess_game_set_tag(game, "Annotator", "Fritz");
    chess_game_set_tag(game, "ECO", "B47");
    chess_game_set_tag(game, "PlyCount", "65");
    chess_game_destroy(game);

    /* Check that extra tags interact with STR tags */
    game = chess_game_new();
    chess_game_init(game);
    chess_game_set_event(game, "F/S Return Match");
    chess_game_set_site(game, "Belgrade, Serbia JUG");
    chess_game_set_date(game, "1992.11.04");
    chess_game_set_round(game, "29");
    chess_game_set_white(game, "Fischer, Robert J.");
    chess_game_set_black(game, "Spassky, Boris V.");
    chess_game_set_result(game, CHESS_RESULT_DRAW);
    CU_ASSERT_STRING_EQUAL("F/S Return Match", chess_game_tag_value(game, "Event"));
    CU_ASSERT_STRING_EQUAL("Belgrade, Serbia JUG", chess_game_tag_value(game, "Site"));
    CU_ASSERT_STRING_EQUAL("1992.11.04", chess_game_tag_value(game, "Date"));
    CU_ASSERT_STRING_EQUAL("29", chess_game_tag_value(game, "Round"));
    CU_ASSERT_STRING_EQUAL("Fischer, Robert J.", chess_game_tag_value(game, "White"));
    CU_ASSERT_STRING_EQUAL("Spassky, Boris V.", chess_game_tag_value(game, "Black"));
    CU_ASSERT_STRING_EQUAL("1/2-1/2", chess_game_tag_value(game, "Result"));
    chess_game_set_result(game, CHESS_RESULT_WHITE_WINS);
    CU_ASSERT_STRING_EQUAL("1-0", chess_game_tag_value(game, "Result"));
    chess_game_set_result(game, CHESS_RESULT_BLACK_WINS);
    CU_ASSERT_STRING_EQUAL("0-1", chess_game_tag_value(game, "Result"));
    chess_game_set_result(game, CHESS_RESULT_IN_PROGRESS);
    CU_ASSERT_STRING_EQUAL("*", chess_game_tag_value(game, "Result"));
    /* chess_game_set_result(game, CHESS_RESULT_NONE);
    CU_ASSERT_STRING_EQUAL("", chess_game_tag_value(game, "Result")); */

    chess_game_set_tag(game, "Event", "Eurotel Trophy");
    chess_game_set_tag(game, "Site", "Prague CZE");
    chess_game_set_tag(game, "Date", "2002.04.29");
    chess_game_set_tag(game, "Round", "1.2");
    chess_game_set_tag(game, "White", "Kramnik, Vladimir");
    chess_game_set_tag(game, "Black", "Hracek, Z.");
    chess_game_set_tag(game, "Result", "1-0");
    CU_ASSERT_STRING_EQUAL("Eurotel Trophy", chess_game_event(game));
    CU_ASSERT_STRING_EQUAL("Prague CZE", chess_game_site(game));
    CU_ASSERT_STRING_EQUAL("2002.04.29", chess_game_date(game));
    CU_ASSERT_STRING_EQUAL("1.2", chess_game_round(game));
    CU_ASSERT_STRING_EQUAL("Kramnik, Vladimir", chess_game_white(game));
    CU_ASSERT_STRING_EQUAL("Hracek, Z.", chess_game_black(game));
    CU_ASSERT_EQUAL(CHESS_RESULT_WHITE_WINS, chess_game_result(game));
    chess_game_set_tag(game, "Result", "0-1");
    CU_ASSERT_EQUAL(CHESS_RESULT_BLACK_WINS, chess_game_result(game));
    chess_game_set_tag(game, "Result", "1/2-1/2");
    CU_ASSERT_EQUAL(CHESS_RESULT_DRAW, chess_game_result(game));
    chess_game_set_tag(game, "Result", "*");
    CU_ASSERT_EQUAL(CHESS_RESULT_IN_PROGRESS, chess_game_result(game));

    chess_game_remove_tag(game, "Event");
    chess_game_remove_tag(game, "Site");
    chess_game_remove_tag(game, "Date");
    chess_game_remove_tag(game, "Round");
    chess_game_remove_tag(game, "White");
    chess_game_remove_tag(game, "Black");
    chess_game_remove_tag(game, "Result");
    CU_ASSERT_STRING_EQUAL("", chess_game_event(game));
    CU_ASSERT_STRING_EQUAL("", chess_game_site(game));
    CU_ASSERT_STRING_EQUAL("", chess_game_date(game));
    CU_ASSERT_STRING_EQUAL("", chess_game_round(game));
    CU_ASSERT_STRING_EQUAL("", chess_game_white(game));
    CU_ASSERT_STRING_EQUAL("", chess_game_black(game));
    /* CU_ASSERT_EQUAL(CHESS_RESULT_NONE, chess_game_result(game)); */

    chess_game_destroy(game);
}

static void test_game_tag_iterator(void)
{
    ChessGame *game;
    ChessGameTagIterator iter;

    game = chess_game_new();
    chess_game_init(game);

    iter = chess_game_get_tag_iterator(game);
    CU_ASSERT(chess_game_tag_iterator_next(&iter));
    CU_ASSERT_STRING_EQUAL("Event", chess_game_tag_iterator_name(&iter));
    CU_ASSERT_STRING_EQUAL("", chess_game_tag_iterator_value(&iter));
    CU_ASSERT(chess_game_tag_iterator_next(&iter));
    CU_ASSERT_STRING_EQUAL("Site", chess_game_tag_iterator_name(&iter));
    CU_ASSERT_STRING_EQUAL("", chess_game_tag_iterator_value(&iter));
    CU_ASSERT(chess_game_tag_iterator_next(&iter));
    CU_ASSERT_STRING_EQUAL("Date", chess_game_tag_iterator_name(&iter));
    CU_ASSERT_STRING_EQUAL("", chess_game_tag_iterator_value(&iter));
    CU_ASSERT(chess_game_tag_iterator_next(&iter));
    CU_ASSERT_STRING_EQUAL("Round", chess_game_tag_iterator_name(&iter));
    CU_ASSERT_STRING_EQUAL("", chess_game_tag_iterator_value(&iter));
    CU_ASSERT(chess_game_tag_iterator_next(&iter));
    CU_ASSERT_STRING_EQUAL("White", chess_game_tag_iterator_name(&iter));
    CU_ASSERT_STRING_EQUAL("", chess_game_tag_iterator_value(&iter));
    CU_ASSERT(chess_game_tag_iterator_next(&iter));
    CU_ASSERT_STRING_EQUAL("Black", chess_game_tag_iterator_name(&iter));
    CU_ASSERT_STRING_EQUAL("", chess_game_tag_iterator_value(&iter));
    CU_ASSERT(chess_game_tag_iterator_next(&iter));
    CU_ASSERT_STRING_EQUAL("Result", chess_game_tag_iterator_name(&iter));
    /* CU_ASSERT_STRING_EQUAL("", chess_game_tag_iterator_value(&iter)); */
    CU_ASSERT(!chess_game_tag_iterator_next(&iter));

    chess_game_set_event(game, "F/S Return Match");
    chess_game_set_site(game, "Belgrade, Serbia JUG");
    chess_game_set_date(game, "1992.11.04");
    chess_game_set_round(game, "29");
    chess_game_set_white(game, "Fischer, Robert J.");
    chess_game_set_black(game, "Spassky, Boris V.");
    chess_game_set_result(game, CHESS_RESULT_DRAW);
    chess_game_set_tag(game, "Annotator", "Fritz");
    chess_game_set_tag(game, "ECO", "B47");
    chess_game_set_tag(game, "PlyCount", "65");

    iter = chess_game_get_tag_iterator(game);
    CU_ASSERT(chess_game_tag_iterator_next(&iter));
    CU_ASSERT_STRING_EQUAL("Event", chess_game_tag_iterator_name(&iter));
    CU_ASSERT_STRING_EQUAL("F/S Return Match", chess_game_tag_iterator_value(&iter));
    CU_ASSERT(chess_game_tag_iterator_next(&iter));
    CU_ASSERT_STRING_EQUAL("Site", chess_game_tag_iterator_name(&iter));
    CU_ASSERT_STRING_EQUAL("Belgrade, Serbia JUG", chess_game_tag_iterator_value(&iter));
    CU_ASSERT(chess_game_tag_iterator_next(&iter));
    CU_ASSERT_STRING_EQUAL("Date", chess_game_tag_iterator_name(&iter));
    CU_ASSERT_STRING_EQUAL("1992.11.04", chess_game_tag_iterator_value(&iter));
    CU_ASSERT(chess_game_tag_iterator_next(&iter));
    CU_ASSERT_STRING_EQUAL("Round", chess_game_tag_iterator_name(&iter));
    CU_ASSERT_STRING_EQUAL("29", chess_game_tag_iterator_value(&iter));
    CU_ASSERT(chess_game_tag_iterator_next(&iter));
    CU_ASSERT_STRING_EQUAL("White", chess_game_tag_iterator_name(&iter));
    CU_ASSERT_STRING_EQUAL("Fischer, Robert J.", chess_game_tag_iterator_value(&iter));
    CU_ASSERT(chess_game_tag_iterator_next(&iter));
    CU_ASSERT_STRING_EQUAL("Black", chess_game_tag_iterator_name(&iter));
    CU_ASSERT_STRING_EQUAL("Spassky, Boris V.", chess_game_tag_iterator_value(&iter));
    CU_ASSERT(chess_game_tag_iterator_next(&iter));
    CU_ASSERT_STRING_EQUAL("Result", chess_game_tag_iterator_name(&iter));
    CU_ASSERT_STRING_EQUAL("1/2-1/2", chess_game_tag_iterator_value(&iter));
    CU_ASSERT(chess_game_tag_iterator_next(&iter));
    CU_ASSERT_STRING_EQUAL("Annotator", chess_game_tag_iterator_name(&iter));
    CU_ASSERT_STRING_EQUAL("Fritz", chess_game_tag_iterator_value(&iter));
    CU_ASSERT(chess_game_tag_iterator_next(&iter));
    CU_ASSERT_STRING_EQUAL("ECO", chess_game_tag_iterator_name(&iter));
    CU_ASSERT_STRING_EQUAL("B47", chess_game_tag_iterator_value(&iter));
    CU_ASSERT(chess_game_tag_iterator_next(&iter));
    CU_ASSERT_STRING_EQUAL("PlyCount", chess_game_tag_iterator_name(&iter));
    CU_ASSERT_STRING_EQUAL("65", chess_game_tag_iterator_value(&iter));
    CU_ASSERT(!chess_game_tag_iterator_next(&iter));

    chess_game_destroy(game);
}

static void test_game_step_to_end(void)
{
    ChessGame* game;
    ChessGameIterator* iter;
    ChessPosition position_temp;

    game = chess_game_new();
    chess_game_init(game);
    iter = chess_game_get_iterator(game);

    chess_game_iterator_step_to_end(iter);
    CU_ASSERT_EQUAL(0, chess_game_ply(game));
    ASSERT_POSITIONS_EQUAL(chess_game_initial_position(game), chess_game_iterator_position(iter));

    chess_game_iterator_append_move(iter, MV(E2,E4));
    chess_game_iterator_append_move(iter, MV(E7,E5));
    chess_game_iterator_append_move(iter, MV(G1,F3));
    chess_position_copy(chess_game_iterator_position(iter), &position_temp);

    chess_game_iterator_step_to_end(iter);
    ASSERT_POSITIONS_EQUAL(&position_temp, chess_game_iterator_position(iter));
    CU_ASSERT_EQUAL(MV(G1,F3), chess_game_iterator_move(iter));

    chess_game_iterator_step_to_start(iter);
    ASSERT_POSITIONS_EQUAL(chess_game_initial_position(game), chess_game_iterator_position(iter));

    chess_game_iterator_step_to_end(iter);
    ASSERT_POSITIONS_EQUAL(&position_temp, chess_game_iterator_position(iter));
    CU_ASSERT_EQUAL(MV(G1,F3), chess_game_iterator_move(iter));

    chess_game_iterator_destroy(iter);
    chess_game_destroy(game);
}

static void test_game_step_to_move(void)
{
    ChessGame* game;
    ChessGameIterator* iter;
    ChessVariation* variation_e4_e5_Nf3_Nc6_d4;
    ChessPosition position_e4_e5_Nf3_Nc6_d4;
    ChessVariation* variation_d4_Nf6_c4_g6_Nc3;
    ChessPosition position_d4_Nf6_c4_g6_Nc3;

    game = chess_game_new();
    chess_game_init(game);
    iter = chess_game_get_iterator(game);

    chess_game_iterator_step_to_move(iter, chess_game_root_variation(game));
    ASSERT_POSITIONS_EQUAL(chess_game_initial_position(game), chess_game_iterator_position(iter));

    chess_game_iterator_append_move(iter, MV(E2,E4));
    chess_game_iterator_append_move(iter, MV(E7,E5));
    chess_game_iterator_append_move(iter, MV(G1,F3));
    chess_game_iterator_append_move(iter, MV(B8,C6));
    chess_game_iterator_append_move(iter, MV(D2,D4));
    variation_e4_e5_Nf3_Nc6_d4 = chess_game_iterator_variation(iter);
    chess_position_copy(chess_game_iterator_position(iter), &position_e4_e5_Nf3_Nc6_d4);

    chess_game_iterator_step_to_move(iter, chess_game_root_variation(game));
    ASSERT_POSITIONS_EQUAL(chess_game_initial_position(game), chess_game_iterator_position(iter));

    chess_game_iterator_append_move(iter, MV(D2,D4));
    chess_game_iterator_append_move(iter, MV(G8,F6));
    chess_game_iterator_append_move(iter, MV(C2,C4));
    chess_game_iterator_append_move(iter, MV(G7,G6));
    chess_game_iterator_append_move(iter, MV(B1,C3));
    variation_d4_Nf6_c4_g6_Nc3 = chess_game_iterator_variation(iter);
    chess_position_copy(chess_game_iterator_position(iter), &position_d4_Nf6_c4_g6_Nc3);

    chess_game_iterator_step_to_move(iter, chess_game_root_variation(game));
    ASSERT_POSITIONS_EQUAL(chess_game_initial_position(game), chess_game_iterator_position(iter));

    chess_game_iterator_step_to_move(iter, variation_e4_e5_Nf3_Nc6_d4);
    ASSERT_POSITIONS_EQUAL(&position_e4_e5_Nf3_Nc6_d4, chess_game_iterator_position(iter));
    CU_ASSERT_EQUAL(variation_e4_e5_Nf3_Nc6_d4, chess_game_iterator_variation(iter));
    CU_ASSERT_EQUAL(MV(D2,D4), chess_game_iterator_move(iter));

    chess_game_iterator_step_to_move(iter, variation_d4_Nf6_c4_g6_Nc3);
    ASSERT_POSITIONS_EQUAL(&position_d4_Nf6_c4_g6_Nc3, chess_game_iterator_position(iter));
    CU_ASSERT_EQUAL(variation_d4_Nf6_c4_g6_Nc3, chess_game_iterator_variation(iter));
    CU_ASSERT_EQUAL(MV(B1,C3), chess_game_iterator_move(iter));
}

void test_game_add_tests(void)
{
    CU_Suite* suite = CU_add_suite("game", NULL, NULL);
    CU_add_test(suite, "game_new", (CU_TestFunc)test_game_new);
    CU_add_test(suite, "game_move", (CU_TestFunc)test_game_move);
    CU_add_test(suite, "game_result", (CU_TestFunc)test_game_result);
    CU_add_test(suite, "game_set_result", (CU_TestFunc)test_game_set_result);
    CU_add_test(suite, "game_tags", (CU_TestFunc)test_game_tags);
    CU_add_test(suite, "game_extra_tags", (CU_TestFunc)test_game_extra_tags);
    CU_add_test(suite, "game_tag_iterator", (CU_TestFunc)test_game_tag_iterator);
    CU_add_test(suite, "game_step_to_end", (CU_TestFunc)test_game_step_to_end);
    CU_add_test(suite, "game_step_to_move", (CU_TestFunc)test_game_step_to_move);
}
