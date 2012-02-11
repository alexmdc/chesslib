#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

#include "chess.h"
#include "move.h"
#include "unmove.h"
#include "position.h"
#include "fen.h"

void chess_fen_load(const char* s, ChessPosition* position)
{
    size_t len;
    ChessRank rank;
    ChessFile file;
    ChessPiece piece;
    ChessCastleState castle;
    char *s_copy, *tokens[6], *token, *c;
    int t, m, skip;

    /* Clone the string, as strtok will clobber it */
    len = strlen(s);
    s_copy = malloc((len + 1) * sizeof(char));
    strcpy(s_copy, s);

    t = 0;
    token = strtok(s_copy, " ");
    while (token && t < 6)
    {
        tokens[t++] = token;
        token = strtok(NULL, " ");
    }

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
                chess_position_set_piece(position, chess_square_from_fr(file, rank), piece);
                file++;
            }
            else if (isdigit(token[m]))
            {
                skip = token[m] - '0';
                while (skip--)
                {
                    chess_position_set_piece(position, chess_square_from_fr(file, rank), CHESS_PIECE_NONE);
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
        chess_position_set_to_move(position, CHESS_COLOR_WHITE);
    else if (!strcmp("b", tokens[1]))
        chess_position_set_to_move(position, CHESS_COLOR_BLACK);

    /* Castle availability */
    castle = 0;
    for (c = tokens[2]; *c; c++)
    {
        if (*c == 'K')
            castle |= CHESS_CASTLE_STATE_WK;
        else if (*c == 'Q')
            castle |= CHESS_CASTLE_STATE_WQ;
        else if (*c == 'k')
            castle |= CHESS_CASTLE_STATE_BK;
        else if (*c == 'q')
            castle |= CHESS_CASTLE_STATE_BQ;
    }
    chess_position_set_castle(position, castle);

    /* En passant */
    file = CHESS_FILE_INVALID;
    token = strchr("abcdefgh", tokens[3][0]);
    if (token && *token)
        file = *token - 'a';
    chess_position_set_ep(position, file);

    /* Half moves */
    chess_position_set_fifty(position, atoi(tokens[4]));

    /* Move num */
    chess_position_set_move_num(position, atoi(tokens[5]));

    free(s_copy);
}

void chess_fen_save(const ChessPosition* position, char* s)
{
    ChessFile file;
    ChessRank rank;
    ChessPiece piece;
    ChessCastleState castle = chess_position_castle(position);
    ChessFile ep = chess_position_ep(position);
    int run;

    for (rank = CHESS_RANK_8; rank >= CHESS_RANK_1; rank--)
    {
        run = 0;
        for (file = CHESS_FILE_A; file <= CHESS_FILE_H; file++)
        {
            piece = chess_position_piece(position, chess_square_from_fr(file, rank));
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
    *s++ = chess_position_to_move(position) == CHESS_COLOR_WHITE ? 'w' : 'b';
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
        *s++ = (chess_position_to_move(position) == CHESS_COLOR_WHITE) ? '6' : '3';
    }
    else
    {
        *s++ = '-';
    }
    s += sprintf(s, " %d %d",
                 chess_position_fifty(position),
                 chess_position_move_num(position));
}
