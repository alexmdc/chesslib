#ifndef CHESSLIB_PRINT_H_
#define CHESSLIB_PRINT_H_

#include "move.h"
#include "position.h"
#include "game.h"
#include "writer.h"

int chess_print_move(ChessMove, char* s);
int chess_print_move_san(ChessMove, const ChessPosition*, char* s);
int chess_print_position(const ChessPosition*, char* s);
void chess_print_game_moves(const ChessGame*, ChessWriter* writer);
int chess_print_result(ChessResult, char* s);

#endif /* CHESSLIB_PRINT_H_ */
