#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "reader.h"

typedef int(*ReadCharFunc)(ChessReader*);

typedef struct
{
    ReadCharFunc read_char;
} ReaderVtable;

void chess_reader_init(ChessReader* reader)
{
    reader->next = EOF;
}

int chess_reader_getc(ChessReader* reader)
{
    int next;
    if (reader->next != EOF)
    {
        next = reader->next;
        reader->next = EOF;
        return next;
    }

   return ((ReaderVtable*)reader->vtable)->read_char(reader);
}

void chess_reader_ungetc(ChessReader* reader, char c)
{
    reader->next = c;
}

static int file_reader_getc(ChessFileReader* reader)
{
    return fgetc(reader->file);
}

static ReaderVtable file_reader_vtable = {
    (ReadCharFunc)&file_reader_getc
};

void chess_file_reader_init(ChessFileReader* reader, FILE* file)
{
    chess_reader_init((ChessReader*)reader);
    reader->base.vtable = &file_reader_vtable;
    reader->file = file;
}

void chess_file_reader_cleanup(ChessFileReader* writer)
{
    /* No-op */
}

static int buffer_reader_getc(ChessBufferReader* reader)
{
    return (reader->index < reader->buffer_size)
        ? reader->buffer[reader->index++] : EOF;
}

static ReaderVtable buffer_reader_vtable = {
    (ReadCharFunc)&buffer_reader_getc
};

void chess_buffer_reader_init(ChessBufferReader* reader, const char* data, size_t size)
{
    chess_reader_init((ChessReader*)reader);
    reader->base.vtable = &buffer_reader_vtable;
    reader->buffer = malloc(size);
    memcpy(reader->buffer, data, size);
    reader->buffer_size = size;
    reader->index = 0;
}

void chess_buffer_reader_cleanup(ChessBufferReader* reader)
{
    free(reader->buffer);
}
