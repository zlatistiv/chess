#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include <termios.h> //termios, TCSANOW, ECHO, ICANON
#include <unistd.h> //STDIN_FILENO
// How to read input without pressing Enter (needed for this game):
// https://stackoverflow.com/questions/1798511/how-to-avoid-pressing-enter-with-getchar-for-reading-a-single-character-only

#include "cli.h"
#include "board.h"


static int input(char movebuffer[], const char fifoname[], const bool white);

static void wait(char movebuffer[], const char fifoname[]);

static void name_fifo(char buffer[], int pid);
 
static const char *status_str[] = {
	"\n## Waiting for the other player... ##",
	"\n## Your turn. ##", // My turn
	"\n## Bad move! Please try again. ##"
};


int play(const bool white, const int game_id) { // This is the main routine in this file
	SETUP_SHELL();

	char fifoname[30];
	char movebuffer[4];
	int status = white; // 0 -> waiting for other player; 1 -> my move; 2 -> my move but my last move was illegal

	movebuffer[0] = 0;
	movebuffer[1] = 0;
	movebuffer[2] = -1;
	movebuffer[3] = -1;

	initialize_board();
	name_fifo(fifoname, game_id);

	int fd;
	if (white) { 
		if (mkfifo(fifoname, 0666) == -1) {
			printf("%s\n", "Could not create FIFO!");
			return -1;
		}
		printf("Waiting for a guest to join, Game ID: %d\n", game_id);
		fd = open(fifoname, O_RDONLY); // Execution will block here, until the other player opens the fifo pipe for writing
		close(fd);
	}
	else {
		fd = open(fifoname, O_WRONLY);
		close(fd);
	}

	while (true) {
		system("clear");
		print_board(movebuffer[0], movebuffer[1], movebuffer[2], movebuffer[3], !white);
		printf("%s\n", status_str[status]);

		if (status == 0) { // Block until other player makes move
			wait(movebuffer, fifoname);
			status = 1;
			
			CLEAR_STDIN();
			
			continue;
		}

		status = input(movebuffer, fifoname, white);
	}

	SHELL_BACK_TO_NORMAL();

	return 0;
}


static int input(char movebuffer[], const char fifoname[], const bool white) {
	char* pi;
	char* pj;
	char* si;
	char* sj;
	pi = &movebuffer[0];
	pj = &movebuffer[1];
	si = &movebuffer[2];
	sj = &movebuffer[3];
	char c;
	int fd;
	static bool selection_made = false;

	c = getchar();

	// The if statements can certaintly be optimized...

	if (white) {
		if ((c == 'w' || c == 'W') && *pi < BOARD_SIZE - 1) (*pi)++;
		else if ((c == 'a' || c == 'A') && *pj > 0) (*pj)--;
		else if ((c == 's' || c == 'S') && *pi > 0) (*pi)--;
		else if ((c == 'd' || c == 'D') && *pj < BOARD_SIZE - 1) (*pj)++;
	}
	else {
		if ((c == 'w' || c == 'W') && *pi > 0) (*pi)--;
		else if ((c == 'a' || c == 'A') && *pj < BOARD_SIZE - 1) (*pj)++;
		else if ((c == 's' || c == 'S') && *pi < BOARD_SIZE - 1) (*pi)++;
		else if ((c == 'd' || c == 'D') && *pj > 0) (*pj)--;
	}

	if (c == '\n') {
		if (selection_made) {
			if (move_piece(*pi, *pj, *si, *sj, white) == 0) { // move_piece should return 0 on success and -1 on an illegal move
				fd = open(fifoname, O_WRONLY);
				write(fd, movebuffer, 4);
				close(fd);

				*si = -1;
				*sj = -1;
				selection_made = false;

				return 0;
			}
			else { // Illegal move
				selection_made = false;
				*si = -1;
				*sj = -1;
				return 2;
			}
		}
		else {
			*si = *pi;
			*sj = *pj;
			selection_made = true;
		}
	}
	else if ((c == 'c' || c == 'C') && selection_made) { // Cancel selection
		*si = -1;
		*sj = -1;
		selection_made = false;
	}

	return 1;
}

static void wait(char movebuffer[], const char fifoname[]) {
	int fd;
	fd = open(fifoname, O_RDONLY); // Execution will block here, until the other player writes to the fifo
	read(fd, movebuffer, 4);
	close(fd);
	move_piece_unsafe(movebuffer[0], movebuffer[1], movebuffer[2], movebuffer[3]);
}



static void name_fifo(char buffer[], int pid) { // This function is very specific to what is done here ... it assigns a filename for the FIFO in the format /tmp/fifo_<PID>
	int i, len;
	strcpy(buffer, "/tmp/fifo_");
	char buf[10];
	
	i = 0;

	do {
		buf[i++] = pid % 10 + '0';

	} while ((pid /= 10) > 0);

	buf[i] = '\0';

	len = i;

	for (i = 0; buf[i] != '\0'; i++) {
		buffer[i + 10] = buf[len - i - 1];
	}

	buffer[10 + len] = '\0';
}

