#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include "cli.h"

void help_output() {
	printf("Usage: chess [MODE] [hostname] [port]...\n");
	printf("Host a game of chess or join an existing one.\n");
	printf("  -h, --host  Host a game and provide a hostname and port combo to listen on\n");
	printf("  -j, --join  Join an existing game and provide a hostname and port combo to connecto to.\n");
	printf("\n");
	printf("In-game rules: use WASD keys to navigate the board cursor.\n");
	printf("Press the Enter key to make a selection. Press 'c' to cancel your selection.\n");
	printf("Move the cursor over the desired location and press Enter again to move the piece.\n");
	printf("Your cursor has green color, your selection purple. Your oponents move is marked with red->yellow\n");
}

int main(int argc, char* argv[]) {
	int status;

	if (argc == 1) {
		help_output();
		return 0;
	}
	else if (argc == 2 && !strcmp(argv[1], "--help")) {
		help_output();
		return 0;
	}
	else if (argc == 4 && (!strcmp(argv[1], "--host") || !strcmp(argv[1], "-h"))) {
		status = play(true, argv[2], argv[3]);
	}
	else if (argc == 4 && (!strcmp(argv[1], "--join") || !strcmp(argv[1], "-j"))) {
		status = play(false, argv[2], argv[3]);
	}
	else {
		help_output();
		return 1;
	}

	return status;
}

