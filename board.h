#ifndef BOARD_H
#define BOARD_H


#define BOARD_SIZE 8

// Background color constants

#define BLACK "\033[37m\033[40m"
#define WHITE "\033[37m\033[44m"
#define DEFAULT "\033[37m\033[40m"

#define POINTED_BLACK "\033[32m\033[40m"
#define POINTED_WHITE "\033[32m\033[44m"

#define SELECTED_BLACK "\033[35m\033[40m"
#define SELECTED_WHITE "\033[35m\033[44m"

void initialize_board();
void print_board(const char, const char, const char, const char, const bool);
int move_piece(const char, const char, const char, const char, const bool);
void move_piece_unsafe(const char, const char, const char, const char);


#endif