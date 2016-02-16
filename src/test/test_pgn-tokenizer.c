#include <CUnit/CUnit.h>

#include "../pgn-tokenizer.h"

#include "helpers.h"

#define ASSERT_TOKEN_EQUAL(token, type, line, col) \
    assert_token_equal(token, type, line, col, __FILE__, __LINE__)

void assert_token_equal(const ChessPgnToken* token, ChessPgnTokenType type,
                        unsigned int tline, unsigned int tcol, const char* file, unsigned int line)
{
    if (token->type != type)
    {
        ASSERT_FAIL("ASSERT_TOKEN_EQUAL(type)", file, line);
        return;
    }
    if (token->line != tline)
    {
        ASSERT_FAIL("ASSERT_TOKEN_EQUAL(line)", file, line);
        return;
    }
    if (token->col != tcol)
    {
        ASSERT_FAIL("ASSERT_TOKEN_EQUAL(col)", file, line);
        return;
    }
    ASSERT_PASS("ASSERT_TOKEN_EQUAL()", file, line);
}

static void test_empty(void)
{
    ChessBufferReader reader;
    ChessPgnTokenizer tokenizer;
    const ChessPgnToken* token;

    chess_buffer_reader_init(&reader, "");
    chess_pgn_tokenizer_init(&tokenizer, (ChessReader*)&reader);
    token = chess_pgn_tokenizer_peek(&tokenizer);
    ASSERT_TOKEN_EQUAL(token, CHESS_PGN_TOKEN_EOF, 1, 1);
    chess_pgn_tokenizer_cleanup(&tokenizer);
    chess_buffer_reader_cleanup(&reader);

    chess_buffer_reader_init(&reader, "  \n");
    chess_pgn_tokenizer_init(&tokenizer, (ChessReader*)&reader);
    token = chess_pgn_tokenizer_peek(&tokenizer);
    ASSERT_TOKEN_EQUAL(token, CHESS_PGN_TOKEN_EOF, 2, 1);
    chess_pgn_tokenizer_cleanup(&tokenizer);
    chess_buffer_reader_cleanup(&reader);
}

static void test_tag(void)
{
    const char tag[] = "[Event \"AUS Champs\"]\n";
    ChessBufferReader reader;
    ChessPgnTokenizer tokenizer;
    const ChessPgnToken* token;

    chess_buffer_reader_init(&reader, tag);
    chess_pgn_tokenizer_init(&tokenizer, (ChessReader*)&reader);
    token = chess_pgn_tokenizer_peek(&tokenizer);
    ASSERT_TOKEN_EQUAL(token, CHESS_PGN_TOKEN_L_BRACKET, 1, 1);
    token = chess_pgn_tokenizer_next(&tokenizer);
    ASSERT_TOKEN_EQUAL(token, CHESS_PGN_TOKEN_L_BRACKET, 1, 1);
    token = chess_pgn_tokenizer_next(&tokenizer);
    ASSERT_TOKEN_EQUAL(token, CHESS_PGN_TOKEN_SYMBOL, 1, 2);
    CU_ASSERT_STRING_EQUAL("Event", token->string.data);
    token = chess_pgn_tokenizer_next(&tokenizer);
    ASSERT_TOKEN_EQUAL(token, CHESS_PGN_TOKEN_STRING, 1, 8);
    CU_ASSERT_STRING_EQUAL("AUS Champs", token->string.data);
    token = chess_pgn_tokenizer_next(&tokenizer);
    ASSERT_TOKEN_EQUAL(token, CHESS_PGN_TOKEN_R_BRACKET, 1, 20);
    token = chess_pgn_tokenizer_next(&tokenizer);
    ASSERT_TOKEN_EQUAL(token, CHESS_PGN_TOKEN_EOF, 2, 1);
    token = chess_pgn_tokenizer_peek(&tokenizer);
    ASSERT_TOKEN_EQUAL(token, CHESS_PGN_TOKEN_EOF, 2, 1);

    chess_pgn_tokenizer_cleanup(&tokenizer);
    chess_buffer_reader_cleanup(&reader);
}

static void test_escaped_string(void)
{
    const char text[] = "\"One \\\"fine\\\" move\"";
    ChessBufferReader reader;
    ChessPgnTokenizer tokenizer;
    const ChessPgnToken* token;

    chess_buffer_reader_init(&reader, text);
    chess_pgn_tokenizer_init(&tokenizer, (ChessReader*)&reader);
    token = chess_pgn_tokenizer_peek(&tokenizer);
    ASSERT_TOKEN_EQUAL(token, CHESS_PGN_TOKEN_STRING, 1, 1);
    token = chess_pgn_tokenizer_next(&tokenizer);
    ASSERT_TOKEN_EQUAL(token, CHESS_PGN_TOKEN_STRING, 1, 1);
    CU_ASSERT_STRING_EQUAL("One \"fine\" move", token->string.data);
    token = chess_pgn_tokenizer_next(&tokenizer);
    ASSERT_TOKEN_EQUAL(token, CHESS_PGN_TOKEN_EOF, 1, 20);

    chess_pgn_tokenizer_cleanup(&tokenizer);
    chess_buffer_reader_cleanup(&reader);
}

static void test_movetext(void)
{
    const char text[] = "1. e4 e5\n2. f4\n1-0";
    ChessBufferReader reader;
    ChessPgnTokenizer tokenizer;
    const ChessPgnToken* token;

    chess_buffer_reader_init(&reader, text);
    chess_pgn_tokenizer_init(&tokenizer, (ChessReader*)&reader);

    token = chess_pgn_tokenizer_next(&tokenizer);
    ASSERT_TOKEN_EQUAL(token, CHESS_PGN_TOKEN_NUMBER, 1, 1);
    CU_ASSERT_EQUAL(1, token->number);
    token = chess_pgn_tokenizer_next(&tokenizer);
    ASSERT_TOKEN_EQUAL(token, CHESS_PGN_TOKEN_PERIOD, 1, 2);
    token = chess_pgn_tokenizer_next(&tokenizer);
    ASSERT_TOKEN_EQUAL(token, CHESS_PGN_TOKEN_SYMBOL, 1, 4);
    CU_ASSERT_STRING_EQUAL("e4", token->string.data);
    token = chess_pgn_tokenizer_next(&tokenizer);
    ASSERT_TOKEN_EQUAL(token, CHESS_PGN_TOKEN_SYMBOL, 1, 7);
    CU_ASSERT_STRING_EQUAL("e5", token->string.data);

    token = chess_pgn_tokenizer_next(&tokenizer);
    ASSERT_TOKEN_EQUAL(token, CHESS_PGN_TOKEN_NUMBER, 2, 1);
    CU_ASSERT_EQUAL(2, token->number);
    token = chess_pgn_tokenizer_next(&tokenizer);
    ASSERT_TOKEN_EQUAL(token, CHESS_PGN_TOKEN_PERIOD, 2, 2);
    token = chess_pgn_tokenizer_next(&tokenizer);
    ASSERT_TOKEN_EQUAL(token, CHESS_PGN_TOKEN_SYMBOL, 2, 4);
    CU_ASSERT_STRING_EQUAL("f4", token->string.data);
    token = chess_pgn_tokenizer_next(&tokenizer);
    ASSERT_TOKEN_EQUAL(token, CHESS_PGN_TOKEN_ONE_ZERO, 3, 1);
    token = chess_pgn_tokenizer_next(&tokenizer);
    ASSERT_TOKEN_EQUAL(token, CHESS_PGN_TOKEN_EOF, 3, 4);

    chess_pgn_tokenizer_cleanup(&tokenizer);
    chess_buffer_reader_cleanup(&reader);
}

static void test_black_movenum(void)
{
    const char text[] = "13... O-O 14. Rd1";
    ChessBufferReader reader;
    ChessPgnTokenizer tokenizer;
    const ChessPgnToken* token;

    chess_buffer_reader_init(&reader, text);
    chess_pgn_tokenizer_init(&tokenizer, (ChessReader*)&reader);

    token = chess_pgn_tokenizer_next(&tokenizer);
    ASSERT_TOKEN_EQUAL(token, CHESS_PGN_TOKEN_NUMBER, 1, 1);
    CU_ASSERT_EQUAL(13, token->number);
    token = chess_pgn_tokenizer_next(&tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_PERIOD, token->type);
    ASSERT_TOKEN_EQUAL(token, CHESS_PGN_TOKEN_PERIOD, 1, 3);
    token = chess_pgn_tokenizer_next(&tokenizer);
    ASSERT_TOKEN_EQUAL(token, CHESS_PGN_TOKEN_PERIOD, 1, 4);
    token = chess_pgn_tokenizer_next(&tokenizer);
    ASSERT_TOKEN_EQUAL(token, CHESS_PGN_TOKEN_PERIOD, 1, 5);
    token = chess_pgn_tokenizer_next(&tokenizer);
    ASSERT_TOKEN_EQUAL(token, CHESS_PGN_TOKEN_SYMBOL, 1, 7);
    CU_ASSERT_STRING_EQUAL("O-O", token->string.data);

    token = chess_pgn_tokenizer_next(&tokenizer);
    ASSERT_TOKEN_EQUAL(token, CHESS_PGN_TOKEN_NUMBER, 1, 11);
    CU_ASSERT_EQUAL(14, token->number);
    token = chess_pgn_tokenizer_next(&tokenizer);
    ASSERT_TOKEN_EQUAL(token, CHESS_PGN_TOKEN_PERIOD, 1, 13);
    token = chess_pgn_tokenizer_next(&tokenizer);
    ASSERT_TOKEN_EQUAL(token, CHESS_PGN_TOKEN_SYMBOL, 1, 15);
    CU_ASSERT_STRING_EQUAL("Rd1", token->string.data);
    token = chess_pgn_tokenizer_next(&tokenizer);
    ASSERT_TOKEN_EQUAL(token, CHESS_PGN_TOKEN_EOF, 1, 18);

    chess_pgn_tokenizer_cleanup(&tokenizer);
    chess_buffer_reader_cleanup(&reader);
}

static void test_nag(void)
{
    const char text[] = "7. Bg5 $2 h6 $3 $17";
    ChessBufferReader reader;
    ChessPgnTokenizer tokenizer;
    const ChessPgnToken* token;

    chess_buffer_reader_init(&reader, text);
    chess_pgn_tokenizer_init(&tokenizer, (ChessReader*)&reader);

    token = chess_pgn_tokenizer_next(&tokenizer);
    CU_ASSERT_EQUAL(CHESS_PGN_TOKEN_NUMBER, token->type);
    ASSERT_TOKEN_EQUAL(token, CHESS_PGN_TOKEN_NUMBER, 1, 1);
    CU_ASSERT_EQUAL(7, token->number);
    token = chess_pgn_tokenizer_next(&tokenizer);
    ASSERT_TOKEN_EQUAL(token, CHESS_PGN_TOKEN_PERIOD, 1, 2);
    token = chess_pgn_tokenizer_next(&tokenizer);
    ASSERT_TOKEN_EQUAL(token, CHESS_PGN_TOKEN_SYMBOL, 1, 4);
    CU_ASSERT_STRING_EQUAL("Bg5", token->string.data);
    token = chess_pgn_tokenizer_next(&tokenizer);
    ASSERT_TOKEN_EQUAL(token, CHESS_PGN_TOKEN_NAG, 1, 8);
    CU_ASSERT_EQUAL(2, token->number);

    token = chess_pgn_tokenizer_next(&tokenizer);
    ASSERT_TOKEN_EQUAL(token, CHESS_PGN_TOKEN_SYMBOL, 1, 11);
    CU_ASSERT_STRING_EQUAL("h6", token->string.data);
    token = chess_pgn_tokenizer_next(&tokenizer);
    ASSERT_TOKEN_EQUAL(token, CHESS_PGN_TOKEN_NAG, 1, 14);
    CU_ASSERT_EQUAL(3, token->number);
    token = chess_pgn_tokenizer_next(&tokenizer);
    ASSERT_TOKEN_EQUAL(token, CHESS_PGN_TOKEN_NAG, 1, 17);
    CU_ASSERT_EQUAL(17, token->number);
    token = chess_pgn_tokenizer_next(&tokenizer);
    ASSERT_TOKEN_EQUAL(token, CHESS_PGN_TOKEN_EOF, 1, 20);

    chess_pgn_tokenizer_cleanup(&tokenizer);
    chess_buffer_reader_cleanup(&reader);
}

static void test_comment(void)
{
    const char text[] = "1. e4 {A strong move} e6 {The French Defence}";
    ChessBufferReader reader;
    ChessPgnTokenizer tokenizer;
    const ChessPgnToken* token;

    chess_buffer_reader_init(&reader, text);
    chess_pgn_tokenizer_init(&tokenizer, (ChessReader*)&reader);

    token = chess_pgn_tokenizer_next(&tokenizer);
    ASSERT_TOKEN_EQUAL(token, CHESS_PGN_TOKEN_NUMBER, 1, 1);
    CU_ASSERT_EQUAL(1, token->number);
    token = chess_pgn_tokenizer_next(&tokenizer);
    ASSERT_TOKEN_EQUAL(token, CHESS_PGN_TOKEN_PERIOD, 1, 2);
    token = chess_pgn_tokenizer_next(&tokenizer);
    ASSERT_TOKEN_EQUAL(token, CHESS_PGN_TOKEN_SYMBOL, 1, 4);
    CU_ASSERT_STRING_EQUAL("e4", token->string.data);
    token = chess_pgn_tokenizer_next(&tokenizer);
    ASSERT_TOKEN_EQUAL(token, CHESS_PGN_TOKEN_COMMENT, 1, 7);
    CU_ASSERT_STRING_EQUAL("A strong move", token->string.data);

    token = chess_pgn_tokenizer_next(&tokenizer);
    ASSERT_TOKEN_EQUAL(token, CHESS_PGN_TOKEN_SYMBOL, 1, 23);
    CU_ASSERT_STRING_EQUAL("e6", token->string.data);
    token = chess_pgn_tokenizer_next(&tokenizer);
    ASSERT_TOKEN_EQUAL(token, CHESS_PGN_TOKEN_COMMENT, 1, 26);
    CU_ASSERT_STRING_EQUAL("The French Defence", token->string.data);
    token = chess_pgn_tokenizer_next(&tokenizer);
    ASSERT_TOKEN_EQUAL(token, CHESS_PGN_TOKEN_EOF, 1, 46);

    chess_pgn_tokenizer_cleanup(&tokenizer);
    chess_buffer_reader_cleanup(&reader);
}

static void test_error(void)
{
    ChessBufferReader reader;
    ChessPgnTokenizer tokenizer;
    const ChessPgnToken* token;

    /* Malformed symbol */
    chess_buffer_reader_init(&reader, "Qxh7 #");
    chess_pgn_tokenizer_init(&tokenizer, (ChessReader*)&reader);
    token = chess_pgn_tokenizer_next(&tokenizer);
    ASSERT_TOKEN_EQUAL(token, CHESS_PGN_TOKEN_SYMBOL, 1, 1);
    CU_ASSERT_STRING_EQUAL("Qxh7", token->string.data);
    token = chess_pgn_tokenizer_next(&tokenizer);
    ASSERT_TOKEN_EQUAL(token, CHESS_PGN_TOKEN_ERROR, 1, 6);
    chess_pgn_tokenizer_cleanup(&tokenizer);
    chess_buffer_reader_cleanup(&reader);

    /* Unterminated string */
    chess_buffer_reader_init(&reader, "[White \"Capa");
    chess_pgn_tokenizer_init(&tokenizer, (ChessReader*)&reader);
    token = chess_pgn_tokenizer_next(&tokenizer);
    ASSERT_TOKEN_EQUAL(token, CHESS_PGN_TOKEN_L_BRACKET, 1, 1);
    token = chess_pgn_tokenizer_next(&tokenizer);
    ASSERT_TOKEN_EQUAL(token, CHESS_PGN_TOKEN_SYMBOL, 1, 2);
    CU_ASSERT_STRING_EQUAL("White", token->string.data);
    token = chess_pgn_tokenizer_next(&tokenizer);
    ASSERT_TOKEN_EQUAL(token, CHESS_PGN_TOKEN_ERROR, 1, 8);
    chess_pgn_tokenizer_cleanup(&tokenizer);
    chess_buffer_reader_cleanup(&reader);

    /* Unterminated comment */
    chess_buffer_reader_init(&reader, "h4 {A terrible");
    chess_pgn_tokenizer_init(&tokenizer, (ChessReader*)&reader);
    token = chess_pgn_tokenizer_next(&tokenizer);
    ASSERT_TOKEN_EQUAL(token, CHESS_PGN_TOKEN_SYMBOL, 1, 1);
    CU_ASSERT_STRING_EQUAL("h4", token->string.data);
    token = chess_pgn_tokenizer_next(&tokenizer);
    ASSERT_TOKEN_EQUAL(token, CHESS_PGN_TOKEN_ERROR, 1, 4);
    chess_pgn_tokenizer_cleanup(&tokenizer);
    chess_buffer_reader_cleanup(&reader);
}

void test_pgn_tokenizer_add_tests(void)
{
    CU_Suite* suite = add_suite("pgn-tokenizer");
    CU_add_test(suite, "empty", (CU_TestFunc)test_empty);
    CU_add_test(suite, "tag", (CU_TestFunc)test_tag);
    CU_add_test(suite, "escaped_string", (CU_TestFunc)test_escaped_string);
    CU_add_test(suite, "movetext", (CU_TestFunc)test_movetext);
    CU_add_test(suite, "black_movenum", (CU_TestFunc)test_black_movenum);
    CU_add_test(suite, "nag", (CU_TestFunc)test_nag);
    CU_add_test(suite, "comment", (CU_TestFunc)test_comment);
    CU_add_test(suite, "error", (CU_TestFunc)test_error);
}
