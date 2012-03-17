#ifndef CHESSLIB_POSITION_H_
#define CHESSLIB_POSITION_H_

#include "chess.h"
#include "move.h"
#include "unmove.h"

typedef struct
{
    ChessPiece piece[64];
    ChessColor to_move;
    ChessCastleState castle;
    ChessFile ep;
    int fifty;
    int move_num;
    ChessSquare wking, bking;
} ChessPosition;

ChessPosition* chess_position_new(void);
ChessPosition* chess_position_clone(const ChessPosition*);
void chess_position_destroy(ChessPosition*);

void chess_position_init(ChessPosition*);
void chess_position_copy(const ChessPosition* from, ChessPosition* to);

ChessPiece chess_position_piece(const ChessPosition*, ChessSquare);
ChessColor chess_position_to_move(const ChessPosition*);
ChessCastleState chess_position_castle(const ChessPosition*);
ChessFile chess_position_ep(const ChessPosition*);
int chess_position_fifty(const ChessPosition*);
int chess_position_move_num(const ChessPosition*);

void chess_position_set_piece(ChessPosition*, ChessSquare, ChessPiece);
void chess_position_set_to_move(ChessPosition*, ChessColor);
void chess_position_set_castle(ChessPosition*, ChessCastleState);
void chess_position_set_ep(ChessPosition*, ChessFile);
void chess_position_set_fifty(ChessPosition*, int);
void chess_position_set_move_num(ChessPosition*, int);

ChessBoolean chess_position_is_check(const ChessPosition*);
ChessBoolean chess_position_move_is_legal(const ChessPosition*, ChessMove);
ChessBoolean chess_position_move_is_capture(const ChessPosition*, ChessMove);
ChessResult chess_position_check_result(const ChessPosition*);

ChessUnmove chess_position_make_move(ChessPosition*, ChessMove);
void chess_position_undo_move(ChessPosition*, ChessUnmove);

#endif /* CHESSLIB_POSITION_H_ */
