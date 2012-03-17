#include <CUnit/CUnit.h>

#include "../pgn.h"

#include "helpers.h"

static void test_pgn_save(void)
{
    const char game1[] =
        "[Event \"\"]\n"
        "[Site \"\"]\n"
        "[Date \"\"]\n"
        "[Round \"\"]\n"
        "[White \"\"]\n"
        "[Black \"\"]\n"
        "[Result \"*\"]\n"
        "\n"
        "*\n";
    const char game2[] =
        "[Event \"World Chess Championship 1886\"]\n"
        "[Site \"New Orleans, USA\"]\n"
        "[Date \"1886.01.21\"]\n"
        "[Round \"20\"]\n"
        "[White \"Steinitz, Wilhelm\"]\n"
        "[Black \"Zukertort, Johannes\"]\n"
        "[Result \"1-0\"]\n"
        "\n"
        "1. e4 e5 2. Nc3 Nc6 3. f4 exf4 4. d4 d5 5. exd5 Qh4+ 6. Ke2 Qe7+"
        " 7. Kf2 Qh4+ 8. g3 fxg3+ 9. Kg2 Nxd4 10. hxg3 Qg4 11. Qe1+ Be7"
        " 12. Bd3 Nf5 13. Nf3 Bd7 14. Bf4 f6 15. Ne4 Ngh6 16. Bxh6 Nxh6"
        " 17. Rxh6 gxh6 18. Nxf6+ Kf8 19. Nxg4 1-0\n";
    const ChessMove game2_moves[] = {
        MV(E2,E4), MV(E7,E5),
        MV(B1,C3), MV(B8,C6),
        MV(F2,F4), MV(E5,F4),
        MV(D2,D4), MV(D7,D5),
        MV(E4,D5), MV(D8,H4),
        MV(E1,E2), MV(H4,E7),
        MV(E2,F2), MV(E7,H4),
        MV(G2,G3), MV(F4,G3),
        MV(F2,G2), MV(C6,D4),
        MV(H2,G3), MV(H4,G4),
        MV(D1,E1), MV(F8,E7),
        MV(F1,D3), MV(D4,F5),
        MV(G1,F3), MV(C8,D7),
        MV(C1,F4), MV(F7,F6),
        MV(C3,E4), MV(G8,H6),
        MV(F4,H6), MV(F5,H6),
        MV(H1,H6), MV(G7,H6),
        MV(E4,F6), MV(E8,F8),
        MV(F6,G4),
    };

    ChessGame* game;
    char buf[1024];
    int i;

    game = chess_game_new();
    chess_game_init(game);
    chess_pgn_save(game, buf);
    CU_ASSERT_STRING_EQUAL(game1, buf);

    chess_game_init(game);
    chess_game_set_event(game, "World Chess Championship 1886");
    chess_game_set_site(game, "New Orleans, USA");
    chess_game_set_date(game, "1886.01.21");
    chess_game_set_round(game, 20);
    chess_game_set_white(game, "Steinitz, Wilhelm");
    chess_game_set_black(game, "Zukertort, Johannes");
    for (i = 0; i < sizeof(game2_moves) / sizeof(ChessMove); i++)
        chess_game_append_move(game, game2_moves[i]);
    chess_game_set_result(game, CHESS_RESULT_WHITE_WINS);
    chess_pgn_save(game, buf);
    CU_ASSERT_STRING_EQUAL(game2, buf);

    chess_game_destroy(game);
}

static void test_pgn_load(void)
{
    const char pgn[] =
    "[Event \"World Chess Championship 1886\"]\n"
    "[Site \"New Orleans, USA\"]\n"
    "[Date \"1886.01.21\"]\n"
    "[Round \"20\"]\n"
    "[White \"Steinitz, Wilhelm\"]\n"
    "[Black \"Zukertort, Johannes\"]\n"
    "[Result \"1-0\"]\n"
    "\n"
    "1. e4 e5 2. Nc3 Nc6 3. f4 exf4 4. d4 d5 5. exd5 Qh4+ 6. Ke2 Qe7+"
    " 7. Kf2 Qh4+ 8. g3 fxg3+ 9. Kg2 Nxd4 10. hxg3 Qg4 11. Qe1+ Be7"
    " 12. Bd3 Nf5 13. Nf3 Bd7 14. Bf4 f6 15. Ne4 Ngh6 16. Bxh6 Nxh6"
    " 17. Rxh6 gxh6 18. Nxf6+ Kf8 19. Nxg4 1-0\n";

    ChessPgnLoadResult result;
    ChessGame* game = chess_game_new();
    chess_game_init(game);
    result = chess_pgn_load(pgn, game);
    CU_ASSERT_EQUAL(CHESS_PGN_LOAD_OK, result);

    CU_ASSERT_STRING_EQUAL("World Chess Championship 1886", chess_game_event(game));
    CU_ASSERT_STRING_EQUAL("New Orleans, USA", chess_game_site(game));
    CU_ASSERT_STRING_EQUAL("1886.01.21", chess_game_date(game));
    CU_ASSERT_EQUAL(20, chess_game_round(game));
    CU_ASSERT_STRING_EQUAL("Steinitz, Wilhelm", chess_game_white(game));
    CU_ASSERT_STRING_EQUAL("Zukertort, Johannes", chess_game_black(game));
    CU_ASSERT_EQUAL(CHESS_RESULT_WHITE_WINS, chess_game_result(game));
    CU_ASSERT_EQUAL(37, chess_game_ply(game));

    chess_game_destroy(game);
}

static void test_pgn_load_subvariations(void)
{
    const char pgn1[] =
        "1. e4 (1. d4 Nf6) e5 *";
    const char pgn2[] =
        "1. d4 d5 (1... Nf6) (1... g6) 2. c4 *";
    const char pgn3[] =
        "1. c4 e5 (1... g6 2. d4 (2. Nf3)) *";

    ChessGame* game;
    ChessVariation* variation, *subvariation;
    ChessPgnLoadResult result;

    /* Test 1 */
    game = chess_game_new();
    chess_game_init(game);
    result = chess_pgn_load(pgn1, game);
    CU_ASSERT_EQUAL(CHESS_PGN_LOAD_OK, result);

    variation = chess_game_root_variation(game);
    variation = chess_variation_first_child(variation);
    CU_ASSERT_EQUAL(MV(E2,E4), chess_variation_move(variation));

    subvariation = chess_variation_right(variation);
    CU_ASSERT_EQUAL(MV(D2,D4), chess_variation_move(subvariation));
    subvariation = chess_variation_first_child(subvariation);
    CU_ASSERT_EQUAL(MV(G8,F6), chess_variation_move(subvariation));
    CU_ASSERT_EQUAL(NULL, chess_variation_first_child(subvariation));

    variation = chess_variation_first_child(variation);
    CU_ASSERT_EQUAL(MV(E7,E5), chess_variation_move(variation));
    CU_ASSERT_EQUAL(NULL, chess_variation_first_child(variation));
    chess_game_destroy(game);

    /* Test 2 */
    game = chess_game_new();
    chess_game_init(game);
    result = chess_pgn_load(pgn2, game);
    CU_ASSERT_EQUAL(CHESS_PGN_LOAD_OK, result);

    variation = chess_game_root_variation(game);
    variation = chess_variation_first_child(variation);
    CU_ASSERT_EQUAL(MV(D2,D4), chess_variation_move(variation));
    variation = chess_variation_first_child(variation);
    CU_ASSERT_EQUAL(MV(D7,D5), chess_variation_move(variation));

    subvariation = chess_variation_right(variation);
    CU_ASSERT_EQUAL(MV(G8,F6), chess_variation_move(subvariation));
    CU_ASSERT_EQUAL(NULL, chess_variation_first_child(subvariation));

    subvariation = chess_variation_right(subvariation);
    CU_ASSERT_EQUAL(MV(G7,G6), chess_variation_move(subvariation));
    CU_ASSERT_EQUAL(NULL, chess_variation_first_child(subvariation));
    CU_ASSERT_EQUAL(NULL, chess_variation_right(subvariation));

    variation = chess_variation_first_child(variation);
    CU_ASSERT_EQUAL(MV(C2,C4), chess_variation_move(variation));
    CU_ASSERT_EQUAL(NULL, chess_variation_first_child(variation));
    chess_game_destroy(game);

    /* Test 3 */
    game = chess_game_new();
    chess_game_init(game);
    result = chess_pgn_load(pgn3, game);
    CU_ASSERT_EQUAL(CHESS_PGN_LOAD_OK, result);

    variation = chess_game_root_variation(game);
    variation = chess_variation_first_child(variation);
    CU_ASSERT_EQUAL(MV(C2,C4), chess_variation_move(variation));
    variation = chess_variation_first_child(variation);
    CU_ASSERT_EQUAL(MV(E7,E5), chess_variation_move(variation));
    CU_ASSERT_EQUAL(NULL, chess_variation_first_child(variation));

    subvariation = chess_variation_right(variation);
    CU_ASSERT_EQUAL(MV(G7,G6), chess_variation_move(subvariation));
    subvariation = chess_variation_first_child(subvariation);
    CU_ASSERT_EQUAL(MV(D2,D4), chess_variation_move(subvariation));
    CU_ASSERT_EQUAL(NULL, chess_variation_first_child(subvariation));

    subvariation = chess_variation_right(subvariation);
    CU_ASSERT_EQUAL(MV(G1,F3), chess_variation_move(subvariation));
    CU_ASSERT_EQUAL(NULL, chess_variation_first_child(subvariation));
    CU_ASSERT_EQUAL(NULL, chess_variation_right(subvariation));
    chess_game_destroy(game);
}

void test_pgn_add_tests(void)
{
    CU_Suite* suite = CU_add_suite("pgn", NULL, NULL);
    CU_add_test(suite, "pgn_save", (CU_TestFunc)test_pgn_save);
    CU_add_test(suite, "pgn_load", (CU_TestFunc)test_pgn_load);
    CU_add_test(suite, "pgn_load_subvariations", (CU_TestFunc)test_pgn_load_subvariations);
}
