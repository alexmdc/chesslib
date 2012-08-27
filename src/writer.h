#ifndef CHESSLIB_WRITER_H_
#define CHESSLIB_WRITER_H_

typedef struct
{
    void* vtable;
} ChessWriter;

void chess_writer_write_char(ChessWriter*, char);
void chess_writer_write_string(ChessWriter*, const char*);
void chess_writer_write_string_size(ChessWriter*, const char*, size_t);

typedef struct
{
    ChessWriter base;
    FILE* file;
} ChessFileWriter;

void chess_file_writer_init(ChessFileWriter*, FILE* file);
void chess_file_writer_cleanup(ChessFileWriter*);

typedef struct
{
    ChessWriter base;
    char* buffer;
    size_t buffer_size;
    size_t size;
} ChessBufferWriter;

void chess_buffer_writer_init(ChessBufferWriter*);
void chess_buffer_writer_cleanup(ChessBufferWriter*);

char* chess_buffer_writer_data(const ChessBufferWriter*);
size_t chess_buffer_writer_size(const ChessBufferWriter*);

void chess_buffer_writer_clear(ChessBufferWriter*);
char* chess_buffer_writer_detach_buffer(ChessBufferWriter*);

#endif /* CHESSLIB_WRITER_H_ */
