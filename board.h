#ifndef BOARD_H
#define BOARD_H


#define BACKGROUND_DEFAULT "\033[37m\033[40m"

#define BOARD_SIZE 8

// This is used for the player color
typedef enum {
	BLACK = 0,
	WHITE = 1
} Color;

// This struct is a position on the board; i is the vertical index, j is the horizontal index
typedef struct {
	unsigned char i:3;
	unsigned char j:3;
} Pos;

// This is the structure of a move; It contains the source and destination positions
typedef struct {
	Pos *src;
	Pos *dst;
} Move;

// This enumerates the pieces; When you add each one to the character at index [2] of the BASE string you get the string of that piece
typedef enum {
	KING_B,
	QUEEN_B,
	ROOK_B,
	BISHOP_B,
	KNIGHT_B,
	PAWN_B,
	KING_W,
	QUEEN_W,
	ROOK_W,
	BISHOP_W,
	KNIGHT_W,
	PAWN_W,
	EMPTY
} Piece;

typedef enum {
	WAITING,
	MY_TURN,
	BAD_MOVE,
	CHECKED
} Status;


void initialize_board();

void print_board(const Move *, const Color, const bool selected);

Status move_piece(const Move *, const Color);

Piece pieceat(const Pos *);

Piece pieceatindex(const char i, const char j);

void read_op_move(char buf[4], const Color color);

#endif