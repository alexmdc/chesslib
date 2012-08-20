#include <assert.h>
#include <stdlib.h>
#include <memory.h>
#include <stdio.h>

#include "position.h"
#include "generate.h"
#include "fen.h"
#include "carray.h"

ChessPosition* chess_position_new(void)
{
    ChessPosition* position = malloc(sizeof(ChessPosition));
    memset(position, 0, sizeof(ChessPosition));
    position->move_num = 1;
    position->ep = CHESS_FILE_INVALID;
    return position;
}

ChessPosition* chess_position_clone(const ChessPosition* position)
{
    ChessPosition* clone = chess_position_new();
    memcpy(clone, position, sizeof(ChessPosition));
    return clone;
}

void chess_position_destroy(ChessPosition* position)
{
    assert(position != NULL);
    free(position);
}

void chess_position_init(ChessPosition* position)
{
    memset(position, 0, sizeof(ChessPosition));
    chess_fen_load("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", position);
}

void chess_position_copy(const ChessPosition* from, ChessPosition* to)
{
    memcpy(to, from, sizeof(ChessPosition));
}

ChessPiece chess_position_piece(const ChessPosition* position, ChessSquare square)
{
    return position->piece[square];
}

ChessColor chess_position_to_move(const ChessPosition* position)
{
    return position->to_move;
}

ChessCastleState chess_position_castle(const ChessPosition* position)
{
    return position->castle;
}

ChessFile chess_position_ep(const ChessPosition* position)
{
    return position->ep;
}

int chess_position_fifty(const ChessPosition* position)
{
    return position->fifty;
}

int chess_position_move_num(const ChessPosition* position)
{
    return position->move_num;
}

void chess_position_set_piece(ChessPosition* position, ChessSquare square, ChessPiece value)
{
    position->piece[square] = value;
    if (value == CHESS_PIECE_WHITE_KING)
        position->wking = square;
    else if (value == CHESS_PIECE_BLACK_KING)
        position->bking = square;
}

void chess_position_set_to_move(ChessPosition* position, ChessColor value)
{
    position->to_move = value;
}

void chess_position_set_castle(ChessPosition* position, ChessCastleState value)
{
    position->castle = value;
}

void chess_position_set_ep(ChessPosition* position, ChessFile value)
{
    position->ep = value;
}

void chess_position_set_fifty(ChessPosition* position, int value)
{
    position->fifty = value;
}

void chess_position_set_move_num(ChessPosition* position, int value)
{
    position->move_num = value;
}

ChessBoolean chess_position_is_check(const ChessPosition* position)
{
    if (position->to_move == CHESS_COLOR_WHITE)
        return chess_generate_is_square_attacked(position, position->wking, CHESS_COLOR_BLACK);
    else
        return chess_generate_is_square_attacked(position, position->bking, CHESS_COLOR_WHITE);
}

ChessBoolean chess_position_move_is_legal(const ChessPosition* position, ChessMove move)
{
    ChessArray moves;
    ChessBoolean found = CHESS_FALSE;
    size_t i;

    chess_array_init(&moves, sizeof(ChessMove));
    chess_generate_moves(position, &moves);

    for (i = 0; i < chess_array_size(&moves); i++)
    {
        if (*((ChessMove*)chess_array_elem(&moves, i)) == move)
        {
            found = CHESS_TRUE;
            break;
        }
    }
    chess_array_cleanup(&moves);
    return found;
}

ChessBoolean chess_position_move_is_capture(const ChessPosition* position, ChessMove move)
{
    ChessSquare to = chess_move_to(move);
    ChessRank ep_rank;
    if (position->piece[to] != CHESS_PIECE_NONE)
        return CHESS_TRUE;

    /* Special case is en passant */
    ep_rank = (position->to_move == CHESS_COLOR_WHITE) ? CHESS_RANK_6 : CHESS_RANK_3;
    return (position->ep != CHESS_FILE_INVALID && to == chess_square_from_fr(position->ep, ep_rank));
}

ChessResult chess_position_check_result(const ChessPosition* position)
{
    ChessArray moves;
    size_t m;

    chess_array_init(&moves, sizeof(ChessMove));
    chess_generate_moves(position, &moves);
    m = chess_array_size(&moves);
    chess_array_cleanup(&moves);

    if (m > 0)
        return CHESS_RESULT_NONE;

    if (!chess_position_is_check(position))
        return CHESS_RESULT_DRAW; /* Stalemate */

    return (position->to_move == CHESS_COLOR_WHITE) ?
        CHESS_RESULT_BLACK_WINS : CHESS_RESULT_WHITE_WINS;
}

static ChessUnmoveCaptured capture_piece(ChessPiece piece)
{
    switch (piece)
    {
        case CHESS_PIECE_NONE:
            return CHESS_UNMOVE_CAPTURED_NONE;
        case CHESS_PIECE_WHITE_PAWN:
        case CHESS_PIECE_BLACK_PAWN:
            return CHESS_UNMOVE_CAPTURED_PAWN;
        case CHESS_PIECE_WHITE_KNIGHT:
        case CHESS_PIECE_BLACK_KNIGHT:
            return CHESS_UNMOVE_CAPTURED_KNIGHT;
        case CHESS_PIECE_WHITE_BISHOP:
        case CHESS_PIECE_BLACK_BISHOP:
            return CHESS_UNMOVE_CAPTURED_BISHOP;
        case CHESS_PIECE_WHITE_ROOK:
        case CHESS_PIECE_BLACK_ROOK:
            return CHESS_UNMOVE_CAPTURED_ROOK;
        case CHESS_PIECE_WHITE_QUEEN:
        case CHESS_PIECE_BLACK_QUEEN:
            return CHESS_UNMOVE_CAPTURED_QUEEN;
        default:
            assert(0);
            return 0;
    }
}

static ChessPiece promoted_piece(ChessMovePromote promote, ChessColor color)
{
    switch (promote)
    {
        case CHESS_MOVE_PROMOTE_KNIGHT:
            return chess_piece_of_color(CHESS_PIECE_WHITE_KNIGHT, color);
        case CHESS_MOVE_PROMOTE_BISHOP:
            return chess_piece_of_color(CHESS_PIECE_WHITE_BISHOP, color);
        case CHESS_MOVE_PROMOTE_ROOK:
            return chess_piece_of_color(CHESS_PIECE_WHITE_ROOK, color);
        case CHESS_MOVE_PROMOTE_QUEEN:
            return chess_piece_of_color(CHESS_PIECE_WHITE_QUEEN, color);
        default:
            assert(0);
            return 0;
    }
}

ChessUnmove chess_position_make_move(ChessPosition* position, ChessMove move)
{
    ChessSquare from = chess_move_from(move);
    ChessSquare to = chess_move_to(move);
    ChessMovePromote promote = chess_move_promotes(move);
    ChessPiece piece = position->piece[from];
    ChessColor color = chess_piece_color(piece);
    ChessUnmoveEp ep;
    ChessUnmoveCaptured captured = capture_piece(position->piece[to]);
    ChessCastleState castle = position->castle;
    int fifty = position->fifty;

    /* Move the piece */
    position->piece[from] = CHESS_PIECE_NONE;
    if (promote == CHESS_MOVE_PROMOTE_NONE)
    {
        position->piece[to] = piece;
        if (piece == CHESS_PIECE_WHITE_KING)
            position->wking = to;
        else if (piece == CHESS_PIECE_BLACK_KING)
            position->bking = to;
    }
    else
    {
        position->piece[to] = promoted_piece(promote, color);
    }

    /* Handle castling */
    if (piece == CHESS_PIECE_WHITE_KING && from == CHESS_SQUARE_E1)
    {
        if (to == CHESS_SQUARE_G1)
        {
            position->piece[CHESS_SQUARE_F1] = CHESS_PIECE_WHITE_ROOK;
            position->piece[CHESS_SQUARE_H1] = CHESS_PIECE_NONE;
        }
        else if (to == CHESS_SQUARE_C1)
        {
            position->piece[CHESS_SQUARE_D1] = CHESS_PIECE_WHITE_ROOK;
            position->piece[CHESS_SQUARE_A1] = CHESS_PIECE_NONE;
        }
    }
    else if (piece == CHESS_PIECE_BLACK_KING && from == CHESS_SQUARE_E8)
    {
        if (to == CHESS_SQUARE_G8)
        {
            position->piece[CHESS_SQUARE_F8] = CHESS_PIECE_BLACK_ROOK;
            position->piece[CHESS_SQUARE_H8] = CHESS_PIECE_NONE;
        }
        else if (to == CHESS_SQUARE_C8)
        {
            position->piece[CHESS_SQUARE_D8] = CHESS_PIECE_BLACK_ROOK;
            position->piece[CHESS_SQUARE_A8] = CHESS_PIECE_NONE;
        }
    }

    /* Handle ep */
    if (position->ep == CHESS_FILE_INVALID)
    {
        ep = CHESS_UNMOVE_EP_NONE;
    }
    else
    {
        if (piece == CHESS_PIECE_WHITE_PAWN && to == chess_square_from_fr(position->ep, CHESS_RANK_6))
        {
            position->piece[chess_square_from_fr(position->ep, CHESS_RANK_5)] = CHESS_PIECE_NONE;
            ep = CHESS_UNMOVE_EP_CAPTURE;
        }
        else if (piece == CHESS_PIECE_BLACK_PAWN && to == chess_square_from_fr(position->ep, CHESS_RANK_3))
        {
            position->piece[chess_square_from_fr(position->ep, CHESS_RANK_4)] = CHESS_PIECE_NONE;
            ep = CHESS_UNMOVE_EP_CAPTURE;
        }
        else {
            ep = CHESS_UNMOVE_EP_AVAILABLE + position->ep;
        }

    }

    /* Update ep on a double pawn move */
    if (piece == CHESS_PIECE_WHITE_PAWN && to - from == 16)
        position->ep = chess_square_file(to);
    else if (piece == CHESS_PIECE_BLACK_PAWN && from - to == 16)
        position->ep = chess_square_file(to);
    else
        position->ep = CHESS_FILE_INVALID;

    /* Check if castling availability was lost */
    if (position->castle & (CHESS_CASTLE_STATE_WKQ))
    {
        if (from == CHESS_SQUARE_A1 || to == CHESS_SQUARE_A1)
            position->castle &= ~CHESS_CASTLE_STATE_WQ;

        if (from == CHESS_SQUARE_H1 || to == CHESS_SQUARE_H1)
            position->castle &= ~CHESS_CASTLE_STATE_WK;

        if (from == CHESS_SQUARE_E1 || to == CHESS_SQUARE_E1)
            position->castle &= ~(CHESS_CASTLE_STATE_WKQ);
    }
    if (position->castle & (CHESS_CASTLE_STATE_BKQ))
    {
        if (from == CHESS_SQUARE_A8 || to == CHESS_SQUARE_A8)
            position->castle &= ~CHESS_CASTLE_STATE_BQ;

        if (from == CHESS_SQUARE_H8 || to == CHESS_SQUARE_H8)
            position->castle &= ~CHESS_CASTLE_STATE_BK;

        if (from == CHESS_SQUARE_E8 || to == CHESS_SQUARE_E8)
            position->castle &= ~(CHESS_CASTLE_STATE_BKQ);
    }

    /* Update fifty counter only if a reversible move was played */
    if (piece == CHESS_PIECE_WHITE_PAWN || piece == CHESS_PIECE_BLACK_PAWN
        || captured != CHESS_UNMOVE_CAPTURED_NONE || castle != position->castle)
        position->fifty = 0;
    else
        position->fifty++;

    /* Update move counter */
    position->to_move = chess_color_other(position->to_move);
    if (position->to_move == CHESS_COLOR_WHITE)
        position->move_num++;

    return chess_unmove_make(from, to, captured,
        promote != CHESS_MOVE_PROMOTE_NONE, ep, castle, fifty);
}

static ChessPiece captured_piece(ChessUnmoveCaptured captured, ChessColor color)
{
    switch (captured)
    {
        case CHESS_UNMOVE_CAPTURED_NONE:
            return CHESS_PIECE_NONE;
        case CHESS_UNMOVE_CAPTURED_PAWN:
            return chess_piece_of_color(CHESS_PIECE_WHITE_PAWN, color);
        case CHESS_UNMOVE_CAPTURED_KNIGHT:
            return chess_piece_of_color(CHESS_PIECE_WHITE_KNIGHT, color);
        case CHESS_UNMOVE_CAPTURED_BISHOP:
            return chess_piece_of_color(CHESS_PIECE_WHITE_BISHOP, color);
        case CHESS_UNMOVE_CAPTURED_ROOK:
            return chess_piece_of_color(CHESS_PIECE_WHITE_ROOK, color);
        case CHESS_UNMOVE_CAPTURED_QUEEN:
            return chess_piece_of_color(CHESS_PIECE_WHITE_QUEEN, color);
        default:
            assert(0);
            return 0;
    }
}

void chess_position_undo_move(ChessPosition* position, ChessUnmove unmove)
{
    ChessSquare from = chess_unmove_from(unmove);
    ChessSquare to = chess_unmove_to(unmove);
    ChessUnmoveCaptured captured = chess_unmove_captured(unmove);
    ChessUnmoveEp ep = chess_unmove_ep(unmove);

    ChessPiece piece = position->piece[to];
    ChessColor other = position->to_move;
    ChessColor color = chess_color_other(other);
    ChessFile file;

    assert(color == chess_piece_color(piece));

    if (chess_unmove_promotion(unmove))
        piece = chess_piece_of_color(CHESS_PIECE_WHITE_PAWN, color);

    /* Unmove the piece */
    position->piece[from] = piece;
    position->piece[to] = captured_piece(captured, other);

    /* Handle castling */
    if (piece == CHESS_PIECE_WHITE_KING && from == CHESS_SQUARE_E1)
    {
        if (to == CHESS_SQUARE_G1)
        {
            position->piece[CHESS_SQUARE_F1] = CHESS_PIECE_NONE;
            position->piece[CHESS_SQUARE_H1] = CHESS_PIECE_WHITE_ROOK;
        }
        else if (to == CHESS_SQUARE_C1)
        {
            position->piece[CHESS_SQUARE_D1] = CHESS_PIECE_NONE;
            position->piece[CHESS_SQUARE_A1] = CHESS_PIECE_WHITE_ROOK;
        }
    }
    else if (piece == CHESS_PIECE_BLACK_KING && from == CHESS_SQUARE_E8)
    {
        if (to == CHESS_SQUARE_G8)
        {
            position->piece[CHESS_SQUARE_F8] = CHESS_PIECE_NONE;
            position->piece[CHESS_SQUARE_H8] = CHESS_PIECE_BLACK_ROOK;
        }
        else if (to == CHESS_SQUARE_C8)
        {
            position->piece[CHESS_SQUARE_D8] = CHESS_PIECE_NONE;
            position->piece[CHESS_SQUARE_A8] = CHESS_PIECE_BLACK_ROOK;
        }
    }
    position->castle = chess_unmove_castle(unmove);

    /* Handle ep */
    if (ep == CHESS_UNMOVE_EP_NONE)
    {
        position->ep = CHESS_FILE_INVALID;
    }
    else if (ep == CHESS_UNMOVE_EP_CAPTURE)
    {
        assert(piece == CHESS_PIECE_WHITE_PAWN || piece == CHESS_PIECE_BLACK_PAWN);

        /* Restore the captured pawn */
        file = chess_square_file(to);
        if (color == CHESS_COLOR_WHITE)
            position->piece[chess_square_from_fr(file, CHESS_RANK_5)] = CHESS_PIECE_BLACK_PAWN;
        else
            position->piece[chess_square_from_fr(file, CHESS_RANK_4)] = CHESS_PIECE_WHITE_PAWN;
        position->ep = file;
    }
    else
    {
        position->ep = ep - CHESS_UNMOVE_EP_AVAILABLE;
    }

    /* Update king positions */
    if (piece == CHESS_PIECE_WHITE_KING)
        position->wking = from;
    else if (piece == CHESS_PIECE_BLACK_KING)
        position->bking = from;

    /* Update move counters */
    position->fifty = chess_unmove_fifty(unmove);

    position->to_move = color;
    if (position->to_move == CHESS_COLOR_BLACK)
        position->move_num--;
}
