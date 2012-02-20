#include <assert.h>
#include <stdio.h>

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <CUnit/Console.h>
#include <CUnit/CUCurses.h>

#include "../chess.h"
#include "../move.h"
#include "../unmove.h"
#include "../position.h"
#include "../generate.h"

void test_chess_add_tests();
void test_move_add_tests();
void test_print_add_tests();
void test_position_add_tests();
void test_generate_add_tests();
void test_fen_add_tests();
void test_unmove_add_tests();
void test_game_add_tests();
void test_pgn_add_tests();
void test_cstring_add_tests();
void test_carray_add_tests();
void test_pgn_tokenizer_add_tests();

int main (int argc, const char* argv[])
{
    CU_initialize_registry();

    chess_generate_init();

    test_chess_add_tests();
    test_move_add_tests();
    test_print_add_tests();
    test_position_add_tests();
    test_generate_add_tests();
    test_fen_add_tests();
    test_unmove_add_tests();
    test_game_add_tests();
    test_pgn_add_tests();
    test_cstring_add_tests();
    test_carray_add_tests();
    test_pgn_tokenizer_add_tests();

    CU_basic_run_tests();

    CU_cleanup_registry();

    return 0;
}
