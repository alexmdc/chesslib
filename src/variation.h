#ifndef CHESSLIB_VARIATION_H_
#define CHESSLIB_VARIATION_H_

typedef struct ChessVariation ChessVariation;

ChessVariation* chess_variation_new(ChessMove move);
void chess_variation_destroy(ChessVariation*);

ChessMove chess_variation_move(const ChessVariation*);
ChessString* chess_variation_comment(ChessVariation*);
ChessVariation* chess_variation_parent(ChessVariation*);
ChessVariation* chess_variation_first_child(ChessVariation*);
ChessVariation* chess_variation_left(ChessVariation*);
ChessVariation* chess_variation_right(ChessVariation*);

size_t chess_variation_length(const ChessVariation*);
ChessVariation* chess_variation_ply(ChessVariation*, size_t ply);

ChessVariation* chess_variation_add_child(ChessVariation*, ChessMove);
ChessVariation* chess_variation_add_sibling(ChessVariation*, ChessMove);

#endif /* CHESSLIB_VARIATION_H_ */
