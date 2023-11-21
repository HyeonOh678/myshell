#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include "arraylist.h"
#include "mysh.h"


//this is where the main function will be run 

int main (int argc, char **argv) {
	enum mode_type {
		BATCH,
		INTERACTIVE
	};
	enum mode_type mode;

	if (argc == 1)
	{
		mode = BATCH;
	} else if (argc == 0) {
		mode = INTERACTIVE;
	} else {
		fprintf(stderr, "ERROR: Cannot initialize mysh with an argument count of %d", argc);
	}



	return EXIT_SUCCESS;
}
