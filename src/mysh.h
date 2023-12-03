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
int hit_EOF = 0;	// flag set when read hits EOF in BATCH mode
int prev_return_value;	// return value of prev job created by the shell