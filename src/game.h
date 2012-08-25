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
const char* chess_game_round(const ChessGame*);
const char* chess_game_white(const ChessGame*);
const char* chess_game_black(const ChessGame*);
ChessResult chess_game_result(const ChessGame*);

void chess_game_set_event(ChessGame*, const char*);
void chess_game_set_site(ChessGame*, const char*);
void chess_game_set_date(ChessGame*, const char*);
void chess_game_set_round(ChessGame*, const char*);
void chess_game_set_white(ChessGame*, const char*);
void chess_game_set_black(ChessGame*, const char*);
void chess_game_set_result(ChessGame*, ChessResult);

void chess_game_set_tag(ChessGame*, const char* name, const char* value);
void chess_game_remove_tag(ChessGame*, const char* name);
const char* chess_game_tag_value(ChessGame*, const char* name);

/* PGN tag iterator */
typedef struct
{
    ChessGame* game;
    int index;
    void* extra;
} ChessGameTagIterator;

ChessGameTagIterator chess_game_get_tag_iterator(ChessGame*);
const char* chess_game_tag_iterator_name(const ChessGameTagIterator*);
const char* chess_game_tag_iterator_value(const ChessGameTagIterator*);
ChessBoolean chess_game_tag_iterator_next(ChessGameTagIterator*);

/* Iterator interface */
const ChessPosition* chess_game_current_position(const ChessGame*);
ChessVariation* chess_game_current_variation(const ChessGame*);
ChessMove chess_game_current_move(const ChessGame*);

void chess_game_append_move(ChessGame*, ChessMove move);
void chess_game_truncate_moves(ChessGame*);

void chess_game_step_forward(ChessGame*);
void chess_game_step_back(ChessGame*);
void chess_game_step_to_start(ChessGame*);
void chess_game_step_to_end(ChessGame*);
void chess_game_step_to_move(ChessGame*, ChessVariation*);

#endif /* CHESSLIB_GAME_H_ */
