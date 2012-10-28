#ifndef CHESSLIB_GENERATE_H_
#define CHESSLIB_GENERATE_H_

#include "chess.h"
#include "position.h"
#include "carray.h"

typedef struct
{
    const ChessPosition* position;
    ChessSquare sq;
    ChessSquare to;
    int d;
    ChessMovePromote promote;
    ChessCastleState castle;
} ChessMoveGenerator;

void chess_generate_init(void);

void chess_move_generator_init(ChessMoveGenerator*, const ChessPosition*);
ChessMove chess_move_generator_next(ChessMoveGenerator*);

void chess_generate_moves(const ChessPosition*, ChessArray*);
ChessBoolean chess_generate_is_square_attacked(const ChessPosition*, ChessSquare, ChessColor);

#endif /* CHESSLIB_GENERATE_H_ */
