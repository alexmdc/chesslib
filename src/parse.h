#ifndef CHESSLIB_PARSE_H_
#define CHESSLIB_PARSE_H_

#include "move.h"
#include "position.h"

typedef enum {
    CHESS_PARSE_OK = 0,
    CHESS_PARSE_ERROR,
    CHESS_PARSE_ILLEGAL_MOVE,
    CHESS_PARSE_AMBIGUOUS_MOVE
} ChessParseResult;

ChessParseResult chess_parse_move(const char* s, const ChessPosition*, ChessMove*);

#endif /* CHESSLIB_PARSE_H_ */
