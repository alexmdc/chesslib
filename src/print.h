#ifndef CHESSLIB_PRINT_H_
#define CHESSLIB_PRINT_H_

int chess_print_move(ChessMove, char* s);
int chess_print_move_san(ChessMove, const ChessPosition*, char* s);
int chess_print_position(const ChessPosition*, char* s);
int chess_print_game_moves(const ChessGame*, char* s);
int chess_print_result(ChessResult, char* s);

#endif /* CHESSLIB_PRINT_H_ */
