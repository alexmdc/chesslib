#ifndef CHESSLIB_PGN_H_
#define CHESSLIB_PGN_H_

typedef enum {
    CHESS_PGN_LOAD_OK = 0,
    CHESS_PGN_LOAD_BAD_TOKEN,
    CHESS_PGN_LOAD_UNEXPECTED_TOKEN,
    CHESS_PGN_LOAD_ILLEGAL_MOVE,
} ChessPgnLoadResult;

ChessPgnLoadResult chess_pgn_load(const char*, ChessGame*);
void chess_pgn_save(const ChessGame*, char*);

#endif /* CHESSLIB_PGN_H_ */
