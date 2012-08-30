#include <CUnit/CUnit.h>

#include "../pgn-tokenizer.h"

#include "helpers.h"

static void test_empty(void)
{
    ChessBufferReader reader;
    ChessPgnTokenizer* tokenizer;
    const ChessPgnToken* token;

    chess_buffer_reader_init(&reader, "");
    tokenizer = chess_pgn_tokenizer_new((ChessReader*)&reader);
    token = chess_pgn_tokenizer_peek(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_EOF, token->type);
    chess_pgn_tokenizer_destroy(tokenizer);
    chess_buffer_reader_cleanup(&reader);

    chess_buffer_reader_init(&reader, "  \n");
    tokenizer = chess_pgn_tokenizer_new((ChessReader*)&reader);
    token = chess_pgn_tokenizer_peek(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_EOF, token->type);
    chess_pgn_tokenizer_destroy(tokenizer);
    chess_buffer_reader_cleanup(&reader);
}

static void test_tag(void)
{
    const char tag[] = "[Event \"AUS Champs\"]\n";
    ChessBufferReader reader;
    ChessPgnTokenizer* tokenizer;
    const ChessPgnToken* token;

    chess_buffer_reader_init(&reader, tag);
    tokenizer = chess_pgn_tokenizer_new((ChessReader*)&reader);
    token = chess_pgn_tokenizer_peek(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_L_BRACKET, token->type);
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_L_BRACKET, token->type);
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_SYMBOL, token->type);
    CU_ASSERT_STRING_EQUAL("Event", chess_string_data(&token->string));
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_STRING, token->type);
    CU_ASSERT_STRING_EQUAL("AUS Champs", chess_string_data(&token->string));
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_R_BRACKET, token->type);
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_EOF, token->type);
    token = chess_pgn_tokenizer_peek(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_EOF, token->type);

    chess_pgn_tokenizer_destroy(tokenizer);
    chess_buffer_reader_cleanup(&reader);
}

static void test_escaped_string(void)
{
    const char text[] = "\"One \\\"fine\\\" move\"";
    ChessBufferReader reader;
    ChessPgnTokenizer* tokenizer;
    const ChessPgnToken* token;

    chess_buffer_reader_init(&reader, text);
    tokenizer = chess_pgn_tokenizer_new((ChessReader*)&reader);
    token = chess_pgn_tokenizer_peek(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_STRING, token->type);
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_STRING, token->type);
    CU_ASSERT_STRING_EQUAL("One \"fine\" move", chess_string_data(&token->string));
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_EOF, token->type);

    chess_pgn_tokenizer_destroy(tokenizer);
    chess_buffer_reader_cleanup(&reader);
}

static void test_movetext(void)
{
    const char text[] = "1. e4 e5 2. f4 1-0";
    ChessBufferReader reader;
    ChessPgnTokenizer* tokenizer;
    const ChessPgnToken* token;

    chess_buffer_reader_init(&reader, text);
    tokenizer = chess_pgn_tokenizer_new((ChessReader*)&reader);

    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_NUMBER, token->type);
    CU_ASSERT_EQUAL(1, token->number);
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_PERIOD, token->type);
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_SYMBOL, token->type);
    CU_ASSERT_STRING_EQUAL("e4", chess_string_data(&token->string));
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_SYMBOL, token->type);
    CU_ASSERT_STRING_EQUAL("e5", chess_string_data(&token->string));

    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_NUMBER, token->type);
    CU_ASSERT_EQUAL(2, token->number);
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_PERIOD, token->type);
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_SYMBOL, token->type);
    CU_ASSERT_STRING_EQUAL("f4", chess_string_data(&token->string));
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_ONE_ZERO, token->type);
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_EOF, token->type);

    chess_pgn_tokenizer_destroy(tokenizer);
    chess_buffer_reader_cleanup(&reader);
}

static void test_black_movenum(void)
{
    const char text[] = "13... O-O 14. Rd1";
    ChessBufferReader reader;
    ChessPgnTokenizer* tokenizer;
    const ChessPgnToken* token;

    chess_buffer_reader_init(&reader, text);
    tokenizer = chess_pgn_tokenizer_new((ChessReader*)&reader);

    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_NUMBER, token->type);
    CU_ASSERT_EQUAL(13, token->number);
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_PERIOD, token->type);
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_PERIOD, token->type);
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_PERIOD, token->type);
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_SYMBOL, token->type);
    CU_ASSERT_STRING_EQUAL("O-O", chess_string_data(&token->string));

    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_NUMBER, token->type);
    CU_ASSERT_EQUAL(14, token->number);
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_PERIOD, token->type);
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_SYMBOL, token->type);
    CU_ASSERT_STRING_EQUAL("Rd1", chess_string_data(&token->string));
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_EOF, token->type);

    chess_pgn_tokenizer_destroy(tokenizer);
    chess_buffer_reader_cleanup(&reader);
}

static void test_nag(void)
{
    const char text[] = "7. Bg5 $2 h6 $3 $17";
    ChessBufferReader reader;
    ChessPgnTokenizer* tokenizer;
    const ChessPgnToken* token;

    chess_buffer_reader_init(&reader, text);
    tokenizer = chess_pgn_tokenizer_new((ChessReader*)&reader);

    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_NUMBER, token->type);
    CU_ASSERT_EQUAL(7, token->number);
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_PERIOD, token->type);
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_SYMBOL, token->type);
    CU_ASSERT_STRING_EQUAL("Bg5", chess_string_data(&token->string));
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_NAG, token->type);
    CU_ASSERT_EQUAL(2, token->number);

    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_SYMBOL, token->type);
    CU_ASSERT_STRING_EQUAL("h6", chess_string_data(&token->string));
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_NAG, token->type);
    CU_ASSERT_EQUAL(3, token->number);
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_NAG, token->type);
    CU_ASSERT_EQUAL(17, token->number);
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_EOF, token->type);

    chess_pgn_tokenizer_destroy(tokenizer);
    chess_buffer_reader_cleanup(&reader);
}

static void test_comment(void)
{
    const char text[] = "1. e4 {A strong move} e6 {The French Defence}";
    ChessBufferReader reader;
    ChessPgnTokenizer* tokenizer;
    const ChessPgnToken* token;

    chess_buffer_reader_init(&reader, text);
    tokenizer = chess_pgn_tokenizer_new((ChessReader*)&reader);

    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_NUMBER, token->type);
    CU_ASSERT_EQUAL(1, token->number);
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_PERIOD, token->type);
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_SYMBOL, token->type);
    CU_ASSERT_STRING_EQUAL("e4", chess_string_data(&token->string));
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_COMMENT, token->type);
    CU_ASSERT_STRING_EQUAL("A strong move", chess_string_data(&token->string));

    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_SYMBOL, token->type);
    CU_ASSERT_STRING_EQUAL("e6", chess_string_data(&token->string));
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_COMMENT, token->type);
    CU_ASSERT_STRING_EQUAL("The French Defence", chess_string_data(&token->string));
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_EOF, token->type);

    chess_pgn_tokenizer_destroy(tokenizer);
    chess_buffer_reader_cleanup(&reader);
}

static void test_error(void)
{
    ChessBufferReader reader;
    ChessPgnTokenizer* tokenizer;
    const ChessPgnToken* token;

    /* Malformed symbol */
    chess_buffer_reader_init(&reader, "Qxh7 #");
    tokenizer = chess_pgn_tokenizer_new((ChessReader*)&reader);
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_SYMBOL, token->type);
    CU_ASSERT_STRING_EQUAL("Qxh7", chess_string_data(&token->string));
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_ERROR, token->type);
    chess_pgn_tokenizer_destroy(tokenizer);
    chess_buffer_reader_cleanup(&reader);

    /* Unterminated string */
    chess_buffer_reader_init(&reader, "[White \"Capa");
    tokenizer = chess_pgn_tokenizer_new((ChessReader*)&reader);
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_L_BRACKET, token->type);
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_SYMBOL, token->type);
    CU_ASSERT_STRING_EQUAL("White", chess_string_data(&token->string));
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_ERROR, token->type);
    chess_pgn_tokenizer_destroy(tokenizer);
    chess_buffer_reader_cleanup(&reader);

    /* Unterminated comment */
    chess_buffer_reader_init(&reader, "h4 {A terrible");
    tokenizer = chess_pgn_tokenizer_new((ChessReader*)&reader);
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_SYMBOL, token->type);
    CU_ASSERT_STRING_EQUAL("h4", chess_string_data(&token->string));
    token = chess_pgn_tokenizer_next(tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_ERROR, token->type);
    chess_pgn_tokenizer_destroy(tokenizer);
    chess_buffer_reader_cleanup(&reader);
}

void test_pgn_tokenizer_add_tests(void)
{
    CU_Suite* suite = CU_add_suite("pgn-tokenizer", NULL, NULL);
    CU_add_test(suite, "empty", (CU_TestFunc)test_empty);
    CU_add_test(suite, "tag", (CU_TestFunc)test_tag);
    CU_add_test(suite, "escaped_string", (CU_TestFunc)test_escaped_string);
    CU_add_test(suite, "movetext", (CU_TestFunc)test_movetext);
    CU_add_test(suite, "black_movenum", (CU_TestFunc)test_black_movenum);
    CU_add_test(suite, "nag", (CU_TestFunc)test_nag);
    CU_add_test(suite, "comment", (CU_TestFunc)test_comment);
    CU_add_test(suite, "error", (CU_TestFunc)test_error);
}
