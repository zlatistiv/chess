#include <stdio.h>
#include <string.h>

#include "cli.h"


void help_output() {
	printf("Usage: chess [OPTION]... [GAME ID]...\n");
	printf("Host a game of chess or join an existing one.\n");
	printf("  -h, --host  Host a game and provide the game ID for the second player to join.\n");
	printf("  -j, --join  Join an existing game with a provided game ID. The game ID matches the PID of the hosting process.\n");
	printf("\n");
	printf("In-game rules: use WASD keys to navigate the board cursor.\n");
	printf("Press the Enter key to make a selection. Press 'c' to cancel your selection.\n");
	printf("Move the cursor over the desired location and press Enter again to move the piece.\n");
}


int main(const int argc, const char** argv) {
	int status;

	if (argc == 1) {
		help_output();
		return 0;
	}
	else if (argc == 2 && !strcmp(argv[1], "--help")) {
		help_output();
		return 0;
	}
	else if (argc == 2 && (!strcmp(argv[1], "--host") || !strcmp(argv[1], "-h"))) {
		status = play(1, getpid());
	}
	else if (argc == 3 && (!strcmp(argv[1], "--join") || !strcmp(argv[1], "-j"))) {
		status = play(0, atoi(argv[2]));
	}
	else {
		help_output();
		return 1;
	}

	return status;
}

