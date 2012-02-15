#ifndef CHESSLIB_GENERATE_H_
#define CHESSLIB_GENERATE_H_

struct ChessArray;

void chess_generate_init();
void chess_generate_moves(const ChessPosition*, struct ChessArray*);
ChessBoolean chess_generate_is_square_attacked(const ChessPosition*, ChessSquare, ChessColor);

#endif /* CHESSLIB_GENERATE_H_ */
