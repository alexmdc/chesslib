#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <assert.h>

#include "pgn-tokenizer.h"
#include "chess.h"

static void token_init_simple(ChessPgnToken* token, ChessPgnTokenType type)
{
    token->type = type;
}

static void token_init_symbol(ChessPgnToken* token, ChessBuffer* buffer)
{
    token->type = CHESS_PGN_TOKEN_SYMBOL;
    chess_string_init_assign_size(&token->data.string,
        chess_buffer_data(buffer), chess_buffer_size(buffer));
}

static void token_init_string(ChessPgnToken* token, ChessBuffer* buffer)
{
    token->type = CHESS_PGN_TOKEN_STRING;
    chess_string_init_assign_size(&token->data.string,
        chess_buffer_data(buffer), chess_buffer_size(buffer));
}

static void token_init_comment(ChessPgnToken* token, ChessBuffer* buffer)
{
    token->type = CHESS_PGN_TOKEN_COMMENT;
    chess_string_init_assign_size(&token->data.string,
        chess_buffer_data(buffer), chess_buffer_size(buffer));
}

static void token_init_error(ChessPgnToken* token, const char* s)
{
    token->type = CHESS_PGN_TOKEN_ERROR;
    chess_string_init_assign(&token->data.string, s);
}

static ChessBoolean token_init_number(ChessPgnToken* token, ChessBuffer* buffer)
{
    size_t i, n = chess_buffer_size(buffer);
    char* s = chess_buffer_data(buffer);

    if (n == 0)
        return CHESS_FALSE;

    for (i = 0; i < n; i++)
        if (!isdigit(s[i]))
            return CHESS_FALSE;

    token->type = CHESS_PGN_TOKEN_NUMBER;
    token->data.number = strtol(s, NULL, 10);
    return CHESS_TRUE;
}

static void token_init_nag(ChessPgnToken* token, ChessBuffer* buffer)
{
    chess_buffer_null_terminate(buffer);
    token->type = CHESS_PGN_TOKEN_NAG;
    token->data.number = strtol(chess_buffer_data(buffer), NULL, 10);
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

static ChessBoolean read_string_token(ChessReader* reader, ChessBuffer* buffer)
{
    /* Eat everything, but check for escape chars */
    int c;
    while ((c = chess_reader_getc(reader)) != EOF)
    {
        if (c == '"')
            return CHESS_TRUE;

        if (c == '\\' && chess_reader_peek(reader) == '"')
            c = chess_reader_getc(reader);

        chess_buffer_append_char(buffer, c);
    }
    return CHESS_FALSE; /* Not terminated */
}

static void read_symbol_token(ChessReader* reader, ChessBuffer* buffer)
{
    int c;
    while ((c = chess_reader_getc(reader)) != EOF
        && (isalnum(c) || strchr("_+#=:-/", c)))
            chess_buffer_append_char(buffer, c);
    chess_reader_ungetc(reader, c);
}

static void read_number_token(ChessReader* reader, ChessBuffer* buffer)
{
    int c;
    while ((c = chess_reader_getc(reader)) != EOF && isnumber(c))
        chess_buffer_append_char(buffer, c);
    chess_reader_ungetc(reader, c);
}

static ChessBoolean read_comment_token(ChessReader* reader, ChessBuffer* buffer)
{
    int c;
    while ((c = chess_reader_getc(reader)) != EOF)
    {
        if (c == '}')
            return CHESS_TRUE;

        chess_buffer_append_char(buffer, c);
    }
    return CHESS_FALSE; /* Not terminated */
}

static ChessPgnToken* read_token(ChessPgnTokenizer* tokenizer)
{
    ChessPgnToken* token = &tokenizer->tokens[tokenizer->count++ % 2];
    ChessBuffer* buffer = &tokenizer->buffer;
    ChessBoolean ok;
    int c;

    while (isspace(c = chess_reader_getc(tokenizer->reader)))
        ;

    chess_buffer_clear(buffer);

    if (c == '"')
    {
        /* String token */
        ok = read_string_token(tokenizer->reader, buffer);
        if (!ok)
        {
            token_init_error(token, "Unterminated string token.");
            return token;
        }
        token_init_string(token, buffer);
        return token;
    }

    if (c == '$')
    {
        /* NAG token */
        read_number_token(tokenizer->reader, buffer);
        if (chess_buffer_size(buffer) == 0)
        {
            token_init_error(token, "Invalid NAG token.");
            return token;
        }
        token_init_nag(token, buffer);
        return token;
    }

    if (c == '{')
    {
        /* Comment token */
        ok = read_comment_token(tokenizer->reader, buffer);
        if (!ok)
        {
            token_init_error(token, "Unterminated comment token.");
            return token;
        }
        token_init_comment(token, buffer);
        return token;
    }

    if (isalnum(c))
    {
        /* Symbol or integer token */
        chess_buffer_append_char(buffer, c);
        read_symbol_token(tokenizer->reader, buffer);
        chess_buffer_null_terminate(buffer);

        if (!token_init_number(token, buffer))
        {
            if (!strcmp(chess_buffer_data(buffer), "1-0"))
            {
                token_init_simple(token, CHESS_PGN_TOKEN_ONE_ZERO);
            }
            else if (!strcmp(chess_buffer_data(buffer), "0-1"))
            {
                token_init_simple(token, CHESS_PGN_TOKEN_ZERO_ONE);
            }
            else if (!strcmp(chess_buffer_data(buffer), "1/2-1/2"))
            {
                token_init_simple(token, CHESS_PGN_TOKEN_HALF_HALF);
            }
            else
            {
                token_init_symbol(token, buffer);
            }

        }
        return token;
    }

    switch (c)
    {
        case EOF:
            token_init_simple(token, CHESS_PGN_TOKEN_EOF);
            return token;
        case '(':
            token_init_simple(token, CHESS_PGN_TOKEN_L_PARENTHESIS);
            return token;
        case ')':
            token_init_simple(token, CHESS_PGN_TOKEN_R_PARENTHESIS);
            return token;
        case '[':
            token_init_simple(token, CHESS_PGN_TOKEN_L_BRACKET);
            return token;
        case ']':
            token_init_simple(token, CHESS_PGN_TOKEN_R_BRACKET);
            return token;
        case '*':
            token_init_simple(token, CHESS_PGN_TOKEN_ASTERISK);
            return token;
        case '.':
            token_init_simple(token, CHESS_PGN_TOKEN_PERIOD);
            return token;
        default:
            chess_reader_ungetc(tokenizer->reader, c);
            token_init_error(token, "Unknown token.");
            return token;
    }
}

ChessPgnTokenizer* chess_pgn_tokenizer_new(ChessReader* reader)
{
    ChessPgnTokenizer* tokenizer = malloc(sizeof(ChessPgnTokenizer));
    memset(tokenizer, 0, sizeof(ChessPgnTokenizer));
    tokenizer->reader = reader;
    chess_buffer_init(&tokenizer->buffer);
    return tokenizer;
}

void chess_pgn_tokenizer_destroy(ChessPgnTokenizer* tokenizer)
{
    if (tokenizer->last != NULL)
        token_cleanup(tokenizer->last);
    if (tokenizer->next != NULL)
        token_cleanup(tokenizer->next);
    chess_buffer_cleanup(&tokenizer->buffer);
    free(tokenizer);
}

const ChessPgnToken* chess_pgn_tokenizer_peek(ChessPgnTokenizer* tokenizer)
{
    if (tokenizer->next == NULL)
        tokenizer->next = read_token(tokenizer);
    assert(tokenizer->next != NULL);
    return tokenizer->next;
}

void chess_pgn_tokenizer_consume(ChessPgnTokenizer* tokenizer)
{
    if (tokenizer->last != NULL)
    {
        token_cleanup(tokenizer->last);
        tokenizer->last = NULL;
    }
    if (tokenizer->next != NULL)
    {
        token_cleanup(tokenizer->next);
        tokenizer->next = NULL;
    }
}

const ChessPgnToken* chess_pgn_tokenizer_next(ChessPgnTokenizer* tokenizer)
{
    if (tokenizer->last != NULL)
        token_cleanup(tokenizer->last);

    if (tokenizer->next != NULL)
    {
        tokenizer->last = tokenizer->next;
        tokenizer->next = NULL;
    }
    else
    {
        tokenizer->last = read_token(tokenizer);
    }

    return tokenizer->last;
}
