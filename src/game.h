#ifndef CHESSLIB_GAME_H_
#define CHESSLIB_GAME_H_

typedef struct ChessGame ChessGame;

ChessGame* chess_game_new();
void chess_game_destroy(ChessGame*);

const ChessPosition* chess_game_position(const ChessGame*);
const ChessPosition* chess_game_initial_position(const ChessGame*);
size_t chess_game_ply(const ChessGame*);
ChessMove chess_game_move(const ChessGame*, size_t ply);
ChessResult chess_game_result(const ChessGame*);
const char* chess_game_result_text(const ChessGame*);

void chess_game_make_move(ChessGame*, ChessMove move);
void chess_game_undo_move(ChessGame*);

void chess_game_set_result(ChessGame*, ChessResult);

void chess_game_reset(ChessGame*);
void chess_game_reset_position(ChessGame*, const ChessPosition*);

#endif /* CHESSLIB_GAME_H_ */