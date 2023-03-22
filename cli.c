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


void name_fifo(char buffer[], int pid) { // This function is very specific to what is done here...
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


int play(const bool white, const int game_id) {
	static struct termios oldt, newt;

    /*tcgetattr gets the parameters of the current terminal
    STDIN_FILENO will tell tcgetattr that it should write the settings
    of stdin to oldt*/
    tcgetattr(STDIN_FILENO, &oldt);
    /*now the settings will be copied*/
    newt = oldt;

    /*ICANON normally takes care that one line at a time will be processed
    that means it will return if it sees a "\n" or an EOF or an EOL*/
    newt.c_lflag &= ~(ICANON);          

    /*Those new settings will be set to STDIN
    TCSANOW tells tcsetattr to change attributes immediately. */
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);	

    char c;
    int pi, pj, si, sj;
    bool selection_made, my_turn;
    
    int fd = -1; // descriptor for the pipe
    char fifoname[30];
    int movebuffer[4];

    c = 0;
    si = -1;
    sj = -1;
    selection_made = false;

    initialize_board();

    if (white) { // Guest process
    	printf("Waiting for a guest to join. Game ID: %d\n", game_id);
		my_turn = true;
		pi = 0;
    	pj = 0;
    	name_fifo(fifoname, game_id);
    	if (mkfifo(fifoname, 0666) == -1) {
    		printf("%s\n", "Could not create FIFO!");
    		return -1;
    	}

	}
	else { // Host process
    	my_turn = false;
    	pi = BOARD_SIZE - 1;
    	pj = BOARD_SIZE - 1;
    	name_fifo(fifoname, game_id);
	}

	while (1) {
		system("clear");
		print_board(pi, pj, si, sj, !white);

		if (!my_turn) { // Block until other player makes move
			// printf("%s\n", "Waiting for opponents move...");
			if (fd != -1) close(fd);

			fd = open(fifoname, O_RDONLY);
			read(fd, movebuffer, sizeof(movebuffer));
			printf("%d\n", movebuffer[0]);
			printf("%d\n", movebuffer[1]);
			printf("%d\n", movebuffer[2]);
			printf("%d\n", movebuffer[3]);
			my_turn = true;

			// At this point the input buffer should be cleared... to be implemented...
			continue;
		}

		c = getchar();

		if (white) {
			if ((c == 'w' || c == 'W') && pi < BOARD_SIZE - 1) pi++;
			else if ((c == 'a' || c == 'A') && pj > 0) pj--;
			else if ((c == 's' || c == 'S') && pi > 0) pi--;
			else if ((c == 'd' || c == 'D') && pj < BOARD_SIZE - 1) pj++;
			else if (c == '\n') {
				if (selection_made) {
					if (move_piece(si, sj, pi, pj) == 0) { // move_piece should return 0 on success and 1 on an illegal move (To be implemented)
						// my_turn = false;
						movebuffer[0] = si;
						movebuffer[1] = sj;
						movebuffer[2] = pi;
						movebuffer[3] = pj;
						// fd = open(fifoname, O_WRONLY);
						// write(fd, movebuffer, sizeof(movebuffer));
					}
					si = -1;
					sj = -1;
					selection_made = false;
				}
				else {
					si = pi;
					sj = pj;
					selection_made = true;
				}
			}
			else if ((c == 'c' || c == 'C') && selection_made) { // Cancel selection
				si = -1;
				sj = -1;
				selection_made = false;
			}
		}
		else {
			if ((c == 'w' || c == 'W') && pi > 0) pi--;
			else if ((c == 'a' || c == 'A') && pj < BOARD_SIZE - 1) pj++;
			else if ((c == 's' || c == 'S') && pi < BOARD_SIZE - 1) pi++;
			else if ((c == 'd' || c == 'D') && pj > 0) pj--;
			else if (c == '\n') {
				if (selection_made) {
					if (move_piece(si, sj, pi, pj) == 0) { // move_piece should return 0 on success and 1 on an illegal move (To be implemented)
						my_turn = false;
						movebuffer[0] = si;
						movebuffer[1] = sj;
						movebuffer[2] = pi;
						movebuffer[3] = pj;
						fd = open(fifoname, O_WRONLY);
						write(fd, movebuffer, sizeof(movebuffer));
					}
					si = -1;
					sj = -1;
					selection_made = false;
				}
				else {
					si = pi;
					sj = pj;
					selection_made = true;
				}
			}
			else if ((c == 'c' || c == 'C') && selection_made) { // Cancel selection
				si = -1;
				sj = -1;
				selection_made = false;
			}
		}
	}

	/*restore the old settings*/
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

	return 0;
}
