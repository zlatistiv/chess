#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <termios.h> //termios, TCSANOW, ECHO, ICANON
#include <unistd.h> //STDIN_FILENO

#include <pthread.h>

#include "cli.h"
#include "board.h"

#define FIFONAMEMAX 16

static const char *status_str[] = {
	"\n## Waiting for the other player... ##",
	"\n## Your turn. ##",
	"\n## Bad move! Please try again. ##",
	"\n## Cannot move due to check condition. ##"
};

int clocks[2] = {600, 600};
Color color;
Status status;
Move selection;
bool selected;

static void update() {
	system("clear");
	printf("White: %d:%02d; Black: %d:%02d\n\n", clocks[WHITE] / 60, clocks[WHITE] % 60, clocks[BLACK] / 60, clocks[BLACK] % 60);
	print_board(&selection, color, selected);
	printf("%s\n", status_str[status]);

}

static void *update_clock(void *vargs) { // Periodically run to update the clock and re-display the screen
	while (1) {
		sleep(1);
		update();

		if (status == WAITING) clocks[!color]--;
		else clocks[color]--;
	}
}

static void name_fifo(char fifoname[], int game_id) {
	int i;
	for (i = 0; i < FIFONAMEMAX; i++) {
		fifoname[i] = '0' + game_id % 10;
		game_id /= 10;
		if (game_id == 0) {
			i++;
			break;
		}
	}
	fifoname[i] = '\0';
}

static void wait(const char fifoname[], Move *move) {
	int fifo_fd;
	fifo_fd = open(fifoname, O_RDONLY);
	char buf[4];
	read(fifo_fd, buf, sizeof(buf));
	close(fifo_fd);
	read_op_move(buf, !color);
}

int play(const bool is_white, const int game_id) {
	SETUP_SHELL();

	color = is_white;
	status = color;
	Pos marked = {0, 0};
	Pos pointed = {0, 0};
	selection.src = &marked;
	selection.dst = &pointed;
	char fifoname[FIFONAMEMAX];
	int fifo_fd;
	char c;
	int increment;
	selected = false;
	increment = color * 2 - 1;
	pthread_t clock_thread;
	
	name_fifo(fifoname, game_id);

	if (color == WHITE) {
		if (mkfifo(fifoname, 0600) == -1) {
			fprintf(stderr, "%s\n", "Error creating named pipe!");
			exit(1);
		}
		printf("Waiting for an oponent to join, Game ID: %d\n", game_id);
		fifo_fd = open(fifoname, O_RDONLY); // Execution will block here, until the other player opens the pipe for writing
		close(fifo_fd);
	}
	else {
		fifo_fd = open(fifoname, O_WRONLY); // Open the pipe to unblock the other players process
		close(fifo_fd);
	}

	// Starting the game...
	pthread_create(&clock_thread, NULL, update_clock, (void *)&clock_thread);
	initialize_board();

	while (true) {
		update();

		if (status == WAITING) { // Block until other player makes move
			wait(fifoname, &selection);
			status = MY_TURN;
			CLEAR_STDIN();
		}
		else {
			c = tolower(getchar());
			if (c == 'w') pointed.i+=increment;
			else if (c == 'a') pointed.j-=increment;
			else if (c == 's') pointed.i-=increment;
			else if (c == 'd') pointed.j+=increment;

			if (selected) {
				if (c == '\n') {
					if ((status = move_piece(&selection, color)) == 0) {
						update();
						fifo_fd = open(fifoname, O_WRONLY);
						char buf[4] = {selection.src->i, selection.src->j, selection.dst->i, selection.dst->j};
						write(fifo_fd, buf, sizeof(buf));
						close(fifo_fd);
					}

					selected = false;
				}
				else if (c == 'c') {
					selected = false;
				}
			}
			else if (c == '\n') {
				marked.i = pointed.i;
				marked.j = pointed.j;
				selected = true;
			}
		}
	}

	SHELL_BACK_TO_NORMAL();
	return 0;
}
