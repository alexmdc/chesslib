#ifndef CHESSLIB_VARIATION_H_
#define CHESSLIB_VARIATION_H_

#include "move.h"
#include "cstring.h"

typedef unsigned char ChessAnnotation;

typedef struct ChessVariation ChessVariation;
struct ChessVariation
{
    ChessMove move;
    ChessString comment;
    ChessAnnotation annotations[4];
    ChessVariation* root;
    ChessVariation* parent;
    ChessVariation* first_child;
    ChessVariation* left;
    ChessVariation* right;
};

ChessVariation* chess_variation_new(void);
void chess_variation_destroy(ChessVariation*);

ChessBoolean chess_variation_is_root(const ChessVariation*);
size_t chess_variation_annotations(const ChessVariation*, ChessAnnotation*);

size_t chess_variation_length(const ChessVariation*);
size_t chess_variation_num_children(const ChessVariation*);
ChessVariation* chess_variation_ply(ChessVariation*, size_t ply);

void chess_variation_add_annotation(ChessVariation*, ChessAnnotation);
void chess_variation_remove_annotation(ChessVariation*, ChessAnnotation);

ChessVariation* chess_variation_add_child(ChessVariation*, ChessMove);
void chess_variation_attach_subvariation(ChessVariation*, ChessVariation*);
void chess_variation_truncate(ChessVariation*);
void chess_variation_delete(ChessVariation*);
void chess_variation_promote(ChessVariation*);

#endif /* CHESSLIB_VARIATION_H_ */
