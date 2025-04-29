#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include "cli.h"

static void help_output() {
	fprintf(stderr, "Usage: chess mode [hostname] [port]...\n");
	fprintf(stderr, "mode is either \"host\" or \"join\".\n");
	fprintf(stderr, "You provide the host and port whre to listen/connect\n");
	fprintf(stderr, "In-game rules: use WASD or HJKL to navigate the board.\n");
	fprintf(stderr, "Press the Enter or Space key to make a selection. Press 'c' to cancel your selection.\n");
	fprintf(stderr, "Move the cursor over the desired location and press Enter again to move the piece.\n");
	fprintf(stderr, "Your cursor has green color, your selection purple. Your oponents move is marked with red->yellow\n");
}

int main(int argc, char* argv[]) {
	if (argc != 4) {
		help_output();
		exit(1);
	}

	if (!strcmp(argv[1], "host")) game_main(WHITE, argv[2], argv[3]);
	else if (!strcmp(argv[1], "join")) game_main(BLACK, argv[2], argv[3]);
	else {
		help_output();
		exit(1);
	}

	return 0;
}

