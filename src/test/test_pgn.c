#include <CUnit/CUnit.h>

#include "../chess.h"
#include "../move.h"
#include "../unmove.h"
#include "../position.h"
#include "../game.h"
#include "../pgn.h"

#include "helpers.h"

static void test_pgn_save()
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
    chess_game_reset(game);
    chess_pgn_save(game, buf);
    CU_ASSERT_STRING_EQUAL(game1, buf);

    chess_game_reset(game);
    chess_game_set_event(game, "World Chess Championship 1886");
    chess_game_set_site(game, "New Orleans, USA");
    chess_game_set_date(game, "1886.01.21");
    chess_game_set_round(game, 20);
    chess_game_set_white(game, "Steinitz, Wilhelm");
    chess_game_set_black(game, "Zukertort, Johannes");
    for (i = 0; i < sizeof(game2_moves) / sizeof(ChessMove); i++)
        chess_game_make_move(game, game2_moves[i]);
    chess_game_set_result(game, CHESS_RESULT_WHITE_WINS);
    chess_pgn_save(game, buf);
    CU_ASSERT_STRING_EQUAL(game2, buf);

    chess_game_destroy(game);
}

void test_pgn_add_tests()
{
    CU_Suite* suite = CU_add_suite("pgn", NULL, NULL);
    CU_add_test(suite, "pgn_save", (CU_TestFunc)test_pgn_save);
}
