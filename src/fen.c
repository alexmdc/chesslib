#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

#include "fen.h"
#include "chess.h"
#include "position.h"
#include "calloc.h"

ChessBoolean chess_fen_load(const char* s, ChessPosition* position)
{
    size_t len;
    ChessPosition temp_position;
    ChessRank rank;
    ChessFile file;
    ChessPiece piece;
    char *s_copy, *tokens[6], *token, *c;
    int t, m, skip;

    /* Clone the string, as strtok will clobber it */
    len = strlen(s);
    s_copy = chess_alloc((len + 1) * sizeof(char));
    strcpy(s_copy, s);

    t = 0;
    token = strtok(s_copy, " ");
    while (token && t < 6)
    {
        tokens[t++] = token;
        token = strtok(NULL, " ");
    }

    /* Clear the position before filling it in */
    memset(&temp_position, 0, sizeof(ChessPosition));

    /* The first token is the board */
    rank = CHESS_RANK_8;
    token = strtok(tokens[0], "/");
    while (token && rank >= CHESS_RANK_1)
    {
        m = 0;
        file = CHESS_FILE_A;
        while (token[m] && file <= CHESS_FILE_H)
        {
            piece = chess_piece_from_char(token[m]);
            if (piece != CHESS_PIECE_NONE)
            {
                temp_position.piece[chess_square_from_fr(file, rank)] = piece;
                file++;
            }
            else if (isdigit(token[m]))
            {
                skip = token[m] - '0';
                while (skip--)
                {
                    temp_position.piece[chess_square_from_fr(file, rank)] = CHESS_PIECE_NONE;
                    file++;
                }
            }
            m++;
        }
        token = strtok(NULL, "/");
        rank--;
    }

    /* To move */
    if (!strcmp("w", tokens[1]))
        temp_position.to_move = CHESS_COLOR_WHITE;
    else if (!strcmp("b", tokens[1]))
        temp_position.to_move = CHESS_COLOR_BLACK;

    /* Castle availability */
    for (c = tokens[2]; *c; c++)
    {
        if (*c == 'K')
            temp_position.castle |= CHESS_CASTLE_STATE_WK;
        else if (*c == 'Q')
            temp_position.castle |= CHESS_CASTLE_STATE_WQ;
        else if (*c == 'k')
            temp_position.castle |= CHESS_CASTLE_STATE_BK;
        else if (*c == 'q')
            temp_position.castle |= CHESS_CASTLE_STATE_BQ;
    }

    /* En passant */
    temp_position.ep = CHESS_FILE_INVALID;
    token = strchr("abcdefgh", tokens[3][0]);
    if (token && *token)
        temp_position.ep = *token - 'a';

    /* Half moves */
    temp_position.fifty = atoi(tokens[4]);

    /* Move num */
    temp_position.move_num = atoi(tokens[5]);

    chess_free(s_copy);

    /* Validate the position before returning */
    if (chess_position_validate(&temp_position) == CHESS_FALSE)
        return CHESS_FALSE;

    chess_position_copy(&temp_position, position);
    return CHESS_TRUE;
}

void chess_fen_save(const ChessPosition* position, char* s)
{
    ChessFile file;
    ChessRank rank;
    ChessPiece piece;
    ChessCastleState castle = position->castle;
    ChessFile ep = position->ep;
    int run;

    for (rank = CHESS_RANK_8; rank >= CHESS_RANK_1; rank--)
    {
        run = 0;
        for (file = CHESS_FILE_A; file <= CHESS_FILE_H; file++)
        {
            piece = position->piece[chess_square_from_fr(file, rank)];
            if (piece == CHESS_PIECE_NONE)
            {
                run++;
                continue;
            }

            if (run)
            {
                *s++ = run + '0';
                run = 0;
            }
            *s++ = chess_piece_to_char(piece);
        }

        if (run)
            *s++ = run + '0';
        if (rank != CHESS_RANK_1)
            *s++ = '/';
    }

    *s++ = ' ';
    *s++ = position->to_move == CHESS_COLOR_WHITE ? 'w' : 'b';
    *s++ = ' ';
    if (castle != CHESS_CASTLE_STATE_NONE)
    {
        if (castle & CHESS_CASTLE_STATE_WK)
            *s++ = 'K';
        if (castle & CHESS_CASTLE_STATE_WQ)
            *s++ = 'Q';
        if (castle & CHESS_CASTLE_STATE_BK)
            *s++ = 'k';
        if (castle & CHESS_CASTLE_STATE_BQ)
            *s++ = 'q';
    }
    else
    {
        *s++ = '-';
    }
    *s++ = ' ';
    if (ep != CHESS_FILE_INVALID)
    {
        *s++ = chess_file_to_char(ep);
        *s++ = (position->to_move == CHESS_COLOR_WHITE) ? '6' : '3';
    }
    else
    {
        *s++ = '-';
    }
    s += sprintf(s, " %d %d", position->fifty, position->move_num);
}
