#include <CUnit/CUnit.h>

#include "../cstring.h"
#include "../chess.h"
#include "../move.h"
#include "../unmove.h"
#include "../position.h"

#include "../pgn-tokenizer.h"

#include "helpers.h"

static void test_tag()
{
    const char tag[] = "[Event \"AUS Champs\"]\n";
    ChessPgnTokenizer* tokenizer = chess_pgn_tokenizer_new(tag);
    const ChessPgnToken* token;

    token = chess_pgn_tokenizer_peek(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_L_BRACKET, token->type);
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_L_BRACKET, token->type);
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_SYMBOL, token->type);
    CU_ASSERT_STRING_EQUAL("Event", chess_string_data(&token->data.string));
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_STRING, token->type);
    CU_ASSERT_STRING_EQUAL("AUS Champs", chess_string_data(&token->data.string));
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_R_BRACKET, token->type);
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_EOF, token->type);
    token = chess_pgn_tokenizer_peek(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_EOF, token->type);

    chess_pgn_tokenizer_destroy(tokenizer);
}

static void test_movetext()
{
    const char text[] = "1. e4 e5 2. f4 1-0";
    ChessPgnTokenizer* tokenizer = chess_pgn_tokenizer_new(text);
    const ChessPgnToken* token;

    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_NUMBER, token->type);
    CU_ASSERT_EQUAL(1, token->data.number);
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_PERIOD, token->type);
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_SYMBOL, token->type);
    CU_ASSERT_STRING_EQUAL("e4", chess_string_data(&token->data.string));
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_SYMBOL, token->type);
    CU_ASSERT_STRING_EQUAL("e5", chess_string_data(&token->data.string));

    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_NUMBER, token->type);
    CU_ASSERT_EQUAL(2, token->data.number);
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_PERIOD, token->type);
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_SYMBOL, token->type);
    CU_ASSERT_STRING_EQUAL("f4", chess_string_data(&token->data.string));
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_ONE_ZERO, token->type);
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_EOF, token->type);

    chess_pgn_tokenizer_destroy(tokenizer);
}

static void test_black_movenum()
{
    const char text[] = "13... O-O 14. Rd1";
    ChessPgnTokenizer* tokenizer = chess_pgn_tokenizer_new(text);
    const ChessPgnToken* token;

    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_NUMBER, token->type);
    CU_ASSERT_EQUAL(13, token->data.number);
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_PERIOD, token->type);
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_PERIOD, token->type);
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_PERIOD, token->type);
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_SYMBOL, token->type);
    CU_ASSERT_STRING_EQUAL("O-O", chess_string_data(&token->data.string));

    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_NUMBER, token->type);
    CU_ASSERT_EQUAL(14, token->data.number);
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_PERIOD, token->type);
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_SYMBOL, token->type);
    CU_ASSERT_STRING_EQUAL("Rd1", chess_string_data(&token->data.string));
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_EOF, token->type);

    chess_pgn_tokenizer_destroy(tokenizer);
}

static void test_nag()
{
    const char text[] = "7. Bg5 $2 h6 $3 $17";
    ChessPgnTokenizer* tokenizer = chess_pgn_tokenizer_new(text);
    const ChessPgnToken* token;

    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_NUMBER, token->type);
    CU_ASSERT_EQUAL(7, token->data.number);
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_PERIOD, token->type);
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_SYMBOL, token->type);
    CU_ASSERT_STRING_EQUAL("Bg5", chess_string_data(&token->data.string));
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_NAG, token->type);
    CU_ASSERT_EQUAL(2, token->data.number);

    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_SYMBOL, token->type);
    CU_ASSERT_STRING_EQUAL("h6", chess_string_data(&token->data.string));
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_NAG, token->type);
    CU_ASSERT_EQUAL(3, token->data.number);
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_NAG, token->type);
    CU_ASSERT_EQUAL(17, token->data.number);
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_EOF, token->type);

    chess_pgn_tokenizer_destroy(tokenizer);
}

void test_pgn_tokenizer_add_tests()
{
    CU_Suite* suite = CU_add_suite("pgn-tokenizer", NULL, NULL);
    CU_add_test(suite, "tag", (CU_TestFunc)test_tag);
    CU_add_test(suite, "movetext", (CU_TestFunc)test_movetext);
    CU_add_test(suite, "black_movenum", (CU_TestFunc)test_black_movenum);
    CU_add_test(suite, "nag", (CU_TestFunc)test_nag);
}
