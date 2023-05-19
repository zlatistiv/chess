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


int move_piece(const char ti, const char tj, const char fi, const char fj, const bool white) { // to" and "from" indeces (corresponfs to pi, pj, si, sj). i is the vertical index, j is the horizontal. Move validation will be implemented here (function should return a status code)
	static int move_counter = 0;

	if (cells[fi][fj] == &EMPTY) return -1;

	if (white) {
		if (cells[fi][fj] == &PAWN_B || cells[fi][fi] == &ROOK_B || cells[fi][fi] == &KNIGHT_B || cells[fi][fi] == &BISHOP_B || cells[fi][fi] == &QUEEN_B || cells[fi][fi] == &KING_B) return -1;
		if (cells[ti][tj] == &PAWN_W || cells[fi][fi] == &ROOK_W || cells[fi][fi] == &KNIGHT_W || cells[fi][fi] == &BISHOP_W || cells[fi][fi] == &QUEEN_W || cells[fi][fi] == &KING_W) return -1;

		if (cells[fi][fj] == &PAWN_W) {
			
		}
		if (cells[fi][fj] == &ROOK_W) {
			
		}
		if (cells[fi][fj] == &KNIGHT_W) {
			
		}
		if (cells[fi][fj] == &BISHOP_W) {
			
		}
		if (cells[fi][fj] == &QUEEN_W) {
			
		}
		if (cells[fi][fj] == &KING_W) {
			
		}
	}
	else {
		if (cells[fi][fj] == &PAWN_W || cells[fi][fi] == &ROOK_W || cells[fi][fi] == &KNIGHT_W || cells[fi][fi] == &BISHOP_W || cells[fi][fi] == &QUEEN_W || cells[fi][fi] == &KING_W) return -1;
		if (cells[ti][tj] == &PAWN_B || cells[fi][fi] == &ROOK_B || cells[fi][fi] == &KNIGHT_B || cells[fi][fi] == &BISHOP_B || cells[fi][fi] == &QUEEN_B || cells[fi][fi] == &KING_B) return -1;
	}

	


	cells[ti][tj] = cells[fi][fj];
	cells[fi][fj] = &EMPTY;

	move_counter++;

	return 0;
}

void move_piece_unsafe(const char ti, const char tj, const char fi, const char fj) { // This is used to move the other players pieces
	cells[ti][tj] = cells[fi][fj];
	cells[fi][fj] = &EMPTY;
}