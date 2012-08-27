#include <assert.h>
#include <string.h>
#include <ctype.h>

#include "chess.h"
#include "move.h"
#include "unmove.h"
#include "position.h"
#include "parse.h"
#include "generate.h"
#include "carray.h"

static ChessBoolean matches_move(const ChessPosition* position, ChessMove move,
    char piece, char from_file, char from_rank, char capture, char to_file, char to_rank, char promote)
{
    ChessSquare from = chess_move_from(move);
    ChessSquare to = chess_move_to(move);
    ChessPiece move_piece;
    ChessMovePromote move_promote;

    if (piece)
    {
        move_piece = chess_position_piece(position, from);
        if (tolower(chess_piece_to_char(move_piece)) != piece)
            return CHESS_FALSE;
    }

    if (from_file && chess_file_to_char(chess_square_file(from)) != from_file)
        return CHESS_FALSE;

    if (from_rank && chess_rank_to_char(chess_square_rank(from)) != from_rank)
        return CHESS_FALSE;

    if (capture && !chess_position_move_is_capture(position, move))
        return CHESS_FALSE;

    if (to_file && chess_file_to_char(chess_square_file(to)) != to_file)
        return CHESS_FALSE;

    if (to_rank && chess_rank_to_char(chess_square_rank(to)) != to_rank)
        return CHESS_FALSE;

    if (promote)
    {
        move_promote = chess_move_promotes(move);
        if (tolower(chess_move_promote_to_char(move_promote)) != promote)
            return CHESS_FALSE;
    }

    return CHESS_TRUE;
}

ChessParseResult chess_parse_move(const char* s, const ChessPosition* position, ChessMove* ret_move)
{
    char piece = '\0';
    char from_file = '\0', from_rank = '\0';
    char to_file = '\0', to_rank = '\0';
    char capture = '\0';
    char equals = '\0', promote = '\0';
    const char* c;
    ChessArray moves;
    ChessMove move, piece_move;
    ChessPiece pc;
    ChessBoolean pawn_move, pm, ambiguous;
    size_t i;

    assert(s && *s);

    if (!strncasecmp(s, "o-o-o", 5))
    {
        piece = 'k';
        from_file = 'e';
        to_file = 'g';
        s += 5;
    }
    else if (!strncasecmp(s, "o-o-o", 3))
    {
        piece = 'k';
        from_file = 'e';
        to_file = 'g';
        s += 3;
    }
    else
    {
        if ((c = strchr("NBRQKnrqk", *s)) && *c)
        {
            piece = tolower(*c);
            s++;
        }
        if ((c = strchr("abcdefgh", *s)) && *c)
        {
            from_file = *c;
            s++;
        }
        if ((c = strchr("12345678", *s)) && *c)
        {
            from_rank = *c;
            s++;
        }
        if (tolower(*s) == 'x')
        {
            capture = 'x';
            s++;
        }
        if ((c = strchr("abcdefgh", *s)) && *c)
        {
            to_file = *c;
            s++;
        }
        if ((c = strchr("12345678", *s)) && *c)
        {
            to_rank = *c;
            s++;
        }
        if (*s == '=')
        {
            equals = '=';
            s++;
        }
        if ((c = strchr("NBQKnbqk", *s)) && *c)
        {
            promote = tolower(*c);
            s++;
        }
    }

    if ((c = strchr("+#!?", *s)) && *c)
        s++;

    if (equals && !promote)
        return CHESS_PARSE_ERROR; /* Extra equals sign */

    if (*s)
        return CHESS_PARSE_ERROR; /* Leftover characters */

    if (!capture && !to_file && !to_rank)
    {
        to_file = from_file;
        from_file = 0;
        to_rank = from_rank;
        from_rank = 0;
    }

    chess_array_init(&moves, sizeof(ChessMove));
    chess_generate_moves(position, &moves);
    move = 0;
    piece_move = 0;
    pawn_move = CHESS_FALSE;
    ambiguous = CHESS_FALSE;
    for (i = 0; i < chess_array_size(&moves); i++)
    {
        move = *((ChessMove*)chess_array_elem(&moves, i));
        if (matches_move(position, move, piece, from_file, from_rank, capture, to_file, to_rank, promote))
        {
            if (piece)
            {
                if (piece_move)
                {
                    chess_array_cleanup(&moves);
                    return CHESS_PARSE_AMBIGUOUS_MOVE;
                }
                piece_move = move;
            }
            else
            {
                /* Need to prioritise pawn moves */
                pc = chess_position_piece(position, chess_move_from(move));
                pm = (pc == CHESS_PIECE_WHITE_PAWN || pc == CHESS_PIECE_BLACK_PAWN);
                if (!piece_move || (pm && !pawn_move))
                {
                    piece_move = move;
                    pawn_move = pm;
                    ambiguous = CHESS_FALSE;
                }
                else if (pm == pawn_move)
                {
                    if (pm)
                    {
                        /* Ambiguous pawn moves, no hope of correcting */
                        chess_array_cleanup(&moves);
                        return CHESS_PARSE_AMBIGUOUS_MOVE;
                    }
                    else
                    {
                        /* Ambiguous piece moves, we may still find a pawn move */
                        ambiguous = CHESS_TRUE;
                    }
                }
            }
        }
    }
    chess_array_cleanup(&moves);

    if (piece_move == 0)
        return CHESS_PARSE_ILLEGAL_MOVE;

    if (ambiguous)
        return CHESS_PARSE_AMBIGUOUS_MOVE;

    *ret_move = piece_move;
    return CHESS_PARSE_OK;
}
