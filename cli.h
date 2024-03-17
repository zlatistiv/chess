#ifndef CLI_H
#define CLI_H


int play(const bool, const char *, const char *);

// How to read input without pressing Enter (needed for this game):
// https://stackoverflow.com/questions/1798511/how-to-avoid-pressing-enter-with-getchar-for-reading-a-single-character-only

/*tcgetattr gets the parameters of the current terminal
	STDIN_FILENO will tell tcgetattr that it should write the settings
	of stdin to oldt*/

/*now the settings will be copied*/

/*ICANON normally takes care that one line at a time will be processed
	that means it will return if it sees a "\n" or an EOF or an EOL*/

/*Those new settings will be set to STDIN
	TCSANOW tells tcsetattr to change attributes immediately. */

// How to read input without pressing Enter (needed for this game):
// https://stackoverflow.com/questions/1798511/how-to-avoid-pressing-enter-with-getchar-for-reading-a-single-character-only

// This will put the shell into "unbuffered mode" where each character can be read one by one using getchar()

#define SETUP_SHELL() \
	static struct termios oldt, newt;\
	tcgetattr(STDIN_FILENO, &oldt);\
	newt = oldt;\
	newt.c_lflag &= ~(ICANON);\
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);\

#define SHELL_BACK_TO_NORMAL() tcsetattr(STDIN_FILENO, TCSANOW, &oldt)


// https://stackoverflow.com/questions/17318886/fflush-is-not-working-in-linux/23885008#23885008
#define CLEAR_STDIN() \
	int stdin_copy = dup(STDIN_FILENO); \
	tcdrain(stdin_copy); \
	tcflush(stdin_copy, TCIFLUSH); \
	close(stdin_copy); \

#endif
