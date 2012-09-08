#include <assert.h>
#include <stdlib.h>
#include <memory.h>

#include "cbuffer.h"
#include "calloc.h"

void chess_buffer_init(ChessBuffer* buffer)
{
    buffer->size = 0;
    buffer->max_size = 0;
}

void chess_buffer_cleanup(ChessBuffer* buffer)
{
    if (buffer->max_size > 0)
        chess_free(buffer->data);
}

size_t chess_buffer_size(const ChessBuffer* buffer)
{
    return buffer->size;
}

char* chess_buffer_data(ChessBuffer* buffer)
{
    assert(buffer->max_size > 0);
    return buffer->data;
}

static void expand(ChessBuffer* buffer, size_t min_size)
{
    size_t size = buffer->max_size ? buffer->max_size : 8;
    while (size < min_size)
        size *= 2;

    if (buffer->max_size)
        buffer->data = chess_realloc(buffer->data, size);
    else
        buffer->data = chess_alloc(size);
    buffer->max_size = size;
}

void chess_buffer_set_size(ChessBuffer* buffer, size_t size)
{
    if (size > buffer->max_size)
        expand(buffer, size);

    buffer->size = size;
}

void chess_buffer_append_char(ChessBuffer* buffer, char c)
{
    if (buffer->size == buffer->max_size)
        expand(buffer, buffer->size + 1);

    assert(buffer->max_size > buffer->size);
    buffer->data[buffer->size++] = c;
}

void chess_buffer_append_string(ChessBuffer* buffer, const char* s)
{
    chess_buffer_append_string_size(buffer, s, strlen(s));
}

void chess_buffer_append_string_size(ChessBuffer* buffer, const char* s, size_t size)
{
    if (buffer->size + size > buffer->max_size)
        expand(buffer, buffer->size + size);

    strncpy(buffer->data + buffer->size, s, size);
    buffer->size += size;
}

void chess_buffer_clear(ChessBuffer* buffer)
{
    buffer->size = 0;
}

void chess_buffer_null_terminate(ChessBuffer* buffer)
{
    chess_buffer_append_char(buffer, '\0');
    buffer->size--;
}
