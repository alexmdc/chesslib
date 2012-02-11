#ifndef CHESSLIB_STRING_H_
#define CHESSLIB_STRING_H_

typedef struct
{
    size_t size;
    const char* data;
} ChessString;

void chess_string_init(ChessString*);
void chess_string_init_assign(ChessString*, const char* s);
void chess_string_cleanup(ChessString*);

size_t chess_string_size(const ChessString*);
const char* chess_string_data(const ChessString*);

void chess_string_clear(ChessString*);
void chess_string_assign(ChessString*, const char* s);

#endif /* CHESSLIB_STRING_H_ */
