#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "writer.h"
#include "calloc.h"

typedef void(*WriteCharFunc)(ChessWriter*, char);
typedef void(*WriteStringFunc)(ChessWriter*, const char*);
typedef void(*WriteStringSizeFunc)(ChessWriter*, const char*, size_t);

typedef struct
{
    WriteCharFunc write_char;
    WriteStringFunc write_string;
    WriteStringSizeFunc write_string_size;
} WriterVtable;

void chess_writer_write_char(ChessWriter* writer, char c)
{
    ((WriterVtable*)writer->vtable)->write_char(writer, c);
}

void chess_writer_write_string(ChessWriter* writer, const char* str)
{
    ((WriterVtable*)writer->vtable)->write_string(writer, str);
}

void chess_writer_write_string_size(ChessWriter* writer, const char* str, size_t size)
{
    ((WriterVtable*)writer->vtable)->write_string_size(writer, str, size);
}

static void file_writer_write_char(ChessFileWriter* writer, char c)
{
    fputc(c, writer->file);
}

static void file_writer_write_string(ChessFileWriter* writer, const char* str)
{
    fputs(str, writer->file);
}

static void file_writer_write_string_size(ChessFileWriter* writer, const char* str, size_t size)
{
    fwrite(str, 1, size, writer->file);
}

static WriterVtable file_writer_vtable = {
    (WriteCharFunc)&file_writer_write_char,
    (WriteStringFunc)&file_writer_write_string,
    (WriteStringSizeFunc)&file_writer_write_string_size
};

void chess_file_writer_init(ChessFileWriter* writer, FILE* file)
{
    writer->base.vtable = &file_writer_vtable;
    writer->file = file;
}

void chess_file_writer_cleanup(ChessFileWriter* writer)
{
    /* No-op */
}

static void expand_buffer(ChessBufferWriter* writer)
{
    if (writer->buffer_size == 0)
    {
        /* Pick an average size to start */
        writer->buffer_size = 32;
        writer->buffer = chess_alloc(writer->buffer_size);
    }
    else
    {
        /* Otherwise double the buffer size */
        writer->buffer_size *= 2;
        writer->buffer = chess_realloc(writer->buffer, writer->buffer_size);
    }
}

static void buffer_writer_write_char(ChessBufferWriter* writer, char c)
{
    if (writer->size + 1 >= writer->buffer_size)
        expand_buffer(writer);

    writer->buffer[writer->size++] = c;
}

static void buffer_writer_write_string_size(ChessBufferWriter* writer, const char* str, size_t size)
{
    while (writer->size + size >= writer->buffer_size)
        expand_buffer(writer);

    memcpy(writer->buffer + writer->size, str, size);
    writer->size += size;
}

static void buffer_writer_write_string(ChessBufferWriter* writer, const char* str)
{
    buffer_writer_write_string_size(writer, str, strlen(str));
}

static WriterVtable buffer_writer_vtable = {
    (WriteCharFunc)&buffer_writer_write_char,
    (WriteStringFunc)&buffer_writer_write_string,
    (WriteStringSizeFunc)&buffer_writer_write_string_size
};

void chess_buffer_writer_init(ChessBufferWriter* writer)
{
    writer->base.vtable = &buffer_writer_vtable;
    writer->buffer = NULL;
    writer->buffer_size = 0;
    writer->size = 0;
}

void chess_buffer_writer_cleanup(ChessBufferWriter* writer)
{
    if (writer->buffer_size > 0)
        chess_free(writer->buffer);
}

char* chess_buffer_writer_data(const ChessBufferWriter* writer)
{
    return writer->buffer;
}

size_t chess_buffer_writer_size(const ChessBufferWriter* writer)
{
    return writer->size;
}

void chess_buffer_writer_clear(ChessBufferWriter* writer)
{
    writer->size = 0;
}

char* chess_buffer_writer_detach_buffer(ChessBufferWriter* writer)
{
    char* buffer = writer->buffer;
    chess_buffer_writer_init(writer);
    return buffer;
}
