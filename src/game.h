#ifndef CHESSLIB_GAME_H_
#define CHESSLIB_GAME_H_

#include "position.h"
#include "move.h"
#include "variation.h"

typedef struct ChessGame ChessGame;

ChessGame* chess_game_new(void);
void chess_game_destroy(ChessGame*);

void chess_game_init(ChessGame*);
void chess_game_init_position(ChessGame*, const ChessPosition*);
void chess_game_set_root_variation(ChessGame*, ChessVariation*);

const ChessPosition* chess_game_initial_position(const ChessGame*);
ChessVariation* chess_game_root_variation(const ChessGame*);
size_t chess_game_ply(const ChessGame*);
ChessMove chess_game_move_at_ply(const ChessGame*, size_t ply);

/* PGN tags */
const char* chess_game_event(const ChessGame*);
const char* chess_game_site(const ChessGame*);
const char* chess_game_date(const ChessGame*);
unsigned int chess_game_round(const ChessGame*);
const char* chess_game_white(const ChessGame*);
const char* chess_game_black(const ChessGame*);
ChessResult chess_game_result(const ChessGame*);

void chess_game_set_event(ChessGame*, const char*);
void chess_game_set_site(ChessGame*, const char*);
void chess_game_set_date(ChessGame*, const char*);
void chess_game_set_round(ChessGame*, unsigned int);
void chess_game_set_white(ChessGame*, const char*);
void chess_game_set_black(ChessGame*, const char*);
void chess_game_set_result(ChessGame*, ChessResult);

/* Iterator interface */
const ChessPosition* chess_game_current_position(const ChessGame*);
ChessVariation* chess_game_current_variation(const ChessGame*);
ChessMove chess_game_current_move(const ChessGame*);

void chess_game_append_move(ChessGame*, ChessMove move);
void chess_game_truncate_moves(ChessGame*);

void chess_game_step_forward(ChessGame*);
void chess_game_step_back(ChessGame*);
void chess_game_reset_to_start(ChessGame*);

#endif /* CHESSLIB_GAME_H_ */