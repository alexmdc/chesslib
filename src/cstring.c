#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "cstring.h"

static const char* empty_string = "";

void chess_string_init(ChessString* string)
{
    string->size = 0;
}

void chess_string_init_assign(ChessString* string, const char* s)
{
    assert(s);
    if (!*s)
    {
        string->size = 0;
        string->data = empty_string;
        return;
    }
    string->size = strlen(s);
    char* buf = (char*)malloc(string->size + 1);
    strcpy(buf, s);
    string->data = buf;
}

void chess_string_cleanup(ChessString* string)
{
    if (string->size > 0)
        free((char*)string->data);
}

size_t chess_string_size(const ChessString* string)
{
    return string->size;
}

const char* chess_string_data(const ChessString* string)
{
    return (string->size > 0) ? string->data : empty_string;
}

void chess_string_clear(ChessString* string)
{
    chess_string_cleanup(string);
    chess_string_init(string);
}

void chess_string_assign(ChessString* string, const char* s)
{
    chess_string_cleanup(string);
    chess_string_init_assign(string, s);
}
