#include <stdio.h>
#include <string.h>

#include "fen.h"
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

    chess_pgn_tokenizer_consume(tokenizer); /* L_BRACKET */
    token = chess_pgn_tokenizer_next(tokenizer);
    if (token->type != CHESS_PGN_TOKEN_SYMBOL)
    {
        result = CHESS_PGN_LOAD_UNEXPECTED_TOKEN;
        goto error;
    }
    chess_string_assign(&tag, token->string.data);

    token = chess_pgn_tokenizer_next(tokenizer);
    if (token->type != CHESS_PGN_TOKEN_STRING)
    {
        result = CHESS_PGN_LOAD_UNEXPECTED_TOKEN;
        goto error;
    }
    chess_string_assign(&value, token->string.data);

    token = chess_pgn_tokenizer_next(tokenizer);
    if (token->type != CHESS_PGN_TOKEN_R_BRACKET)
    {
        result = CHESS_PGN_LOAD_UNEXPECTED_TOKEN;
        goto error;
    }

    chess_game_set_tag(game, tag.data, value.data);

error:
    chess_string_cleanup(&tag);
    chess_string_cleanup(&value);
    return result;
}

static ChessPgnLoadResult parse_move(ChessPgnTokenizer* tokenizer,
    const ChessPosition* position, ChessMove* move)
{
    ChessParseMoveResult result;
    const ChessPgnToken* token;

    token = chess_pgn_tokenizer_peek(tokenizer); /* SYMBOL */
    result = chess_parse_move(token->string.data, position, move);
    if (result != CHESS_PARSE_MOVE_OK)
        return CHESS_PGN_LOAD_ILLEGAL_MOVE;

    chess_pgn_tokenizer_consume(tokenizer);
    return CHESS_PGN_LOAD_OK;
}

static ChessPgnLoadResult parse_variation(ChessPgnTokenizer* tokenizer,
    const ChessPosition* initial_position, ChessVariation* root)
{
    const ChessPgnToken* token;
    ChessMove move;
    ChessUnmove unmove;
    ChessPgnLoadResult result;
    ChessPosition position;
    ChessVariation* variation = root;
 
    chess_position_copy(initial_position, &position);
    for (;;)
    {
        token = chess_pgn_tokenizer_peek(tokenizer);
        switch (token->type)
        {
            case CHESS_PGN_TOKEN_COMMENT:
                /* Skip comments for now */
                chess_pgn_tokenizer_consume(tokenizer);
                break;
            case CHESS_PGN_TOKEN_NUMBER:
                chess_pgn_tokenizer_consume(tokenizer);
                /* Move number may be followed by zero or more periods */
                while (chess_pgn_tokenizer_peek(tokenizer)->type == CHESS_PGN_TOKEN_PERIOD)
                    chess_pgn_tokenizer_consume(tokenizer);
                break;
            case CHESS_PGN_TOKEN_SYMBOL:
                result = parse_move(tokenizer, &position, &move);
                if (result != CHESS_PGN_LOAD_OK)
                    return result;

                unmove = chess_position_make_move(&position, move);
                variation = chess_variation_add_child(variation, move);
                break;
            case CHESS_PGN_TOKEN_NAG:
                if (variation == root)
                    return CHESS_PGN_LOAD_UNEXPECTED_TOKEN;

                chess_variation_add_annotation(variation, token->number);
                chess_pgn_tokenizer_consume(tokenizer);
                break;
            case CHESS_PGN_TOKEN_L_PARENTHESIS:
                if (variation == root)
                    return CHESS_PGN_LOAD_UNEXPECTED_TOKEN;

                chess_pgn_tokenizer_consume(tokenizer);

                /* Subvariation, back up a move and parse it */
                chess_position_undo_move(&position, unmove);
                result = parse_variation(tokenizer, &position, variation->parent);
                if (result != CHESS_PGN_LOAD_OK)
                    return result;

                chess_position_make_move(&position, move);

                token = chess_pgn_tokenizer_peek(tokenizer);
                if (token->type != CHESS_PGN_TOKEN_R_PARENTHESIS)
                    return CHESS_PGN_LOAD_UNEXPECTED_TOKEN;

                chess_pgn_tokenizer_consume(tokenizer); /* R_PARENTHESIS */
                break;
            default:
                /* Stop parsing variation on any other token */
                return CHESS_PGN_LOAD_OK;
        }
    }
}

static ChessPgnLoadResult parse_movetext(ChessPgnTokenizer* tokenizer, ChessGame* game)
{
    ChessPgnLoadResult result;
    const ChessPgnToken* token;
    ChessResult game_result;

    result = parse_variation(tokenizer, chess_game_initial_position(game), chess_game_root_variation(game));
    if (result != CHESS_PGN_LOAD_OK)
        return result;

    token = chess_pgn_tokenizer_peek(tokenizer);
    switch (token->type)
    {
        case CHESS_PGN_TOKEN_ASTERISK:
            chess_pgn_tokenizer_consume(tokenizer);
            game_result = CHESS_RESULT_IN_PROGRESS;
            break;
        case CHESS_PGN_TOKEN_ONE_ZERO:
            chess_pgn_tokenizer_consume(tokenizer);
            game_result = CHESS_RESULT_WHITE_WINS;
            break;
        case CHESS_PGN_TOKEN_ZERO_ONE:
            chess_pgn_tokenizer_consume(tokenizer);
            game_result = CHESS_RESULT_BLACK_WINS;
            break;
        case CHESS_PGN_TOKEN_HALF_HALF:
            chess_pgn_tokenizer_consume(tokenizer);
            game_result = CHESS_RESULT_DRAW;
            break;
        default:
            return CHESS_PGN_LOAD_UNEXPECTED_TOKEN;
    }
    chess_game_set_result(game, game_result);

    return CHESS_PGN_LOAD_OK;
}

static void check_setup_tag(ChessGame* game)
{
    ChessPosition position;
    const char* value;

    value = chess_game_tag_value(game, "SetUp");
    if (value == NULL || strcmp(value, "1") != 0)
        return;

    value = chess_game_tag_value(game, "FEN");
    if (value == NULL)
        return;

    chess_fen_load(value, &position);
    chess_game_set_initial_position(game, &position);
}

static ChessPgnLoadResult parse_game(ChessPgnTokenizer* tokenizer, ChessGame* game)
{
    const ChessPgnToken* token;
    ChessPgnLoadResult result;

    chess_game_reset(game);

    for (;;)
    {
        token = chess_pgn_tokenizer_peek(tokenizer);
        switch (token->type)
        {
            case CHESS_PGN_TOKEN_EOF:
                return CHESS_PGN_LOAD_EOF;
            case CHESS_PGN_TOKEN_L_BRACKET:
                result = parse_tag(tokenizer, game);
                if (result != CHESS_PGN_LOAD_OK)
                    return result;
                break;
            default:
                check_setup_tag(game);
                return parse_movetext(tokenizer, game);
        }
    }
}

ChessPgnLoadResult chess_pgn_load(ChessReader* reader, ChessGame* game)
{
    ChessPgnTokenizer tokenizer;
    ChessPgnLoadResult result;
    chess_pgn_tokenizer_init(&tokenizer, reader);
    result = parse_game(&tokenizer, game);
    chess_pgn_tokenizer_cleanup(&tokenizer);
    return result;
}

void chess_pgn_loader_init(ChessPgnLoader* loader, ChessReader* reader)
{
    loader->reader = reader;
    chess_pgn_tokenizer_init(&loader->tokenizer, reader);
}

void chess_pgn_loader_cleanup(ChessPgnLoader* loader)
{
    chess_pgn_tokenizer_cleanup(&loader->tokenizer);
}

ChessPgnLoadResult chess_pgn_loader_next(ChessPgnLoader* loader, ChessGame* game)
{
    const ChessPgnToken* token;

    for ((token = chess_pgn_tokenizer_peek(&loader->tokenizer));
         token->type != CHESS_PGN_TOKEN_L_BRACKET;
         (token = chess_pgn_tokenizer_peek(&loader->tokenizer)))
    {
        if (token->type == CHESS_PGN_TOKEN_EOF)
            return CHESS_PGN_LOAD_EOF;

        if (token->type == CHESS_PGN_TOKEN_ERROR)
            chess_reader_getc(loader->reader);

        chess_pgn_tokenizer_consume(&loader->tokenizer);
    }

    return parse_game(&loader->tokenizer, game);
}

const ChessPgnToken* chess_pgn_loader_last_token(ChessPgnLoader* loader)
{
    return chess_pgn_tokenizer_peek(&loader->tokenizer);
}
