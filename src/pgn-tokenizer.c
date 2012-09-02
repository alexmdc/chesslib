#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <assert.h>

#include "pgn-tokenizer.h"
#include "chess.h"

const int NOCHAR = -2;

static int tokenizer_getc(ChessPgnTokenizer* tokenizer)
{
    if (tokenizer->nextc == NOCHAR)
    {
        if (tokenizer->lastc == '\n')
        {
            tokenizer->line++;
            tokenizer->col = 1;
        }
        else if (tokenizer->lastc != EOF)
        {
            tokenizer->col++;
        }
    }
    else
    {
        tokenizer->nextc = NOCHAR;
    }
    tokenizer->lastc = chess_reader_getc(tokenizer->reader);
    return tokenizer->lastc;
}

static int tokenizer_peek(ChessPgnTokenizer* tokenizer)
{
    return chess_reader_peek(tokenizer->reader);
}

static void tokenizer_ungetc(ChessPgnTokenizer* tokenizer)
{
    chess_reader_ungetc(tokenizer->reader, tokenizer->lastc);
    tokenizer->nextc = tokenizer->lastc;
    tokenizer->lastc = NOCHAR;
}

static void token_init(ChessPgnToken* token)
{
    token->type = CHESS_PGN_TOKEN_NONE;
    chess_string_init(&token->string);
}

static void token_cleanup(ChessPgnToken* token)
{
    chess_string_cleanup(&token->string);
}

static void token_assign_simple(ChessPgnToken* token, ChessPgnTokenType type)
{
    token->type = type;
}

static void token_assign_symbol(ChessPgnToken* token, ChessBuffer* buffer)
{
    token->type = CHESS_PGN_TOKEN_SYMBOL;
    chess_string_assign_size(&token->string,
        chess_buffer_data(buffer), chess_buffer_size(buffer));
}

static void token_assign_string(ChessPgnToken* token, ChessBuffer* buffer)
{
    token->type = CHESS_PGN_TOKEN_STRING;
    chess_string_assign_size(&token->string,
        chess_buffer_data(buffer), chess_buffer_size(buffer));
}

static void token_assign_comment(ChessPgnToken* token, ChessBuffer* buffer)
{
    token->type = CHESS_PGN_TOKEN_COMMENT;
    chess_string_assign_size(&token->string,
        chess_buffer_data(buffer), chess_buffer_size(buffer));
}

static void token_assign_error(ChessPgnToken* token, const char* s)
{
    token->type = CHESS_PGN_TOKEN_ERROR;
    chess_string_assign(&token->string, s);
}

static ChessBoolean token_assign_number(ChessPgnToken* token, ChessBuffer* buffer)
{
    size_t i, n = chess_buffer_size(buffer);
    char* s = chess_buffer_data(buffer);

    if (n == 0)
        return CHESS_FALSE;

    for (i = 0; i < n; i++)
        if (!isdigit(s[i]))
            return CHESS_FALSE;

    token->type = CHESS_PGN_TOKEN_NUMBER;
    token->number = strtol(s, NULL, 10);
    return CHESS_TRUE;
}

static void token_assign_nag(ChessPgnToken* token, ChessBuffer* buffer)
{
    chess_buffer_null_terminate(buffer);
    token->type = CHESS_PGN_TOKEN_NAG;
    token->number = strtol(chess_buffer_data(buffer), NULL, 10);
}

static ChessBoolean read_string_token(ChessPgnTokenizer* tokenizer)
{
    /* Eat everything, but check for escape chars */
    int c;
    while ((c = tokenizer_getc(tokenizer)) != EOF)
    {
        if (c == '"')
            return CHESS_TRUE;

        if (c == '\\' && tokenizer_peek(tokenizer) == '"')
            c = tokenizer_getc(tokenizer);

        chess_buffer_append_char(&tokenizer->buffer, c);
    }
    return CHESS_FALSE; /* Not terminated */
}

static void read_symbol_token(ChessPgnTokenizer* tokenizer)
{
    int c;
    while ((c = tokenizer_getc(tokenizer)) != EOF
        && (isalnum(c) || strchr("_+#=:-/", c)))
            chess_buffer_append_char(&tokenizer->buffer, c);
    tokenizer_ungetc(tokenizer);
}

static void read_number_token(ChessPgnTokenizer* tokenizer)
{
    int c;
    while ((c = tokenizer_getc(tokenizer)) != EOF && isnumber(c))
        chess_buffer_append_char(&tokenizer->buffer, c);
    tokenizer_ungetc(tokenizer);
}

static ChessBoolean read_comment_token(ChessPgnTokenizer* tokenizer)
{
    int c;
    while ((c = tokenizer_getc(tokenizer)) != EOF)
    {
        if (c == '}')
            return CHESS_TRUE;

        chess_buffer_append_char(&tokenizer->buffer, c);
    }
    return CHESS_FALSE; /* Not terminated */
}

static ChessPgnToken* read_token(ChessPgnTokenizer* tokenizer)
{
    ChessPgnToken* token = &tokenizer->tokens[tokenizer->count++ % 2];
    ChessBuffer* buffer = &tokenizer->buffer;
    ChessBoolean ok;
    int c;

    while (isspace(c = tokenizer_getc(tokenizer)))
        ;

    token->line = tokenizer->line;
    token->col = tokenizer->col;

    chess_buffer_clear(buffer);

    if (c == '"')
    {
        /* String token */
        ok = read_string_token(tokenizer);
        if (!ok)
        {
            token_assign_error(token, "Unterminated string token.");
            return token;
        }
        token_assign_string(token, buffer);
        return token;
    }

    if (c == '$')
    {
        /* NAG token */
        read_number_token(tokenizer);
        if (chess_buffer_size(buffer) == 0)
        {
            token_assign_error(token, "Invalid NAG token.");
            return token;
        }
        token_assign_nag(token, buffer);
        return token;
    }

    if (c == '{')
    {
        /* Comment token */
        ok = read_comment_token(tokenizer);
        if (!ok)
        {
            token_assign_error(token, "Unterminated comment token.");
            return token;
        }
        token_assign_comment(token, buffer);
        return token;
    }

    if (isalnum(c) || c == '-')
    {
        /* Symbol or integer token */
        chess_buffer_append_char(buffer, c);
        read_symbol_token(tokenizer);
        chess_buffer_null_terminate(buffer);

        if (!token_assign_number(token, buffer))
        {
            if (!strcmp(chess_buffer_data(buffer), "1-0"))
            {
                token_assign_simple(token, CHESS_PGN_TOKEN_ONE_ZERO);
            }
            else if (!strcmp(chess_buffer_data(buffer), "0-1"))
            {
                token_assign_simple(token, CHESS_PGN_TOKEN_ZERO_ONE);
            }
            else if (!strcmp(chess_buffer_data(buffer), "1/2-1/2"))
            {
                token_assign_simple(token, CHESS_PGN_TOKEN_HALF_HALF);
            }
            else
            {
                token_assign_symbol(token, buffer);
            }

        }
        return token;
    }

    switch (c)
    {
        case EOF:
            token_assign_simple(token, CHESS_PGN_TOKEN_EOF);
            return token;
        case '(':
            token_assign_simple(token, CHESS_PGN_TOKEN_L_PARENTHESIS);
            return token;
        case ')':
            token_assign_simple(token, CHESS_PGN_TOKEN_R_PARENTHESIS);
            return token;
        case '[':
            token_assign_simple(token, CHESS_PGN_TOKEN_L_BRACKET);
            return token;
        case ']':
            token_assign_simple(token, CHESS_PGN_TOKEN_R_BRACKET);
            return token;
        case '*':
            token_assign_simple(token, CHESS_PGN_TOKEN_ASTERISK);
            return token;
        case '.':
            token_assign_simple(token, CHESS_PGN_TOKEN_PERIOD);
            return token;
        default:
            tokenizer_ungetc(tokenizer);
            token_assign_error(token, "Unknown token.");
            return token;
    }
}

ChessPgnTokenizer* chess_pgn_tokenizer_new(ChessReader* reader)
{
    ChessPgnTokenizer* tokenizer = malloc(sizeof(ChessPgnTokenizer));
    memset(tokenizer, 0, sizeof(ChessPgnTokenizer));
    tokenizer->reader = reader;
    tokenizer->line = 1;
    tokenizer->col = 0;
    tokenizer->nextc = NOCHAR;
    tokenizer->lastc = NOCHAR;
    token_init(&tokenizer->tokens[0]);
    token_init(&tokenizer->tokens[1]);
    chess_buffer_init(&tokenizer->buffer);
    return tokenizer;
}

void chess_pgn_tokenizer_destroy(ChessPgnTokenizer* tokenizer)
{
    token_cleanup(&tokenizer->tokens[0]);
    token_cleanup(&tokenizer->tokens[1]);
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
    tokenizer->next = NULL;
}

const ChessPgnToken* chess_pgn_tokenizer_next(ChessPgnTokenizer* tokenizer)
{
    const ChessPgnToken* last;
    if (tokenizer->next != NULL)
    {
        last = tokenizer->next;
        tokenizer->next = NULL;
        return last;
    }
    return read_token(tokenizer);
}
