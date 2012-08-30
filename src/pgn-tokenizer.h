#ifndef CHESSLIB_PGN_TOKENIZER_H_
#define CHESSLIB_PGN_TOKENIZER_H_

#include "cbuffer.h"
#include "cstring.h"
#include "reader.h"

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

typedef struct
{
    unsigned int line, row;
    ChessPgnTokenType type;
    ChessString string;
    int number;
} ChessPgnToken;

typedef struct
{
    ChessReader* reader;
    unsigned int line, row;
    ChessPgnToken* next;
    ChessPgnToken tokens[2];
    int count;
    ChessBuffer buffer;
} ChessPgnTokenizer;

ChessPgnTokenizer* chess_pgn_tokenizer_new(ChessReader*);
void chess_pgn_tokenizer_destroy(ChessPgnTokenizer*);

const ChessPgnToken* chess_pgn_tokenizer_peek(ChessPgnTokenizer*);
void chess_pgn_tokenizer_consume(ChessPgnTokenizer*);

const ChessPgnToken* chess_pgn_tokenizer_next(ChessPgnTokenizer*);

#endif /* CHESSLIB_PGN_TOKENIZER_H_ */
