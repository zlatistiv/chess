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

#define BASEDIR "/tmp/chess/"
#define FIFONAME "fifo"

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
	char *dirname;
	char *fifoname;
	int fifo_fd;
	char c;
	int increment;
	selected = false;
	increment = color * 2 - 1;
	pthread_t clock_thread;
	
	// Name the fifo and the path to it
	int i, j;
	int n = 10; // This is the max amount of digits that will be taken into account from the value of "game_id". If it has less than that they will be filled with zeroes
	int gid = game_id;

	dirname = malloc(strlen(BASEDIR) + n + 2);
	strcpy(dirname, BASEDIR);
	i = strlen(BASEDIR);
	
	for (j = 0; j < 10; j++) {
		*(dirname + i + j) = '0' + gid % 10;
		gid /= 10;
	}
	dirname[i + j] = '/';
	dirname[i + j + 1] = '\0';

	fifoname = malloc(strlen(dirname) + strlen(FIFONAME));
	strcpy(fifoname, dirname);
	strcpy(fifoname + strlen(dirname), FIFONAME);

	if (color == WHITE) {
		mkdir(BASEDIR, 0777 | S_ISVTX); // World writable as well as sticky bit.
		mkdir(dirname, 0755);
		chdir(dirname);
		umask(0000);
		if (mkfifo(FIFONAME, 0666) == -1) {
			fprintf(stderr, "%s\n", "Error creating named pipe!");
			exit(1);
		}
		printf("Waiting for an oponent to join, Game ID: %d\n", game_id);
		fifo_fd = open(FIFONAME, O_RDONLY); // Execution will block here, until the other player opens the pipe for writing

		close(fifo_fd);
	}
	else {
		chdir(dirname);
		fifo_fd = open(FIFONAME, O_WRONLY); // Open the pipe to unblock the other players process
		if (fifo_fd == -1) {
			fprintf(stderr, "Invalid game ID!\n");
			exit(1);
		}
		close(fifo_fd);
	}

	// Starting the game...
	pthread_create(&clock_thread, NULL, update_clock, (void *)&clock_thread);
	initialize_board();

	while (true) {
		update();

		if (status == WAITING) { // Block until other player makes move
			wait(FIFONAME, &selection);
			status = MY_TURN;
			CLEAR_STDIN();
		}
		else {
			c = tolower(getchar());
			switch (c) {
				case 'w':
					pointed.i += increment;
					break;
				case 'a':
					pointed.j -= increment;
					break;
				case 's':
					pointed.i -= increment;
					break;
				case 'd':
					pointed.j += increment;
					break;
			}

			if (selected) {
				if (c == '\n') {
					if ((status = move_piece(&selection, color)) == 0) {
						update();
						fifo_fd = open(FIFONAME, O_WRONLY);
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
