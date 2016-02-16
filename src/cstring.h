#ifndef CHESSLIB_STRING_H_
#define CHESSLIB_STRING_H_

typedef struct
{
    size_t size;
    const char* data;
} ChessString;

void chess_string_init(ChessString*);
void chess_string_init_assign(ChessString*, const char* s);
void chess_string_init_assign_size(ChessString*, const char* s, size_t n);
void chess_string_cleanup(ChessString*);

void chess_string_clear(ChessString*);
void chess_string_assign(ChessString*, const char* s);
void chess_string_assign_size(ChessString*, const char* s, size_t n);

#endif /* CHESSLIB_STRING_H_ */
