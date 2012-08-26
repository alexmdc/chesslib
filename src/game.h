#ifndef CHESSLIB_GAME_H_
#define CHESSLIB_GAME_H_

#include "position.h"
#include "move.h"
#include "variation.h"
#include "carray.h"

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
typedef struct ChessGameIterator ChessGameIterator;

ChessGameIterator* chess_game_get_iterator(ChessGame*);
void chess_game_iterator_destroy(ChessGameIterator*);

ChessGame* chess_game_iterator_game(const ChessGameIterator*);
const ChessPosition* chess_game_iterator_position(const ChessGameIterator*);
ChessVariation* chess_game_iterator_variation(ChessGameIterator*);
ChessMove chess_game_iterator_move(const ChessGameIterator*);
size_t chess_game_iterator_ply(const ChessGameIterator*);
ChessResult chess_game_iterator_check_result(const ChessGameIterator*);

void chess_game_iterator_append_move(ChessGameIterator*, ChessMove move);
void chess_game_iterator_truncate_moves(ChessGameIterator*);

void chess_game_iterator_step_forward(ChessGameIterator*);
void chess_game_iterator_step_back(ChessGameIterator*);
void chess_game_iterator_step_to_start(ChessGameIterator*);
void chess_game_iterator_step_to_end(ChessGameIterator*);
void chess_game_iterator_step_to_move(ChessGameIterator*, ChessVariation*);

#endif /* CHESSLIB_GAME_H_ */
