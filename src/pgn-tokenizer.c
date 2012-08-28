#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#include "pgn-tokenizer.h"
#include "chess.h"

const size_t INVALID = (size_t)-1;

static void token_init_simple(ChessPgnToken* token, ChessPgnTokenType type)
{
    token->type = type;
}

static void token_init_symbol(ChessPgnToken* token, const char* s, size_t n)
{
    token->type = CHESS_PGN_TOKEN_SYMBOL;
    chess_string_init_assign_size(&token->data.string, s, n);
}

static void token_init_string(ChessPgnToken* token, const char* s, size_t n)
{
    char* buf = malloc(n + 1);
    size_t i, b = 0;

    for (i = 0; i < n; i++)
    {
        if (s[i] == '\\' && (s[i + 1] == '\\' || s[i + 1] == '"'))
            i++;
        buf[b++] = s[i];
    }
    token->type = CHESS_PGN_TOKEN_STRING;
    chess_string_init_assign_size(&token->data.string, buf, b);
    free(buf);
}

static void token_init_comment(ChessPgnToken* token, const char* s, size_t n)
{
    token->type = CHESS_PGN_TOKEN_COMMENT;
    chess_string_init_assign_size(&token->data.string, s, n);
}

static void token_init_error(ChessPgnToken* token, const char* s)
{
    token->type = CHESS_PGN_TOKEN_ERROR;
    chess_string_init_assign(&token->data.string, s);
}

static ChessBoolean token_init_number(ChessPgnToken* token, const char* s, size_t n)
{
    size_t i;
    if (n == 0)
        return CHESS_FALSE;

    for (i = 0; i < n; i++)
        if (!isdigit(s[i]))
            return CHESS_FALSE;

    token->type = CHESS_PGN_TOKEN_NUMBER;
    token->data.number = strtol(s, NULL, 10);
    return CHESS_TRUE;
}

static void token_init_nag(ChessPgnToken* token, const char* s)
{
    token->type = CHESS_PGN_TOKEN_NAG;
    token->data.number = strtol(s, NULL, 10);
}

static void token_cleanup(ChessPgnToken* token)
{
    switch (token->type)
    {
        case CHESS_PGN_TOKEN_SYMBOL:
        case CHESS_PGN_TOKEN_STRING:
        case CHESS_PGN_TOKEN_COMMENT:
        case CHESS_PGN_TOKEN_ERROR:
            chess_string_cleanup(&token->data.string);
            break;
        default:
            break;
    }
}

static size_t read_string_token(ChessReader* reader, char* buf)
{
    /* Eat everything, but check for escape chars */
    size_t n = 0;
    int c;
    while ((c = chess_reader_getc(reader)) != EOF)
    {
        if (c == '"')
            return n;

        if (c == '\\' && chess_reader_peek(reader) == '"')
            c = chess_reader_getc(reader);

        buf[n++] = c;
    }
    return INVALID; /* Not terminated */
}

static size_t read_symbol_token(ChessReader* reader, char* buf)
{
    size_t n = 0;
    int c;
    while ((c = chess_reader_getc(reader)) != EOF
        && (isalnum(c) || strchr("_+#=:-/", c)))
            buf[n++] = c;
    chess_reader_ungetc(reader, c);
    return n;
}

static size_t read_number_token(ChessReader* reader, char* buf)
{
    size_t n = 0;
    int c;
    while ((c = chess_reader_getc(reader)) != EOF && isnumber(c))
        buf[n++] = c;
    chess_reader_ungetc(reader, c);
    return n;
}

static size_t read_comment_token(ChessReader* reader, char* buf)
{
    size_t n = 0;
    int c;
    while ((c = chess_reader_getc(reader)) != EOF)
    {
        if (c == '}')
            return n;

        buf[n++] = c;
    }
    return INVALID; /* Not terminated */
}

static void skip_token(ChessPgnTokenizer* tokenizer)
{
    ChessPgnToken* token = tokenizer->last;
    char buf[128];
    size_t size;
    int c;

    token_cleanup(tokenizer->last);
    tokenizer->last = tokenizer->next;
    tokenizer->next = token;

    while (isspace(c = chess_reader_getc(tokenizer->reader)))
        ;

    if (c == '"')
    {
        /* String token */
        size = read_string_token(tokenizer->reader, buf);
        if (size == INVALID)
        {
            token_init_error(token, "Unterminated string token.");
            return;
        }
        token_init_string(token, buf, size);
        return;
    }

    if (c == '$')
    {
        /* NAG token */
        size = read_number_token(tokenizer->reader, buf);
        if (size == 0)
        {
            token_init_error(token, "Invalid NAG token.");
            return;
        }
        buf[size] = '\0';
        token_init_nag(token, buf);
        return;
    }

    if (c == '{')
    {
        /* Comment token */
        size = read_comment_token(tokenizer->reader, buf);
        if (size == INVALID)
        {
            token_init_error(token, "Unterminated comment token.");
            return;
        }
        token_init_comment(token, buf, size);
        return;
    }

    if (isalnum(c))
    {
        /* Symbol or integer token */
        buf[0] = c;
        size = read_symbol_token(tokenizer->reader, buf + 1) + 1;
        buf[size] = '\0';
        if (!token_init_number(token, buf, size))
        {
            if (!strcmp(buf, "1-0"))
            {
                token_init_simple(token, CHESS_PGN_TOKEN_ONE_ZERO);
            }
            else if (!strcmp(buf, "0-1"))
            {
                token_init_simple(token, CHESS_PGN_TOKEN_ZERO_ONE);
            }
            else if (!strcmp(buf, "1/2-1/2"))
            {
                token_init_simple(token, CHESS_PGN_TOKEN_HALF_HALF);
            }
            else
            {
                token_init_symbol(token, buf, size);
            }

        }
        return;
    }

    switch (c)
    {
        case EOF:
            token_init_simple(token, CHESS_PGN_TOKEN_EOF);
            return;
        case '(':
            token_init_simple(token, CHESS_PGN_TOKEN_L_PARENTHESIS);
            return;
        case ')':
            token_init_simple(token, CHESS_PGN_TOKEN_R_PARENTHESIS);
            return;
        case '[':
            token_init_simple(token, CHESS_PGN_TOKEN_L_BRACKET);
            return;
        case ']':
            token_init_simple(token, CHESS_PGN_TOKEN_R_BRACKET);
            return;
        case '*':
            token_init_simple(token, CHESS_PGN_TOKEN_ASTERISK);
            return;
        case '.':
            token_init_simple(token, CHESS_PGN_TOKEN_PERIOD);
            return;
        default:
            chess_reader_ungetc(tokenizer->reader, c);
            token_init_error(token, "Unknown token.");
            break;
    }
}

ChessPgnTokenizer* chess_pgn_tokenizer_new(ChessReader* reader)
{
    ChessPgnTokenizer* tokenizer = malloc(sizeof(ChessPgnTokenizer));
    memset(tokenizer, 0, sizeof(ChessPgnTokenizer));
    tokenizer->reader = reader;
    tokenizer->last = &tokenizer->tokens[0];
    tokenizer->next = &tokenizer->tokens[1];
    skip_token(tokenizer);
    return tokenizer;
}

const ChessPgnToken* chess_pgn_tokenizer_peek(const ChessPgnTokenizer* tokenizer)
{
    return tokenizer->next;
}

const ChessPgnToken* chess_pgn_tokenizer_next(ChessPgnTokenizer* tokenizer)
{
    skip_token(tokenizer);
    return tokenizer->last;
}

void chess_pgn_tokenizer_destroy(ChessPgnTokenizer* tokenizer)
{
    token_cleanup(tokenizer->last);
    token_cleanup(tokenizer->next);
    free(tokenizer);
}
