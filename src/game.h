#ifndef CHESSLIB_GAME_H_
#define CHESSLIB_GAME_H_

#include "position.h"
#include "move.h"
#include "variation.h"

typedef struct ChessGame ChessGame;

ChessGame* chess_game_new();
void chess_game_destroy(ChessGame*);

const ChessPosition* chess_game_position(const ChessGame*);
const ChessPosition* chess_game_initial_position(const ChessGame*);
ChessVariation* chess_game_variation(const ChessGame*);
size_t chess_game_ply(const ChessGame*);
ChessMove chess_game_move(const ChessGame*, size_t ply);

const char* chess_game_event(const ChessGame*);
const char* chess_game_site(const ChessGame*);
const char* chess_game_date(const ChessGame*);
unsigned int chess_game_round(const ChessGame*);
const char* chess_game_white(const ChessGame*);
const char* chess_game_black(const ChessGame*);
ChessResult chess_game_result(const ChessGame*);
const char* chess_game_result_text(const ChessGame*);

void chess_game_set_event(ChessGame*, const char*);
void chess_game_set_site(ChessGame*, const char*);
void chess_game_set_date(ChessGame*, const char*);
void chess_game_set_round(ChessGame*, unsigned int);
void chess_game_set_white(ChessGame*, const char*);
void chess_game_set_black(ChessGame*, const char*);
void chess_game_set_result(ChessGame*, ChessResult);

void chess_game_make_move(ChessGame*, ChessMove move);
void chess_game_undo_move(ChessGame*);

void chess_game_reset(ChessGame*);
void chess_game_reset_position(ChessGame*, const ChessPosition*);
void chess_game_set_variation(ChessGame*, ChessVariation*);

#endif /* CHESSLIB_GAME_H_ */