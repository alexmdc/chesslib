#ifndef CHESSLIB_FEN_H_
#define CHESSLIB_FEN_H_

#include "position.h"

void chess_fen_load(const char* s, ChessPosition*);
void chess_fen_save(const ChessPosition*, char* s);

#endif /* CHESSLIB_FEN_H_ */
