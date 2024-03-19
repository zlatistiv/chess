#include <stdbool.h>
#include <stdlib.h>

#include "board.h"
#include "move_validation.h"

// my_move -> My current move that is being tested
// op_move oponents last move

bool is_valid_move_pawn_white(const Move *my_move, const Move *op_move) {
	if (my_move->src->i >= my_move->dst->i) return false;
	else if (my_move->dst->i - my_move->src->i == 1) {
		if (my_move->src->j == my_move->dst->j) {
			if (pieceat(my_move->dst) != EMPTY) return false;
		}
		else if (abs(my_move->dst->j - my_move->src->j) == 1) {
			if (my_move->src->i == 4 && pieceat(op_move->dst) == PAWN_B && op_move->src->i == 6 && op_move->dst->i == 4 && my_move->dst->j == op_move->dst->j) {  // En passant
				setpiece(op_move->dst, EMPTY);
				return true;
			}
			if (pieceat(my_move->dst) == EMPTY) return false;
		}
		else return false;
	}
	else if (my_move->src->i == 1) {
		if (my_move->dst->i - my_move->src->i != 2 || my_move->src->j != my_move->dst->j || pieceat(my_move->dst) != EMPTY || pieceatindex(my_move->dst->i - 1, my_move->dst->j) != EMPTY) return false;
	}
	else return false;

	return true;
}


bool is_valid_move_pawn_black(const Move *my_move, const Move *op_move) {
	if (my_move->src->i <= my_move->dst->i) return false;
	else if (my_move->src->i - my_move->dst->i == 1) {
		if (my_move->src->j == my_move->dst->j) {
			if (pieceat(my_move->dst) != EMPTY) return false;
		}
		else if (abs(my_move->dst->j - my_move->src->j) == 1) {
			if (my_move->src->i == 3 && pieceat(op_move->dst) == PAWN_W && op_move->src->i == 1 && op_move->dst->i == 3 && my_move->dst->j == op_move->dst->j) {  // En passant
				setpiece(op_move->dst, EMPTY);
				return true;
			}
			if (pieceat(my_move->dst) == EMPTY) return false;
		}
		else return false;
	}
	else if (my_move->src->i == 6) {
		if (my_move->src->i - my_move->dst->i != 2 || my_move->src->j != my_move->dst->j || pieceat(my_move->dst) != EMPTY || pieceatindex(my_move->dst->i + 1, my_move->dst->j) != EMPTY) return false;
	}
	else return false;

	return true;
}


bool is_valid_move_rook(const Move *my_move, const Move* op_move) {
	int index;

	if (my_move->dst->j != my_move->src->j) {
		if (my_move->dst->i != my_move->src->i) return false;
		if (my_move->dst->j > my_move->src->j) {
			for (index = my_move->src->j + 1; index < my_move->dst->j; index++) {
				if (pieceatindex(my_move->dst->i, index) != EMPTY) return false;
			}
		}
		else {
			for (index = my_move->dst->j + 1; index < my_move->src->j; index++) {
				if (pieceatindex(my_move->dst->i, index) != EMPTY) return false;
			}
		}
	}
	else {
		if (my_move->dst->i > my_move->src->i) {
			for (index = my_move->src->i + 1; index < my_move->dst->i; index++) {
				if (pieceatindex(index, my_move->dst->j) != EMPTY) return false;
			}
		}
		else {
			for (index = my_move->dst->i + 1; index < my_move->src->i; index++) {
				if (pieceatindex(index, my_move->dst->j) != EMPTY) return false;
			}
		}
	}

	return true;
}


bool is_valid_move_knight(const Move *my_move, const Move *op_move) {
	return abs(my_move->src->i - my_move->dst->i) + abs(my_move->src->j - my_move->dst->j) == 3 && my_move->src->i != my_move->dst->i && my_move->src->j != my_move->dst->j;
}

bool is_valid_move_bishop(const Move *my_move, const Move *op_move) {
	int index;

	if (abs(my_move->dst->i - my_move->src->i) != abs(my_move->dst->j - my_move->src->j)) return false;
	else {
		if (my_move->dst->i > my_move->src->i) {
			if (my_move->dst->j > my_move->src->j) {
				for (index = 1; index < my_move->dst->i - my_move->src->i; index++) {
					if (pieceatindex(my_move->src->i + index, my_move->src->j + index) != EMPTY) return false;
				}
			}
			else if (my_move->dst->j < my_move->src->j) {
				for (index = 1; index < my_move->dst->i - my_move->src->i; index++) {
					if (pieceatindex(my_move->src->i + index, my_move->src->j - index) != EMPTY) return false;
				}
			}
			else return false;
		}
		else if (my_move->dst->i < my_move->src->i) {
			if (my_move->dst->j > my_move->src->j) {
				for (index = 1; index < my_move->src->i - my_move->dst->i; index++) {
					if (pieceatindex(my_move->src->i - index, my_move->src->j + index) != EMPTY) return false;
				}
			}
			else if (my_move->dst->j < my_move->src->j) {
				for (index = 1; index < my_move->src->i - my_move->dst->i; index++) {
					if (pieceatindex(my_move->src->i - index, my_move->src->j - index) != EMPTY) return false;
				}
			}
			else return false;
		}
		else return false;
	}

	return true;
}

bool is_valid_move_queen(const Move *my_move, const Move *op_move) {
	return is_valid_move_rook(my_move, op_move) || is_valid_move_bishop(my_move, op_move);
}

bool is_valid_move_king_white(const Move *my_move, const Move *op_move) {
	if (abs(my_move->dst->i - my_move->src->i) > 1 || abs(my_move->dst->j - my_move->src->j) > 1) return false;

	return true;
}

bool is_valid_move_king_black(const Move *my_move, const Move *op_move) {
	if (abs(my_move->dst->i - my_move->src->i) > 1 || abs(my_move->dst->j - my_move->src->j) > 1) return false;

	return true;
}

bool is_valid_move_empty(const Move * my_move, const Move *op_move) {
	return false;
}
