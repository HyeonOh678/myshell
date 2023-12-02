#ifndef MYSH_DEBUG
#define MYSH_DEBUG 1
#endif

char* readLine (char*);

enum mode_type {
	BATCH,
	INTERACTIVE,
	INVALID
};

// global variables
enum mode_type mode = INVALID;
int fd; //fd for the shell's input
int hit_EOF = 0;
int prev_return_value;