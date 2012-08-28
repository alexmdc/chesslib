#ifndef CHESSLIB_READER_H_
#define CHESSLIB_READER_H_

typedef struct
{
    void* vtable;
    int next;
} ChessReader;

int chess_reader_getc(ChessReader*);
int chess_reader_peek(ChessReader*);
void chess_reader_ungetc(ChessReader*, char);

typedef struct
{
    ChessReader base;
    FILE* file;
} ChessFileReader;

void chess_file_reader_init(ChessFileReader*, FILE* file);
void chess_file_reader_cleanup(ChessFileReader*);

typedef struct
{
    ChessReader base;
    char* buffer;
    size_t buffer_size;
    size_t index;
} ChessBufferReader;

void chess_buffer_reader_init(ChessBufferReader*, const char* str);
void chess_buffer_reader_init_size(ChessBufferReader*, const char* data, size_t size);
void chess_buffer_reader_cleanup(ChessBufferReader*);

#endif /* CHESSLIB_READER_H_ */
