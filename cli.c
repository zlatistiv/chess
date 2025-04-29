#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>

#include <termios.h> //termios, TCSANOW, ECHO, ICANON
#include <unistd.h> //STDIN_FILENO

#include <pthread.h>

#include "cli.h"
#include "board.h"

#define BSIZE 4
#define LOSEMSG "lll"
#define WINMSG "www"

#define LOSEBANNER "\
############\n\
#          #\n\
# YOU LOSE #\n\
#          #\n\
############\n"

#define WINBANNER "\
###########\n\
#         #\n\
# YOU WIN #\n\
#         #\n\
###########\n"

static const char *status_str[] = {
	"\n## Waiting for the other player... ##",
	"\n## Your turn. ##",
	"\n## Bad move! Please try again. ##",
	"\n## Cannot move due to check condition. ##"
};

int clocks[2];
Color color;
Status status;
Move selection = {.src = {0, 0}, .dst = {0, 0}};
Move op_move = {.src = {0, 0}, .dst = {0, 0}};
bool selected;
int cfd;
pthread_t clock_thread;

static void lose() {
	if (cfd > 2) write(cfd, LOSEMSG, BSIZE);
	printf(LOSEBANNER);
	exit(0);
}

static void update() {
	system("clear");
	printf("White: %d:%02d; Black: %d:%02d\n\n", clocks[WHITE] / 60, clocks[WHITE] % 60, clocks[BLACK] / 60, clocks[BLACK] % 60);
	print_board(&selection, &op_move, color, selected);
	printf("%s\n", status_str[status]);
}

static void *update_clock(void *vargs) { // Periodically run to update the clock and re-display the screen
	while (1) {
		sleep(1);
		update();

		if (status == WAITING) clocks[!color]--;
		else clocks[color]--;
		if (clocks[color] < 0) {
			lose();
			break;
		}
	}
	return NULL;
}

static void wait() {
	char buf[BSIZE];
	read(cfd, buf, sizeof(buf));
	op_move.src.i = buf[0];
	op_move.src.j = buf[1];
	op_move.dst.i = buf[2];
	op_move.dst.j = buf[3];

	if (strcmp(buf, LOSEMSG) == 0) {
		pthread_cancel(clock_thread);
		printf(WINBANNER);
		exit(0);
	}

	move_piece(&op_move, &selection, !color);
}

int game_main(const Color col, const char *hostname, const char *port) {
	SETUP_SHELL();

	color = col;
	status = color;
	clocks[0] = 600;
	clocks[1] = 600;
	char c;
	int increment;
	selected = false;
	increment = color * 2 - 1;

	// Name resolution as shown in getaddrinfo(3)
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int sfd, s;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_protocol = 0;
	hints.ai_canonname = NULL;
	hints.ai_next = NULL;
	
	s = getaddrinfo(hostname, port, &hints, &result);
	if (s != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
		exit(EXIT_FAILURE);
	}

	if (color == WHITE) {
		for (rp = result; rp != NULL; rp = rp->ai_next) {
			sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
			if (sfd == -1) continue;
			if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0) break;
			close(sfd);
		}

		if (rp == NULL) {
			fprintf(stderr, "bind() failure!\n");
			exit(EXIT_FAILURE);
		}
	
		if ((listen(sfd, 1)) != 0) {
			fprintf(stderr, "listen() failure!\n");
			exit(EXIT_FAILURE); 
		}
	
		printf("Waiting for an oponent to join, listening on: %s:%s\n", hostname, port);
		
		cfd = accept(sfd, rp->ai_addr, &rp->ai_addrlen); // Will block here until the opponent has connected
		if (cfd < 0) {
			fprintf(stderr, "accept() failure!\n");
			exit(EXIT_FAILURE); 
		}
	}
	else {
		for (rp = result; rp != NULL; rp = rp->ai_next) {
			cfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
			if (cfd == -1) continue;
			if (connect(cfd, rp->ai_addr, rp->ai_addrlen) != -1) break;
			close(cfd);
		}
			
		if (rp == NULL) {
			fprintf(stderr, "connect() failure!\n");
			exit(EXIT_FAILURE);
		}
	 }

	freeaddrinfo(result);
	
	// Starting the game...
	pthread_create(&clock_thread, NULL, update_clock, NULL);
	initialize_board();

	// Main game loop
	while (true) {
		update();

		if (status == WAITING) { // Block until other player makes move
			wait(cfd);
			status = MY_TURN;
			CLEAR_STDIN();
		}
		else {
			c = tolower(getchar());
			switch (c) {
				case 'w':
				case 'k':
					selection.dst.i += increment;
					break;
				case 'a':
				case 'h':
					selection.dst.j -= increment;
					break;
				case 's':
				case 'j':
					selection.dst.i -= increment;
					break;
				case 'd':
				case 'l':
					selection.dst.j += increment;
					break;
			}

			if (selected) {
				if (c == '\n' || c == ' ') {
					if ((status = move_piece(&selection, &op_move, color)) == 0) {
						update();
						char buf[BSIZE] = {selection.src.i, selection.src.j, selection.dst.i, selection.dst.j};
						write(cfd, buf, sizeof(buf));
					}

					selected = false;
				}
				else if (c == 'c' || c == ' ') {
					selected = false;
				}
			}
			else if (c == '\n' || c == ' ') {
				selection.src.i = selection.dst.i;
				selection.src.j = selection.dst.j;
				selected = true;
			}
		}
	}

	SHELL_BACK_TO_NORMAL();
	return 0;
}
