#ifndef CHESSLIB_PGN_H_
#define CHESSLIB_PGN_H_

#include "game.h"
#include "reader.h"
#include "writer.h"
#include "pgn-tokenizer.h"

typedef enum {
    CHESS_PGN_LOAD_OK = 0,
    CHESS_PGN_LOAD_BAD_TOKEN,
    CHESS_PGN_LOAD_UNEXPECTED_TOKEN,
    CHESS_PGN_LOAD_ILLEGAL_MOVE,
    CHESS_PGN_LOAD_EOF
} ChessPgnLoadResult;

ChessPgnLoadResult chess_pgn_load(ChessReader*, ChessGame*);
void chess_pgn_save(const ChessGame*, ChessWriter*);

typedef struct
{
    ChessReader* reader;
    ChessPgnTokenizer* tokenizer;
} ChessPgnLoader;

void chess_pgn_loader_init(ChessPgnLoader*, ChessReader*);
void chess_pgn_loader_cleanup(ChessPgnLoader*);

ChessPgnLoadResult chess_pgn_loader_next(ChessPgnLoader*, ChessGame*);
const ChessPgnToken* chess_pgn_loader_last_token(ChessPgnLoader*);

#endif /* CHESSLIB_PGN_H_ */
