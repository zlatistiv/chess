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

#include <termios.h> //termios, TCSANOW, ECHO, ICANON
#include <unistd.h> //STDIN_FILENO

#include <pthread.h>

#include "cli.h"
#include "board.h"

#define BSIZE 4

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
	return NULL;
}

static void wait(const int cfd) {
	char buf[BSIZE];
	read(cfd, buf, sizeof(buf));
	read_op_move(buf, !color);
}

int play(const bool is_white, const char *hostname, const char *port) {
	SETUP_SHELL();

	color = is_white;
	status = color;
	Pos marked = {0, 0};
	Pos pointed = {0, 0};
	selection.src = &marked;
	selection.dst = &pointed;
	char c;
	int increment;
	selected = false;
	increment = color * 2 - 1;
	pthread_t clock_thread;

	// Name resolution as shown in getaddrinfo(3)
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int sfd, cfd, s;
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
						char buf[BSIZE] = {selection.src->i, selection.src->j, selection.dst->i, selection.dst->j};
						write(cfd, buf, sizeof(buf));
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
