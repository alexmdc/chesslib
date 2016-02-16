#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "cstring.h"
#include "calloc.h"

static const char* const EMPTY_STRING = "";

void chess_string_init(ChessString* string)
{
    string->size = 0;
    string->data = EMPTY_STRING;
}

void chess_string_init_assign(ChessString* string, const char* s)
{
    chess_string_init_assign_size(string, s, strlen(s));
}

void chess_string_init_assign_size(ChessString* string, const char* s, size_t n)
{
    char* buf;
    assert(s != NULL);
    if (n == 0)
    {
        string->size = 0;
        string->data = EMPTY_STRING;
        return;
    }
    string->size = n;
    buf = (char*)chess_alloc(n + 1);
    strncpy(buf, s, n);
    buf[n] = '\0';
    string->data = buf;
}

void chess_string_cleanup(ChessString* string)
{
    if (string->size > 0)
        chess_free((char*)string->data);
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

void chess_string_assign_size(ChessString* string, const char* s, size_t n)
{
    chess_string_cleanup(string);
    chess_string_init_assign_size(string, s, n);
}
