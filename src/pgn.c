#include <stdio.h>
#include <string.h>

#include "pgn.h"
#include "pgn-tokenizer.h"
#include "parse.h"
#include "print.h"

static void append_tag(const char* name, const char* value, ChessWriter* writer)
{
    chess_writer_write_char(writer, '[');
    chess_writer_write_string(writer, name);
    chess_writer_write_string(writer, " \"");
    chess_writer_write_string(writer, value);
    chess_writer_write_string(writer, "\"]\n");
}

void chess_pgn_save(const ChessGame* game, ChessWriter* writer)
{
    ChessGameTagIterator iter = chess_game_get_tag_iterator((ChessGame*)game);

    while (chess_game_tag_iterator_next(&iter))
    {
        append_tag(chess_game_tag_iterator_name(&iter),
            chess_game_tag_iterator_value(&iter), writer);
    }
    chess_writer_write_char(writer, '\n');

    chess_print_game_moves(game, writer);
    chess_writer_write_char(writer, '\n');
}

static ChessPgnLoadResult parse_tag(ChessPgnTokenizer* tokenizer, ChessGame* game)
{
    const ChessPgnToken* token;
    ChessString tag, value;
    ChessPgnLoadResult result = CHESS_PGN_LOAD_OK;

    chess_string_init(&tag);
    chess_string_init(&value);

    token = chess_pgn_tokenizer_next(tokenizer); /* L_BRACKET */
    token = chess_pgn_tokenizer_next(tokenizer);
    if (token->type != CHESS_PGN_TOKEN_SYMBOL)
    {
        result = CHESS_PGN_LOAD_UNEXPECTED_TOKEN;
        goto error;
    }
    chess_string_assign(&tag, chess_string_data(&token->data.string));

    token = chess_pgn_tokenizer_next(tokenizer);
    if (token->type != CHESS_PGN_TOKEN_STRING)
    {
        result = CHESS_PGN_LOAD_UNEXPECTED_TOKEN;
        goto error;
    }
    chess_string_assign(&value, chess_string_data(&token->data.string));

    token = chess_pgn_tokenizer_next(tokenizer);
    if (token->type != CHESS_PGN_TOKEN_R_BRACKET)
    {
        result = CHESS_PGN_LOAD_UNEXPECTED_TOKEN;
        goto error;
    }

    chess_game_set_tag(game, chess_string_data(&tag), chess_string_data(&value));

error:
    chess_string_cleanup(&tag);
    chess_string_cleanup(&value);
    return result;
}

static ChessPgnLoadResult parse_move(ChessPgnTokenizer* tokenizer,
    const ChessPosition* position, ChessMove* move)
{
    ChessParseResult result;
    const ChessPgnToken* token;

    token = chess_pgn_tokenizer_peek(tokenizer); /* SYMBOL */
    result = chess_parse_move(chess_string_data(&token->data.string), position, move);
    if (result != CHESS_PARSE_OK)
        return CHESS_PGN_LOAD_ILLEGAL_MOVE;

    chess_pgn_tokenizer_next(tokenizer);
    return CHESS_PGN_LOAD_OK;
}

static ChessPgnLoadResult parse_variation(ChessPgnTokenizer* tokenizer,
    const ChessPosition* position, ChessVariation** variation)
{
    const ChessPgnToken* token;
    ChessMove move;
    ChessUnmove unmove;
    ChessPgnLoadResult result = CHESS_PGN_LOAD_OK;
    ChessPosition* current_position = chess_position_clone(position);
    ChessVariation* current_variation = chess_variation_new();
    ChessVariation* subvariation;

    while (result == CHESS_PGN_LOAD_OK)
    {
        token = chess_pgn_tokenizer_peek(tokenizer);
        switch (token->type)
        {
            case CHESS_PGN_TOKEN_COMMENT:
                /* Skip comments for now */
                chess_pgn_tokenizer_next(tokenizer);
                break;
            case CHESS_PGN_TOKEN_NUMBER:
                chess_pgn_tokenizer_next(tokenizer);
                /* Move number may be followed by zero or more periods */
                while (chess_pgn_tokenizer_peek(tokenizer)->type == CHESS_PGN_TOKEN_PERIOD)
                    chess_pgn_tokenizer_next(tokenizer);
                break;
            case CHESS_PGN_TOKEN_SYMBOL:
                result = parse_move(tokenizer, current_position, &move);
                if (result != CHESS_PGN_LOAD_OK)
                    break;

                unmove = chess_position_make_move(current_position, move);
                current_variation = chess_variation_add_child(current_variation, move);
                break;
            case CHESS_PGN_TOKEN_NAG:
                if (chess_variation_is_root(current_variation))
                {
                    result = CHESS_PGN_LOAD_UNEXPECTED_TOKEN;
                    break;
                }

                chess_variation_add_annotation(current_variation, token->data.number);
                chess_pgn_tokenizer_next(tokenizer);
                break;
            case CHESS_PGN_TOKEN_L_PARENTHESIS:
                if (chess_variation_is_root(current_variation))
                {
                    result = CHESS_PGN_LOAD_UNEXPECTED_TOKEN;
                    break;
                }
                chess_pgn_tokenizer_next(tokenizer);

                /* Subvariation, back up a move and parse it */
                chess_position_undo_move(current_position, unmove);
                result = parse_variation(tokenizer, current_position, &subvariation);
                if (result != CHESS_PGN_LOAD_OK)
                    break;

                chess_position_make_move(current_position, move);

                token = chess_pgn_tokenizer_peek(tokenizer);
                if (token->type != CHESS_PGN_TOKEN_R_PARENTHESIS)
                {
                    result = CHESS_PGN_LOAD_UNEXPECTED_TOKEN;
                    chess_variation_destroy(subvariation);
                    break;
                }

                chess_pgn_tokenizer_next(tokenizer); /* R_PARENTHESIS */
                chess_variation_attach_subvariation(
                    chess_variation_parent(current_variation), subvariation);
                break;
            default:
                /* Stop parsing on unexpected token */
                result = CHESS_PGN_LOAD_UNEXPECTED_TOKEN;
                break;
        }
    }

    if (result == CHESS_PGN_LOAD_UNEXPECTED_TOKEN)
    {
        /* Rewind back to the head of the variation */
        while (current_variation != NULL && chess_variation_parent(current_variation))
            current_variation = chess_variation_parent(current_variation);
        *variation = current_variation;
        result = CHESS_PGN_LOAD_OK;
    }

    chess_position_destroy(current_position);
    return result;
}

static ChessPgnLoadResult parse_movetext(ChessPgnTokenizer* tokenizer,
    ChessVariation** variation, ChessResult* game_result)
{
    const ChessPgnToken* token;
    ChessPgnLoadResult result = CHESS_PGN_LOAD_OK;
    ChessPosition* position;

    position = chess_position_new();
    chess_position_init(position);
    parse_variation(tokenizer, position, variation);
    chess_position_destroy(position);

    token = chess_pgn_tokenizer_peek(tokenizer);
    switch (token->type)
    {
        case CHESS_PGN_TOKEN_ASTERISK:
            chess_pgn_tokenizer_next(tokenizer);
            *game_result = CHESS_RESULT_IN_PROGRESS;
            break;
        case CHESS_PGN_TOKEN_ONE_ZERO:
            chess_pgn_tokenizer_next(tokenizer);
            *game_result = CHESS_RESULT_WHITE_WINS;
            break;
        case CHESS_PGN_TOKEN_ZERO_ONE:
            chess_pgn_tokenizer_next(tokenizer);
            *game_result = CHESS_RESULT_BLACK_WINS;
            break;
        case CHESS_PGN_TOKEN_HALF_HALF:
            chess_pgn_tokenizer_next(tokenizer);
            *game_result = CHESS_RESULT_DRAW;
            break;
        default:
            result = CHESS_PGN_LOAD_UNEXPECTED_TOKEN;
            break;
    }

    return result;
}

static ChessPgnLoadResult parse_game(ChessPgnTokenizer* tokenizer, ChessGame* game)
{
    const ChessPgnToken* token;
    ChessVariation* variation;
    ChessResult game_result;
    ChessPgnLoadResult result;

    for (;;)
    {
        token = chess_pgn_tokenizer_peek(tokenizer);
        switch (token->type)
        {
            case CHESS_PGN_TOKEN_L_BRACKET:
                result = parse_tag(tokenizer, game);
                if (result != CHESS_PGN_LOAD_OK)
                    return result;
                break;
            default:
                result = parse_movetext(tokenizer, &variation, &game_result);
                if (result == CHESS_PGN_LOAD_OK)
                {
                    chess_game_set_root_variation(game, variation);
                    chess_game_set_result(game, game_result);
                }
                return result;
        }
    }
}

ChessPgnLoadResult chess_pgn_load(ChessReader* reader, ChessGame* game)
{
    ChessPgnTokenizer* tokenizer = chess_pgn_tokenizer_new(reader);
    ChessPgnLoadResult result = parse_game(tokenizer, game);
    chess_pgn_tokenizer_destroy(tokenizer);
    return result;
}
