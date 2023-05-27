#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "board.h"


static char** cells[BOARD_SIZE][BOARD_SIZE];

static char* PAWN_W = "\xe2\x99\x9f";
static char* ROOK_W = "\xe2\x99\x9c";
static char* KNIGHT_W = "\xe2\x99\x9e";
static char* BISHOP_W = "\xe2\x99\x9d";
static char* QUEEN_W = "\xe2\x99\x9b";
static char* KING_W = "\xe2\x99\x9a";

// Black pieces
static char* PAWN_B = "\xe2\x99\x99";
static char* ROOK_B = "\xe2\x99\x96";
static char* KNIGHT_B = "\xe2\x99\x98";
static char* BISHOP_B = "\xe2\x99\x97";
static char* QUEEN_B = "\xe2\x99\x95";
static char* KING_B = "\xe2\x99\x94";

static char* EMPTY = " ";


static bool is_valid_move_pawn_white(const char to_i, const char to_j, const char from_i, const char from_j);
static bool is_valid_move_pawn_black(const char to_i, const char to_j, const char from_i, const char from_j);
static bool is_valid_move_rook(const char to_i, const char to_j, const char from_i, const char from_j);
static bool is_valid_move_knight(const char to_i, const char to_j, const char from_i, const char from_j);
static bool is_valid_move_bishop(const char to_i, const char to_j, const char from_i, const char from_j);
static bool is_valid_move_queen(const char to_i, const char to_j, const char from_i, const char from_j);
static bool is_valid_move_king(const char to_i, const char to_j, const char from_i, const char from_j);
static bool is_valid_move(const char to_i, const char to_j, const char from_i, const char from_j, const bool white);
static bool checked(const bool white);



void initialize_board() {
	int i, j;

	for (j = 0; j < BOARD_SIZE; j++) {
		cells[1][j] = &PAWN_W;
		cells[6][j] = &PAWN_B;
	}

	cells[0][0] = &ROOK_W;
	cells[0][1] = &KNIGHT_W;
	cells[0][1] = &KNIGHT_W;
	cells[0][2] = &BISHOP_W;
	cells[0][3] = &QUEEN_W;
	cells[0][4] = &KING_W;
	cells[0][5] = &BISHOP_W;
	cells[0][6] = &KNIGHT_W;
	cells[0][7] = &ROOK_W;

	cells[7][0] = &ROOK_B;
	cells[7][1] = &KNIGHT_B;
	cells[7][2] = &BISHOP_B;
	cells[7][3] = &QUEEN_B;
	cells[7][4] = &KING_B;
	cells[7][5] = &BISHOP_B;
	cells[7][6] = &KNIGHT_B;
	cells[7][7] = &ROOK_B;

	for (i = 2; i < BOARD_SIZE - 2; i++) {
		for (j = 0; j < BOARD_SIZE; j++) {
			cells[i][j] = &EMPTY;
		}
	}
}

void print_board_pieces(const char i, const char j, const char pi, const char pj, const char si, const char sj) {
	if (i == si && j == sj) { // selected case
		if ((i + j) % 2 == 0) {
			printf("%s[", SELECTED_BLACK);
			printf("%s%s", BLACK, *cells[i][j]);
			printf("%s]", SELECTED_BLACK);
		}
		else {
			printf("%s[", SELECTED_WHITE);
			printf("%s%s", WHITE, *cells[i][j]);
			printf("%s]", SELECTED_WHITE);
		}
	}
	else if (i == pi && j == pj) { // pointed case
		if ((i + j) % 2 == 0) {
			printf("%s[", POINTED_BLACK);
			printf("%s%s", BLACK, *cells[i][j]);
			printf("%s]", POINTED_BLACK);
		}
		else {
			printf("%s[", POINTED_WHITE);
			printf("%s%s", WHITE, *cells[i][j]);
			printf("%s]", POINTED_WHITE);
		}
	}
	else { // default case
		if ((i + j) % 2 == 0) printf("%s %s ", BLACK, *cells[i][j]);
		else printf("%s %s ", WHITE, *cells[i][j]);
	}
}


void print_board(const char pi, const char pj, const char si, const char sj, const bool flipped) { // pointed and selected indeces
	int i, j;

	if (!flipped) {
		for (i = BOARD_SIZE - 1; i >= 0; i--) {
			printf("%d|", i + 1);
			
			for (j = 0; j < BOARD_SIZE; j++) {
				print_board_pieces(i, j, pi, pj, si, sj);
			}
			printf("%s\n", DEFAULT);
		}
		printf("%s\n", "   ______________________");
		printf("%s\n", "   a  b  c  d  e  f  g  h");
	}
	else {
		for (i = 0; i < BOARD_SIZE; i++) {
			printf("%d|", i + 1);
		
			for (j = BOARD_SIZE - 1; j >= 0; j--) {
				print_board_pieces(i, j, pi, pj, si, sj);
			}
			printf("%s\n", DEFAULT);
		}
		printf("%s\n", "   ______________________");
		printf("%s\n", "   h  g  f  e  d  c  b  a");
	}
}


int move_piece(const char to_i, const char to_j, const char from_i, const char from_j, const bool white) { // to" and "from" indeces (corresponfs to pi, pj, si, sj). i is the vertical index, j is the horizontal. Move validation will be implemented here (function should return a status code)
	static int move_counter = 0;
	static int king_move_counter = 0;
	char **tmp;

	if (!is_valid_move(to_i, to_j, from_i, from_j, white)) return -1;
	
	if (checked(white)) {
		tmp = cells[to_i][to_j];
		cells[to_i][to_j] = cells[from_i][from_j];
		cells[from_i][from_j] = &EMPTY;

		if (checked(white)) {
			cells[from_i][from_j] = cells[to_i][to_j];
			cells[to_i][to_j] = tmp;
			return -1;
		}
		else {
			move_counter++;
			return 0;
		}
	}
	
	tmp = cells[to_i][to_j];
	cells[to_i][to_j] = cells[from_i][from_j];
	cells[from_i][from_j] = &EMPTY;

	if (checked(white)) {
		cells[from_i][from_j] = cells[to_i][to_j];
		cells[to_i][to_j] = tmp;
		return -1;
	}

	move_counter++;

	return 0;
}

void move_piece_unsafe(const char to_i, const char to_j, const char from_i, const char from_j) { // This is used to move the other players pieces
	cells[to_i][to_j] = cells[from_i][from_j];
	cells[from_i][from_j] = &EMPTY;
}


// Below code is for move validation

static bool is_valid_move_pawn_white(const char to_i, const char to_j, const char from_i, const char from_j) {
	if (from_i >= to_i) return false;
	else if (to_i - from_i == 1) {
		if (from_j == to_j) {
			if (cells[to_i][to_j] != &EMPTY) return false;
		}
		else if (abs(to_j - from_j) == 1) {
			if (cells[to_i][to_j] == &EMPTY) return false;
		}
		else return false;
	}
	else if (from_i == 1) {
		if (to_i - from_i != 2 || from_j != to_j) return false;
	}
	else return false;

	return true;
}

static bool is_valid_move_pawn_black(const char to_i, const char to_j, const char from_i, const char from_j) {
	if (to_i >= from_i) return false;
	else if (from_i - to_i == 1) {
		if (from_j == to_j) {
			if (cells[to_i][to_j] != &EMPTY) return false;
		}
		else if (abs(to_j - from_j) == 1) {
			if (cells[to_i][to_j] == &EMPTY) return false;
		}
		else return false;
	}
	else if (from_i == 6) {
		if (from_i - to_i != 2 || from_j != to_j) return false;
	}
	else return false;

	return true;
}

static bool is_valid_move_rook(const char to_i, const char to_j, const char from_i, const char from_j) {
	int index;

	if (to_i == from_i) {
		if (to_j > from_j) {
			for (index = from_j + 1; index < to_j; index++) {
				if (cells[to_i][index] != &EMPTY) return false;
			}
		}
		else if (to_j < from_j) {
			for (index = to_j + 1; index < from_j; index++) {
				if (cells[to_i][index] != &EMPTY) return false;
			}
		}
		else return false;
	}
	else if (to_j == from_j) {
		if (to_i > from_i) {
			for (index = from_i + 1; index < to_i; index++) {
				if (cells[index][to_j] != &EMPTY) return false;
			}
		}
		else if (to_i < from_i) {
			for (index = to_i + 1; index < from_i; index++) {
				if (cells[index][to_j] != &EMPTY) return false;
			}
		}
		else return false;
	}
	else return false;

	return true;
}

static bool is_valid_move_knight(const char to_i, const char to_j, const char from_i, const char from_j) {
	return abs(from_i - to_i) + abs(from_j - to_j) == 3 && from_i != to_i && from_j != to_j;
}

static bool is_valid_move_bishop(const char to_i, const char to_j, const char from_i, const char from_j) {
	int index;

	if (abs(to_i - from_i) != abs(to_j - from_j)) return false;
	else {
		if (to_i > from_i) {
			if (to_j > from_j) {
				for (index = 1; index < to_i - from_i; index++) {
					if (cells[from_i + index][from_j + index] != &EMPTY) return false;
				}
			}
			else if (to_j < from_j) {
				for (index = 1; index < to_i - from_i; index++) {
					if (cells[from_i + index][from_j - index] != &EMPTY) return false;
				}
			}
			else return false;
		}
		else if (to_i < from_i) {
			if (to_j > from_j) {
				for (index = 1; index < from_i - to_i; index++) {
					if (cells[from_i - index][from_j + index] != &EMPTY) return false;
				}
			}
			else if (to_j < from_j) {
				for (index = 1; index < from_i - to_i; index++) {
					if (cells[from_i - index][from_j - index] != &EMPTY) return false;
				}
			}
			else return false;
		}
		else return false;
	}

	return true;
}

static bool is_valid_move_queen(const char to_i, const char to_j, const char from_i, const char from_j) {
	return is_valid_move_rook(to_i, to_j, from_i, from_j) || is_valid_move_bishop(to_i, to_j, from_i, from_j);
}

static bool is_valid_move_king(const char to_i, const char to_j, const char from_i, const char from_j) {
	return abs(from_i - to_i) <= 1 && abs(from_j - to_j) <= 1;
}

static bool is_valid_move(const char to_i, const char to_j, const char from_i, const char from_j, const bool white) {
	if (cells[from_i][from_j] == &EMPTY) return false;

	if (white) {
		if (cells[from_i][from_j] == &PAWN_B || cells[from_i][from_j] == &ROOK_B || cells[from_i][from_j] == &KNIGHT_B || cells[from_i][from_j] == &BISHOP_B || cells[from_i][from_j] == &QUEEN_B || cells[from_i][from_j] == &KING_B) return false;
		if (cells[to_i][to_j] == &PAWN_W || cells[to_i][to_j] == &ROOK_W || cells[to_i][to_j] == &KNIGHT_W || cells[to_i][to_j] == &BISHOP_W || cells[to_i][to_j] == &QUEEN_W || cells[to_i][to_j] == &KING_W) return false;

		if (cells[from_i][from_j] == &PAWN_W) {
			if (!is_valid_move_pawn_white(to_i, to_j, from_i, from_j)) return false;
		}
		else if (cells[from_i][from_j] == &ROOK_W) {
			if (!is_valid_move_rook(to_i, to_j, from_i, from_j)) return false;
		}
		else if (cells[from_i][from_j] == &KNIGHT_W) {
			if (!is_valid_move_knight(to_i, to_j, from_i, from_j)) return false;
		}
		else if (cells[from_i][from_j] == &BISHOP_W) {
			if (!is_valid_move_bishop(to_i, to_j, from_i, from_j)) return false;
		}
		else if (cells[from_i][from_j] == &QUEEN_W) {
			if (!is_valid_move_queen(to_i, to_j, from_i, from_j)) return false;
		}
		else if (cells[from_i][from_j] == &KING_W) {
			if (!is_valid_move_king(to_i, to_j, from_i, from_j)) return false;
		}
	}
	else {
		if (cells[from_i][from_j] == &PAWN_W || cells[from_i][from_j] == &ROOK_W || cells[from_i][from_j] == &KNIGHT_W || cells[from_i][from_j] == &BISHOP_W || cells[from_i][from_j] == &QUEEN_W || cells[from_i][from_j] == &KING_W) return false;
		if (cells[to_i][to_j] == &PAWN_B || cells[to_i][to_j] == &ROOK_B || cells[to_i][to_j] == &KNIGHT_B || cells[to_i][to_j] == &BISHOP_B || cells[to_i][to_j] == &QUEEN_B || cells[to_i][to_j] == &KING_B) return false;

		if (cells[from_i][from_j] == &PAWN_B) {
			if (!is_valid_move_pawn_black(to_i, to_j, from_i, from_j)) return false;
		}
		else if (cells[from_i][from_j] == &ROOK_B) {
			if (!is_valid_move_rook(to_i, to_j, from_i, from_j)) return false;
		}
		else if (cells[from_i][from_j] == &KNIGHT_B) {
			if (!is_valid_move_knight(to_i, to_j, from_i, from_j)) return false;
		}
		else if (cells[from_i][from_j] == &BISHOP_B) {
			if (!is_valid_move_bishop(to_i, to_j, from_i, from_j)) return false;
		}
		else if (cells[from_i][from_j] == &QUEEN_B) {
			if (!is_valid_move_queen(to_i, to_j, from_i, from_j)) return false;
		}
		else if (cells[from_i][from_j] == &KING_B) {
			if (!is_valid_move_king(to_i, to_j, from_i, from_j)) return false;
		}
	}

	return true;
}

static bool checked(const bool white) {
	int king_i, king_j, i, j;

	if (white) {
		for (king_i = 0; king_i < BOARD_SIZE; king_i++) {
			for (king_j = 0; king_j < BOARD_SIZE; king_j++) {
				if (cells[king_i][king_j] == &KING_W) goto found_king_white;
			}
		}
		found_king_white:

		for (i = 0; i < BOARD_SIZE; i++) {
			for (j = 0; j < BOARD_SIZE; j++) {
				if (cells[i][j] == &PAWN_B) {
					if (is_valid_move_pawn_black(king_i, king_j, i, j)) return true;
				}
				else if (cells[i][j] == &ROOK_B) {
					if (is_valid_move_rook(king_i, king_j, i, j)) return true;
				}
				else if (cells[i][j] == &KNIGHT_B) {
					if (is_valid_move_knight(king_i, king_j, i, j)) return true;
				}
				else if (cells[i][j] == &BISHOP_B) {
					if (is_valid_move_bishop(king_i, king_j, i, j)) return true;
				}
				else if (cells[i][j] == &QUEEN_B) {
					if (is_valid_move_queen(king_i, king_j, i, j)) return true;
				}
				else if (cells[i][j] == &KING_B) {
					if (is_valid_move_king(king_i, king_j, i, j)) return true;
				}
			}
		}
	}
	else {
		for (king_i = 0; king_i < BOARD_SIZE; king_i++) {
			for (king_j = 0; king_j < BOARD_SIZE; king_j++) {
				if (cells[king_i][king_j] == &KING_B) goto found_king_black;
			}
		}
		found_king_black:

		for (i = 0; i < BOARD_SIZE; i++) {
			for (j = 0; j < BOARD_SIZE; j++) {
				if (cells[i][j] == &PAWN_W) {
					if (is_valid_move_pawn_white(king_i, king_j, i, j)) return true;
				}
				else if (cells[i][j] == &ROOK_W) {
					if (is_valid_move_rook(king_i, king_j, i, j)) return true;
				}
				else if (cells[i][j] == &KNIGHT_W) {
					if (is_valid_move_knight(king_i, king_j, i, j)) return true;
				}
				else if (cells[i][j] == &BISHOP_W) {
					if (is_valid_move_bishop(king_i, king_j, i, j)) return true;
				}
				else if (cells[i][j] == &QUEEN_W) {
					if (is_valid_move_queen(king_i, king_j, i, j)) return true;
				}
				else if (cells[i][j] == &KING_W) {
					if (is_valid_move_king(king_i, king_j, i, j)) return true;
				}
			}
		}
	}

	return false;
}