#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include "arraylist.h"
#include "mysh.h"

int main (int argc, char **argv) {
	enum mode_type {
		BATCH,
		INTERACTIVE
	};
	enum mode_type mode;

	// validation
	if (argc == 1) {
		mode = INTERACTIVE;
		printf("Starting mysh in interactive mode\n");
	} 
	else if (argc == 2) {
		mode = BATCH;

		if (access(argv[1], R_OK) == 0) {
			fprintf(stderr, "ERROR: Invalid file, %s is not a shell script\n", argv[1]);
			return EXIT_FAILURE;
		} 
		else {
			fprintf(stderr, "ERROR: Invalid file: %s\n", strerror(errno));
			return EXIT_FAILURE;
		}

		printf("in batch mode now\n");
		//if file name is valid, we will pass it to our parser where it will open the file and read commands 
		//it will read line by line 
                struct stat st;
                char* name = argv[0];

                

			

	} 
	else {
		fprintf(stderr, "ERROR: Cannot initialize mysh with an argument count of %d\n", argc);
		return EXIT_FAILURE;
	}
	
	/*
	while (1) {
		printf("mysh> ");

		char* input;
		if (mode == INTERACTIVE) {
			// take user input and wait
		} else {
			// read a line
		}

		// if empty user input pass
		// if end of .sh file break

		// else tokenize
			// if exit break
			// if valid input
				// figure out what to do with the command
			// else error and return
	}
	*/
	return EXIT_SUCCESS;
}
