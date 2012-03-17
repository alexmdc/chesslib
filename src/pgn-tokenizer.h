#ifndef CHESSLIB_PGN_TOKENIZER_H_
#define CHESSLIB_PGN_TOKENIZER_H_

#include "cstring.h"

typedef enum
{
    CHESS_PGN_TOKEN_NONE = 0,
    CHESS_PGN_TOKEN_ERROR,
    CHESS_PGN_TOKEN_L_PARENTHESIS,
    CHESS_PGN_TOKEN_R_PARENTHESIS,
    CHESS_PGN_TOKEN_L_BRACKET,
    CHESS_PGN_TOKEN_R_BRACKET,
    CHESS_PGN_TOKEN_PERIOD,
    CHESS_PGN_TOKEN_SYMBOL,
    CHESS_PGN_TOKEN_STRING,
    CHESS_PGN_TOKEN_COMMENT,
    CHESS_PGN_TOKEN_NUMBER,
    CHESS_PGN_TOKEN_NAG,
    CHESS_PGN_TOKEN_ASTERISK,
    CHESS_PGN_TOKEN_ONE_ZERO,
    CHESS_PGN_TOKEN_ZERO_ONE,
    CHESS_PGN_TOKEN_HALF_HALF,
    CHESS_PGN_TOKEN_EOF
} ChessPgnTokenType;

typedef union
{
    ChessString string;
    int number;
} ChessPgnTokenData;

typedef struct
{
    unsigned int line, row;
    ChessPgnTokenType type;
    ChessPgnTokenData data;
} ChessPgnToken;

typedef struct
{
    const char* text;
    size_t index;
    unsigned int line, row;
    ChessPgnToken* last, *next;
    ChessPgnToken tokens[2];
} ChessPgnTokenizer;

ChessPgnTokenizer* chess_pgn_tokenizer_new(const char* text);
const ChessPgnToken* chess_pgn_tokenizer_peek(const ChessPgnTokenizer*);
const ChessPgnToken* chess_pgn_tokenizer_next(ChessPgnTokenizer*);
void chess_pgn_tokenizer_destroy(ChessPgnTokenizer*);

#endif /* CHESSLIB_PGN_TOKENIZER_H_ */
