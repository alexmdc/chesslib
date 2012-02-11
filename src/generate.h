#ifndef CHESSLIB_GENERATE_H_
#define CHESSLIB_GENERATE_H_

void chess_generate_init();
int chess_generate_moves(const ChessPosition*, ChessMove*);
ChessBoolean chess_generate_is_square_attacked(const ChessPosition*, ChessSquare, ChessColor);

#endif /* CHESSLIB_GENERATE_H_ */
