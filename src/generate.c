#include <assert.h>
#include <stdio.h>

#include "generate.h"

typedef enum {
    DIR_N = (1 << 0),
    DIR_NE = (1 << 1),
    DIR_E = (1 << 2),
    DIR_SE = (1 << 3),
    DIR_S = (1 << 4),
    DIR_SW = (1 << 5),
    DIR_W = (1 << 6),
    DIR_NW = (1 << 7)
} Dirs;

typedef enum {
    SLIDE_N = 8,
    SLIDE_NE = 9,
    SLIDE_E = 1,
    SLIDE_SE = -7,
    SLIDE_S = -8,
    SLIDE_SW = -9,
    SLIDE_W = -1,
    SLIDE_NW = 7
} Slides;

typedef enum {
    JUMP_NNE = 17,
    JUMP_ENE = 10,
    JUMP_ESE = -6,
    JUMP_SSE = -15,
    JUMP_SSW = -17,
    JUMP_WSW = -10,
    JUMP_WNW = 6,
    JUMP_NNW = 15
} Jumps;

static int slide_dirs[64];
static int jump_dirs[64];

static int dirs_array[] = { DIR_N, DIR_NE, DIR_E, DIR_SE, DIR_S, DIR_SW, DIR_W, DIR_NW };
static int slides_array[] = { SLIDE_N, SLIDE_NE, SLIDE_E, SLIDE_SE, SLIDE_S, SLIDE_SW, SLIDE_W, SLIDE_NW };
static int jumps_array[] = { JUMP_NNE, JUMP_ENE, JUMP_ESE, JUMP_SSE, JUMP_SSW, JUMP_WSW, JUMP_WNW, JUMP_NNW };

static int rook_dirs = DIR_N | DIR_E | DIR_S | DIR_W;
static int bishop_dirs = DIR_NE | DIR_SE | DIR_SW | DIR_NW;
static int queen_dirs = 0xff;

void chess_generate_init(void)
{
    static int initialized = 0;
    ChessSquare sq;
    int dirs, d;
    int slide, jump;
    int file, rank;
    int to_file, to_rank;

    if (initialized)
        return;
    initialized = 1;

    for (sq = CHESS_SQUARE_A1; sq <= CHESS_SQUARE_H8; sq++)
    {
        dirs = 0;
        file = chess_square_file(sq);
        rank = chess_square_rank(sq);
        for (d = 0; d < 8; d++)
        {
            slide = slides_array[d];
            to_file = file + chess_square_file(slide + 36) - 4;
            to_rank = rank + chess_square_rank(slide + 36) - 4;
            if (to_file >= CHESS_FILE_A && to_file <= CHESS_FILE_H
            &&  to_rank >= CHESS_RANK_1 && to_rank <= CHESS_RANK_8)
                dirs |= dirs_array[d];
        }

        slide_dirs[sq] = dirs;
    }

    for (sq = CHESS_SQUARE_A1; sq <= CHESS_SQUARE_H8; sq++)
    {
        dirs = 0;
        file = chess_square_file(sq);
        rank = chess_square_rank(sq);
        for (d = 0; d < 8; d++)
        {
            jump = jumps_array[d];
            to_file = file + chess_square_file(jump + 36) - 4;
            to_rank = rank + chess_square_rank(jump + 36) - 4;
            if (to_file >= CHESS_FILE_A && to_file <= CHESS_FILE_H
            &&  to_rank >= CHESS_RANK_1 && to_rank <= CHESS_RANK_8)
                dirs |= dirs_array[d];
        }

        jump_dirs[sq] = dirs;
    }
}

static void append_move(ChessArray* moves, ChessMove move)
{
    chess_array_push(moves, &move);
}

static void gen_pawn_promotes(ChessSquare from, ChessSquare to, ChessArray* moves)
{
    append_move(moves, chess_move_make_promote(from, to, CHESS_MOVE_PROMOTE_KNIGHT));
    append_move(moves, chess_move_make_promote(from, to, CHESS_MOVE_PROMOTE_BISHOP));
    append_move(moves, chess_move_make_promote(from, to, CHESS_MOVE_PROMOTE_ROOK));
    append_move(moves, chess_move_make_promote(from, to, CHESS_MOVE_PROMOTE_QUEEN));
}

static void gen_pawn_moves(const ChessPosition* position, ChessSquare sq, ChessColor color, ChessArray* moves)
{
    ChessSquare to;
    ChessPiece piece;
    ChessRank start_rank = (color == CHESS_COLOR_WHITE) ? CHESS_RANK_2 : CHESS_RANK_7;
    ChessRank end_rank = (color == CHESS_COLOR_WHITE) ? CHESS_RANK_8 : CHESS_RANK_1;
    int slide = (color == CHESS_COLOR_WHITE) ? SLIDE_N : SLIDE_S;
    int capture_dirs = (color == CHESS_COLOR_WHITE) ? DIR_NE | DIR_NW : DIR_SE | DIR_SW;
    int dirs = capture_dirs & slide_dirs[sq];
    ChessFile ep_file = chess_position_ep(position);
    ChessSquare ep = (ep_file == CHESS_FILE_INVALID) ? CHESS_SQUARE_INVALID :
        chess_square_from_fr(ep_file, (color == CHESS_COLOR_WHITE) ? CHESS_RANK_6 : CHESS_RANK_3);
    int d;

    to = sq + slide;
    if (chess_position_piece(position, to) == CHESS_PIECE_NONE)
    {
        if (chess_square_rank(to) == end_rank)
        {
            gen_pawn_promotes(sq, to, moves);
        }
        else
        {
            append_move(moves, chess_move_make(sq, to));
            if (chess_square_rank(sq) == start_rank)
            {
                to += slide;
                if (chess_position_piece(position, to) == CHESS_PIECE_NONE)
                {
                    append_move(moves, chess_move_make(sq, to));
                }
            }
        }
    }

    for (d = 1; d < 8; d += 2)
    {
        if (dirs_array[d] & dirs)
        {
            to = sq + slides_array[d];
            piece = chess_position_piece(position, to);
            if (piece != CHESS_PIECE_NONE && chess_piece_color(piece) != color)
            {
                if (chess_square_rank(to) == end_rank)
                    gen_pawn_promotes(sq, to, moves);
                else
                    append_move(moves, chess_move_make(sq, to));
            }
            else if (to == ep)
            {
                append_move(moves, chess_move_make(sq, to));
            }
        }
    }
}

static void gen_knight_moves(const ChessPosition* position, ChessSquare sq, ChessColor color, ChessArray* moves)
{
    ChessSquare to;
    int dirs, dir, d;
    ChessPiece target;

    dirs = jump_dirs[sq];
    for (d = 0; d < 8; d++)
    {
        dir = dirs_array[d];
        if ((dir & dirs) == 0)
            continue;

        to = sq + jumps_array[d];
        target = chess_position_piece(position, to);
        if (target == CHESS_PIECE_NONE || chess_piece_color(target) != color)
            append_move(moves, chess_move_make(sq, to));
    }
}

static void gen_king_moves(const ChessPosition* position, ChessSquare sq, ChessColor color, ChessArray* moves)
{
    ChessSquare to;
    int dirs, dir, d;
    ChessPiece target;

    dirs = slide_dirs[sq];
    for (d = 0; d < 8; d++)
    {
        dir = dirs_array[d];
        if ((dir & dirs) == 0)
            continue;

        to = sq + slides_array[d];
        target = chess_position_piece(position, to);
        if (target == CHESS_PIECE_NONE || chess_piece_color(target) != color)
            append_move(moves, chess_move_make(sq, to));
    }
}

static void gen_slide_moves(const ChessPosition* position, ChessSquare sq, int piece_dirs, ChessColor color, ChessArray* moves)
{
    ChessSquare to;
    int dirs, dir, d;
    ChessPiece target;

    for (d = 0; d < 8; d++)
    {
        dir = dirs_array[d] & piece_dirs;
        to = sq;

        do
        {
            dirs = slide_dirs[to];
            if ((dir & dirs) == 0)
                break;

            to += slides_array[d];
            target = chess_position_piece(position, to);
            if (target == CHESS_PIECE_NONE || chess_piece_color(target) != color)
                append_move(moves, chess_move_make(sq, to));
        } while (target == CHESS_PIECE_NONE);
    }
}

static void gen_castle_moves(const ChessPosition* position, ChessColor color, ChessArray* moves)
{
    ChessCastleState castle = chess_position_castle(position);

    if (color == CHESS_COLOR_WHITE)
    {
        if ((castle & CHESS_CASTLE_STATE_WK)
            && chess_position_piece(position, CHESS_SQUARE_F1) == CHESS_PIECE_NONE
            && chess_position_piece(position, CHESS_SQUARE_G1) == CHESS_PIECE_NONE
            && !chess_generate_is_square_attacked(position, CHESS_SQUARE_F1, CHESS_COLOR_BLACK)
            && !chess_generate_is_square_attacked(position, CHESS_SQUARE_G1, CHESS_COLOR_BLACK))
            append_move(moves, chess_move_make(CHESS_SQUARE_E1, CHESS_SQUARE_G1));

        if ((castle & CHESS_CASTLE_STATE_WQ)
            && chess_position_piece(position, CHESS_SQUARE_B1) == CHESS_PIECE_NONE
            && chess_position_piece(position, CHESS_SQUARE_C1) == CHESS_PIECE_NONE
            && chess_position_piece(position, CHESS_SQUARE_D1) == CHESS_PIECE_NONE
            && !chess_generate_is_square_attacked(position, CHESS_SQUARE_D1, CHESS_COLOR_BLACK)
            && !chess_generate_is_square_attacked(position, CHESS_SQUARE_C1, CHESS_COLOR_BLACK))
            append_move(moves, chess_move_make(CHESS_SQUARE_E1, CHESS_SQUARE_C1));
    }
    else
    {
        if ((castle & CHESS_CASTLE_STATE_BK)
            && chess_position_piece(position, CHESS_SQUARE_F8) == CHESS_PIECE_NONE
            && chess_position_piece(position, CHESS_SQUARE_G8) == CHESS_PIECE_NONE
            && !chess_generate_is_square_attacked(position, CHESS_SQUARE_F8, CHESS_COLOR_WHITE)
            && !chess_generate_is_square_attacked(position, CHESS_SQUARE_G8, CHESS_COLOR_WHITE))
            append_move(moves, chess_move_make(CHESS_SQUARE_E8, CHESS_SQUARE_G8));

        if ((castle & CHESS_CASTLE_STATE_BQ)
            && chess_position_piece(position, CHESS_SQUARE_B8) == CHESS_PIECE_NONE
            && chess_position_piece(position, CHESS_SQUARE_C8) == CHESS_PIECE_NONE
            && chess_position_piece(position, CHESS_SQUARE_D8) == CHESS_PIECE_NONE
            && !chess_generate_is_square_attacked(position, CHESS_SQUARE_D8, CHESS_COLOR_WHITE)
            && !chess_generate_is_square_attacked(position, CHESS_SQUARE_C8, CHESS_COLOR_WHITE))
            append_move(moves, chess_move_make(CHESS_SQUARE_E8, CHESS_SQUARE_C8));
    }
}

static void filter_illegal_moves(const ChessPosition* position, ChessColor color, ChessArray* moves)
{
    ChessPosition temp_position;
    ChessColor other = chess_color_other(color);
    ChessMove move;
    ChessUnmove unmove;
    int i, m = 0;

    chess_position_copy(position, &temp_position);
    for (i = 0; i < chess_array_size(moves); i++)
    {
        move = *((ChessMove*)chess_array_elem(moves, i));
        unmove = chess_position_make_move(&temp_position,  move);
        chess_position_set_to_move(&temp_position, color);
        if (!chess_position_is_check(&temp_position))
        {
            chess_array_set_elem(moves, m++, &move);
        }
        chess_position_set_to_move(&temp_position, other);
        chess_position_undo_move(&temp_position, unmove);
    }

    chess_array_prune(moves, m);
}

void chess_generate_moves(const ChessPosition* position, ChessArray* moves)
{
    ChessSquare sq;
    ChessPiece piece;
    ChessColor color = chess_position_to_move(position);

    for (sq = CHESS_SQUARE_A1; sq <= CHESS_SQUARE_H8; sq++)
    {
        piece = chess_position_piece(position, sq);
        if (piece == CHESS_PIECE_NONE)
            continue;

        if (color != chess_piece_color(piece))
            continue;

        switch (piece)
        {
            case CHESS_PIECE_WHITE_PAWN:
            case CHESS_PIECE_BLACK_PAWN:
                gen_pawn_moves(position, sq, color, moves);
                break;
            case CHESS_PIECE_WHITE_KNIGHT:
            case CHESS_PIECE_BLACK_KNIGHT:
                gen_knight_moves(position, sq, color, moves);
                break;
            case CHESS_PIECE_WHITE_BISHOP:
            case CHESS_PIECE_BLACK_BISHOP:
                gen_slide_moves(position, sq, bishop_dirs, color, moves);
                break;
            case CHESS_PIECE_WHITE_ROOK:
            case CHESS_PIECE_BLACK_ROOK:
                gen_slide_moves(position, sq, rook_dirs, color, moves);
                break;
            case CHESS_PIECE_WHITE_QUEEN:
            case CHESS_PIECE_BLACK_QUEEN:
                gen_slide_moves(position, sq, queen_dirs, color, moves);
                break;
            case CHESS_PIECE_WHITE_KING:
            case CHESS_PIECE_BLACK_KING:
                gen_king_moves(position, sq, color, moves);
                break;
            default:
                assert(0);
                break;
        }
    }

    if (!chess_position_is_check(position))
        gen_castle_moves(position, color, moves);

    filter_illegal_moves(position, color, moves);
}

ChessBoolean chess_generate_is_square_attacked(const ChessPosition* position, ChessSquare sq, ChessColor color)
{
    ChessSquare from;
    ChessPiece piece;
    int dirs, dir, d, dist;

    /* Check for knight attacks */
    dirs = jump_dirs[sq];
    for (d = 0; d < 8; d++)
    {
        dir = dirs_array[d];
        if ((dir & dirs) == 0)
            continue;

        from = sq + jumps_array[d];
        piece = chess_position_piece(position, from);
        if (chess_position_piece(position, from) == chess_piece_of_color(CHESS_PIECE_WHITE_KNIGHT, color))
            return CHESS_TRUE;
    }

    for (d = 0; d < 8; d++)
    {
        dir = dirs_array[d];
        from = sq;
        dist = 1;

        do
        {
            dirs = slide_dirs[from];
            if ((dir & dirs) == 0)
                break;

            from += slides_array[d];
            piece = chess_position_piece(position, from);
            if (piece != CHESS_PIECE_NONE && chess_piece_color(piece) == color)
            {
                switch (piece)
                {
                    case CHESS_PIECE_WHITE_QUEEN:
                    case CHESS_PIECE_BLACK_QUEEN:
                        return CHESS_TRUE;
                    case CHESS_PIECE_WHITE_BISHOP:
                    case CHESS_PIECE_BLACK_BISHOP:
                        if (dir & bishop_dirs)
                            return CHESS_TRUE;
                        break;
                    case CHESS_PIECE_WHITE_ROOK:
                    case CHESS_PIECE_BLACK_ROOK:
                        if (dir & rook_dirs)
                            return CHESS_TRUE;
                        break;
                    case CHESS_PIECE_WHITE_KING:
                    case CHESS_PIECE_BLACK_KING:
                        if (dist == 1)
                            return CHESS_TRUE;
                        break;
                    case CHESS_PIECE_WHITE_PAWN:
                        if (dist == 1 && dir & (DIR_SE | DIR_SW))
                            return CHESS_TRUE;
                        break;
                    case CHESS_PIECE_BLACK_PAWN:
                        if (dist == 1 && dir & (DIR_NE | DIR_NW))
                            return CHESS_TRUE;
                        break;
                    case CHESS_PIECE_WHITE_KNIGHT:
                    case CHESS_PIECE_BLACK_KNIGHT:
                        break;
                    default:
                        assert(CHESS_FALSE);
                        break;
                }
            }
            dist++;
        } while (piece == CHESS_PIECE_NONE);
    }

    return CHESS_FALSE;
}
