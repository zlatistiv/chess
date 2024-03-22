#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "board.h"
#include "move_validation.h"


// This is the base string from which begin all the pieces; This is the black king
static const char *BASE = "\xe2\x99\x94";

// Here supplying the Player Color as index will provide you with the correct color scheme to use
static const char *POINTER_COLOR[] = {
	"\033[32m\033[40m",
	"\033[32m\033[44m"
};
static const char *SELECTION_COLOR[] = {
	"\033[35m\033[40m",
	"\033[35m\033[44m"
};
static const char *OP_POINTER_COLOR[] = {
	"\033[33m\033[40m",
	"\033[33m\033[44m"
};
static const char *OP_SELECTION_COLOR[] = {
	"\033[31m\033[40m",
	"\033[31m\033[44m"
};
static const char *BACKGROUND_COLOR[] = {
	"\033[37m\033[40m",
	"\033[37m\033[44m"
};


bool (*is_valid_move[13]) (const Move *, const Move *) = {
	is_valid_move_king_black,
	is_valid_move_queen,
	is_valid_move_rook,
	is_valid_move_bishop,
	is_valid_move_knight,
	is_valid_move_pawn_black,
	is_valid_move_king_black,
	is_valid_move_queen,
	is_valid_move_rook,
	is_valid_move_bishop,
	is_valid_move_knight,
	is_valid_move_pawn_white,
	is_valid_move_empty,
};


Piece board[BOARD_SIZE][BOARD_SIZE];
Move *move_history;

int move_counter;

Piece pieceat(const Pos *p) {
	return board[p->i][p->j];
}

void setpiece(const Pos *pos, const Piece p) {
	board[pos->i][pos->j] = p;
}

Piece pieceatindex(const int i, const int j) {
	return board[i][j];
}

int get_color(const Piece piece) {
	if (piece >= KING_B && piece <= PAWN_B) return BLACK;
	if (piece >= KING_W && piece <= PAWN_W) return WHITE;
	return -1; // This is for when the "Piece" is an empty square
}

void initialize_board() {
	int i, j;
	
	for (j = 0; j < BOARD_SIZE; j++) {
		board[1][j] =  PAWN_W;
		board[6][j] =  PAWN_B;
	}
	
	board[0][0] = ROOK_W;
	board[0][1] = KNIGHT_W;
	board[0][2] = BISHOP_W;
	board[0][3] = QUEEN_W;
	board[0][4] = KING_W;
	board[0][5] = BISHOP_W;
	board[0][6] = KNIGHT_W;
	board[0][7] = ROOK_W;

	board[7][0] = ROOK_B;
	board[7][1] = KNIGHT_B;
	board[7][2] = BISHOP_B;
	board[7][3] = QUEEN_B;
	board[7][4] = KING_B;
	board[7][5] = BISHOP_B;
	board[7][6] = KNIGHT_B;
	board[7][7] = ROOK_B;

	for (i = 2; i < BOARD_SIZE - 2; i++) {
		for (j = 0; j < BOARD_SIZE; j++) {
			board[i][j] =  EMPTY;
		}
	}

	move_counter = 0;
	move_history = malloc(HISTSIZE * sizeof(Move));
}

void print_board(const Move *move, const Move *op_move, const Color color, const bool selected) {
	Pos ind; // indeces
	int i, j;
	char buf[4];
	Color square_color;

	for (i = 0; i < BOARD_SIZE; i++) {
		if (color == WHITE) ind.i = BOARD_SIZE - i - 1;
		else ind.i = i;
		printf("%d|", ind.i + 1);
		
		for (j = 0; j < BOARD_SIZE; j++) {
			if (color == BLACK) ind.j = BOARD_SIZE - j - 1;
			else ind.j = j;
			
			if (board[ind.i][ind.j] != EMPTY) {
				strcpy(buf, BASE);
				buf[2] += board[ind.i][ind.j];
			}
			else strcpy(buf, " ");

			square_color = (ind.i + ind.j) % 2 != 0;

			if (selected && ind.i == move->src.i && ind.j == move->src.j) {
				printf("%s[", SELECTION_COLOR[square_color]);
				printf("%s%s", BACKGROUND_COLOR[square_color], buf);
				printf("%s]", SELECTION_COLOR[square_color]);
			}
			else if (ind.i == move->dst.i && ind.j == move->dst.j) {
				printf("%s[", POINTER_COLOR[square_color]);
				printf("%s%s", BACKGROUND_COLOR[square_color], buf);
				printf("%s]", POINTER_COLOR[square_color]);
			}
			else if (move_counter > color && ind.i == op_move->src.i && ind.j == op_move->src.j) {
				printf("%s[", OP_SELECTION_COLOR[square_color]);
				printf("%s%s", BACKGROUND_COLOR[square_color], buf);
				printf("%s]", OP_SELECTION_COLOR[square_color]);
			}
			else if (move_counter > color && ind.i == op_move->dst.i && ind.j == op_move->dst.j) {
				printf("%s[", OP_POINTER_COLOR[square_color]);
				printf("%s%s", BACKGROUND_COLOR[square_color], buf);
				printf("%s]", OP_POINTER_COLOR[square_color]);
			}
			else printf("%s %s ", BACKGROUND_COLOR[square_color], buf);
		}
		printf("%s\n", BACKGROUND_DEFAULT);
	}

	if (color == WHITE) {
		printf("%s\n", "   ______________________");
		printf("%s\n", "   a  b  c  d  e  f  g  h");
	}
	else {
		printf("%s\n", "   ______________________");
		printf("%s\n", "   h  g  f  e  d  c  b  a");
	}
}


static bool threatened(Pos* pos, const Color color, const Move *op_move) { // Checks whether any piece with color "color" threatens the position "pos"
	int i, j;
	Piece p;
	Move mv = {.src = {0, 0}, .dst = {pos->i, pos->j}};

	for (i = 0; i < BOARD_SIZE; i++) {
		for (j = 0; j < BOARD_SIZE; j++) {
			p = pieceatindex(i, j);
			mv.src.i = i;
			mv.src.j = j;
			if (get_color(p) == color) {
				if (is_valid_move[p](&mv, op_move)) return true;
			}
		}
	}

	return false;
}


static bool checked(const Color color, const Move *op_move) { // Checks whether the player with color "color" is checked
	int i, j;
	Pos pos;

	for (i = 0; i < BOARD_SIZE; i++) {
		for (j = 0; j < BOARD_SIZE; j++) {
			pos.i = i;
			pos.j = j;
			if ((color == WHITE && pieceat(&pos) == KING_W) || (color == BLACK && pieceat(&pos) == KING_B)) {
				return threatened(&pos, !color, op_move);
			}
		}
	}

	return false;
}


Status move_piece(const Move *move, const Move *op_move, const Color color) {
	Piece taker = board[move->src.i][move->src.j];
	Piece taken = board[move->dst.i][move->dst.j];

	if (get_color(taker) != color || get_color(taken) == color) return BAD_MOVE;
	if (!is_valid_move[taker](move, op_move)) return BAD_MOVE;
	
	board[move->dst.i][move->dst.j] = taker;
	board[move->src.i][move->src.j] = EMPTY;
	
	if (checked(color, op_move)) {
		board[move->src.i][move->src.j] = taker;
		board[move->dst.i][move->dst.j] = taken;
		return CHECKED;
	}
	
	memcpy(move_history + (move_counter++ % HISTSIZE), move, sizeof(Move)); // Store move in the history ring buffer
	
	return 0;
}
