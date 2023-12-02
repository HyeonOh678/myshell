#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "arraylist.h"
#include "tokenizer.h"
#include "mysh.h"

int main (int argc, char **argv) {
	// initializes shell: sets mode and validates argument for batch mode
	if (argc == 1) {
		mode = INTERACTIVE;
		fd = STDIN_FILENO;
		printf("Starting mysh in interactive mode\n");
	} else if (argc == 2) {
		if (access(argv[1], R_OK) == 0) {
			struct stat file_buffer;
			if (stat(argv[1], &file_buffer) == 0) {
				if (S_ISDIR(file_buffer.st_mode) == 0) {
					fd = open(argv[1], O_RDONLY);
					if (fd > 0) {
						mode = BATCH;
						if (MYSH_DEBUG)
							printf("Starting mysh in batch mode\n");
					} else {
						fprintf(stderr, "ERROR: Cannot open %s: %s\n", argv[1], strerror(errno));
					}
				} else {
					fprintf(stderr, "ERROR: Cannot run mysh on %s because it is a directory\n", argv[1]);
				}
			} else {
				fprintf(stderr, "Error checking file type for %s: %s\n", argv[1], strerror(errno));
			} 
		} else {
			fprintf(stderr, "ERROR: Invalid file: %s\n", strerror(errno));
		}
	} else {
		fprintf(stderr, "ERROR: Invalid argument count of %d\n", argc);
	}

	if (mode == INVALID) {
		return EXIT_FAILURE;
	}





	// main input loop
	do {
		if (mode == INTERACTIVE) {
			printf("mysh> ");
			fflush(stdout);
		}

		char* input = readLine(input);
		if (input == NULL) {
			fprintf(stderr, "ERROR: readLine returns null\n");
			return EXIT_FAILURE;
		}

		if (MYSH_DEBUG)
			printf(";%s;\n", input);

		if (strlen(input) != 0) {
			printf("tokeninzing");
			arraylist_t* arraylist = al_create(1);
			tokenizer(arraylist, input);
			char buf[100];
			printf("%s\n", getcwd(buf, 90));
			
			if (MYSH_DEBUG) {
				printf("%d\n", al_contains(arraylist, "|"));
				al_print(arraylist);
			}
			
			if(arraylist->length > 0) {
			// if exit break
			// if valid input
				// figure out what to do with the command
			// else error and return
			} else {
				fprintf(stderr, "ERROR: <= 0 tokens returned, this should not happen\n");
				free(input);
				al_destroy(arraylist);
				return EXIT_FAILURE;
			}

			al_destroy(arraylist);
		}

		free(input);
		if (hit_EOF) {
			break;
		}
	} while (1);
	
	if (mode == BATCH) {
		if (close(fd) == -1) {
			fprintf(stderr, "ERROR: Cannot close %s: %s\n", argv[1], strerror(errno));
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}

char* readLine (char* buffer) {
	buffer = malloc(4);
	int len = 0, capacity = 4;
	char* i = buffer;

	if (mode == BATCH) {

	}

	while (len == 0 || *(i - 1) != '\n') {
		if (len > capacity - 1) {
			capacity = capacity * 2;
			buffer = realloc(buffer, capacity);
			i = buffer + len;
		}
		int bytes_read = read(fd, i, 1);
		

		if (bytes_read == -1) {
			fprintf(stderr, "ERROR: Cannot read from file: %s\n", strerror(errno));
			return NULL;
		} else if (bytes_read == 0) {
			hit_EOF = 1;
			*i = '\0';
			return buffer;
		} else {
			if (*i == '\n') {
				*i = '\0';
				return buffer;
			}
			len++;
		}

		i++;
	}

	fprintf(stderr, "ERROR: This loop isn't supposed to terminate\n");
	return NULL;
}