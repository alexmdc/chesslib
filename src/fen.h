#ifndef CHESSLIB_FEN_H_
#define CHESSLIB_FEN_H_

#include "chess.h"
#include "position.h"

/* Longest possible FEN string is about 102 characters */
#define CHESS_FEN_MAX_LENGTH 128

ChessBoolean chess_fen_load(const char* s, ChessPosition*);
void chess_fen_save(const ChessPosition*, char* s);

#endif /* CHESSLIB_FEN_H_ */
