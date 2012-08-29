#ifndef CHESSLIB_BUFFER_H_
#define CHESSLIB_BUFFER_H_

typedef struct
{
    size_t size;
    size_t max_size;
    char* data;
} ChessBuffer;

void chess_buffer_init(ChessBuffer*);
void chess_buffer_cleanup(ChessBuffer*);

size_t chess_buffer_size(const ChessBuffer*);
char* chess_buffer_data(ChessBuffer*);

void chess_buffer_set_size(ChessBuffer*, size_t size);
void chess_buffer_append_char(ChessBuffer*, char c);
void chess_buffer_append_string(ChessBuffer*, const char* s);
void chess_buffer_append_string_size(ChessBuffer*, const char* s, size_t size);

void chess_buffer_clear(ChessBuffer*);
void chess_buffer_null_terminate(ChessBuffer*);

#endif /* CHESSLIB_BUFFER_H_ */
