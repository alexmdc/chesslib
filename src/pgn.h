#ifndef CHESSLIB_PGN_H_
#define CHESSLIB_PGN_H_

void chess_pgn_load(const char*, ChessGame*);
void chess_pgn_save(const ChessGame*, char*);

#endif /* CHESSLIB_PGN_H_ */
