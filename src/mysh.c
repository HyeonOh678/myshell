#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "arraylist.h"
#include "tokenizer.h"
#include "mysh.h"

int main (int argc, char **argv) {
	if (init(argc, argv) == EXIT_FAILURE) {
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
			break;
		}

		if (MYSH_DEBUG) {
			//printf(";%s;\n", input);
		}

		if (strlen(input) != 0) {
			arraylist_t* arraylist = al_create(1);
			int tokenizer_rv = tokenizer(arraylist, input) != 0;
			free(input);

			if (MYSH_DEBUG) {
				al_print(arraylist);
				//char buf[100];
				//printf("%s\n", getcwd(buf, 90));
			}
			
			if (tokenizer_rv != 0) {				
				if(arraylist->length > 0) {
					int check_conditionals_rv = check_conditionals(arraylist);

					if (check_conditionals_rv == EXIT_SUCCESS) {
						int pipes = al_contains(arraylist, "|");
						if (pipes == 0) {
							if (create_run_job(arraylist, NULL, NULL) == EXIT_SUCCESS) {
								prev_return_value = EXIT_SUCCESS;
							} else {
								prev_return_value = EXIT_FAILURE;
							}
						} else if (pipes == 1) {
							arraylist_t* left = arraylist;
							arraylist_t* right = al_create(1);
							int found_pipe = 0;

							for (int i = 0; i < arraylist->length; i++) {
								char* curr = al_get(arraylist, i);
								if (strcmp(curr, "|") == 0) {
									found_pipe = 1;

									if (i == 0 || strcmp(al_get(arraylist, i - 1), "<") == 0 || strcmp(al_get(arraylist, i - 1), ">") == 0) {
										fprintf(stderr, "mysh: syntax error\n");
										prev_return_value = EXIT_FAILURE;
										break;
									} else if (i == arraylist->length - 1 || strcmp(al_get(arraylist, i + 1), "then") == 0 || strcmp(al_get(arraylist, i + 1), "else") == 0) {
										fprintf(stderr, "mysh: syntax error\n");
										prev_return_value = EXIT_FAILURE;
										break;
									}

									al_remove(left, i);
									i--;
								} else if (found_pipe == 1) {
									al_push(right, al_get(arraylist, i));
									al_remove(left, i);
									i--;
								}
								
								if (MYSH_DEBUG) {
									al_print(left);
									al_print(right);
								}
							}

							// IMPLEMENT PIPING

							al_destroy(right);
						} else if (pipes > 1) {
							printf("mysh: mysh does not accept more than one pipe\n");
						}
					} else {
						prev_return_value = EXIT_FAILURE;
					}
				} else {
					fprintf(stderr, "ERROR: <= 0 tokens returned\n");
					al_destroy(arraylist);
					break;
				}
			} else {
				prev_return_value = EXIT_FAILURE;
			}
			al_destroy(arraylist);
		}
		if (exit_shell) {
			break;
		}
	} while (1);
	
	if (mode == BATCH) {
		if (close(fd) == -1) {
			fprintf(stderr, "ERROR: Cannot close %s: %s\n", argv[1], strerror(errno));
		}
	}
	if (exit_shell) {
		return EXIT_SUCCESS;
	} else {
		return EXIT_FAILURE;
	}
}

char* readLine (char* buffer) {
	buffer = malloc(4);
	int len = 0, capacity = 4;
	char* i = buffer;

	while (len == 0 || *(i - 1) != '\n') {
		if (len > capacity - 1) {
			capacity = capacity * 2;
			buffer = realloc(buffer, capacity);
			i = buffer + len;
		}
		int bytes_read = read(fd, i, 1);
		

		if (bytes_read == -1) {
			fprintf(stderr, "ERROR: Cannot read from file: %s\n", strerror(errno));
			free(buffer);
			return NULL;
		} else if (bytes_read == 0) {
			exit_shell = 1;
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
	free(buffer);
	return NULL;
}

int create_run_job(arraylist_t* tokens, char* input_stream, char* output_stream) {
	job_info job;

	job.name = NULL;
	job.arguments = tokens;
	if (input_stream != NULL) {
		job.path_std_in = malloc(strlen(input_stream) + 1);
		strcpy(job.path_std_in, input_stream);
	} else {
		job.path_std_in = NULL;
	}
	
	if (output_stream != NULL) {
		job.path_std_out = malloc(strlen(output_stream) + 1);
		strcpy(job.path_std_out, input_stream);
	} else {
		job.path_std_out = NULL;
	}

	int rv = parse_args(tokens, &job);

	if (MYSH_DEBUG) {
		printf("---JOB---\n");
		if (job.name != NULL)
			printf("NAME: %s\n", job.name);
		else
			printf("NAME: NULL\n");
		if (job.path_std_in != NULL)
			printf("STDIN: %s\n", job.path_std_in);
		else
			printf("STDIN: NULL\n");
		if (job.path_std_out != NULL)
			printf("STDOUT: %s\n", job.path_std_out);
		else
			printf("STDOUT: \n");
		printf("ARGS: \n");
		al_print(job.arguments);
		printf("---------\n");
	}

	if (rv == EXIT_SUCCESS) {
		int arg_one_is_path = strchr(job.name, '/') != NULL;

		if (MYSH_DEBUG)
			printf("%d\n", arg_one_is_path);

		if (arg_one_is_path) {
			if (access(job.name, F_OK) == 0) {
				if(access(job.name, X_OK) == 0) {
					// run command
					// fork and execv?
				} else {
					printf("mysh: %s: Permission denied\n", job.name);
				}
			} else {
				printf("mysh: %s: No such file or directory\n", job.name);
			}
		} else {
			if (strcmp(job.name, "exit") == 0) {
				exit_mysh();
				return EXIT_SUCCESS;
			} else if (strcmp(job.name, "pwd") == 0) {
				int id = 1;
				id = fork();
				if (id == 0) {
					char* wd = getcwd(NULL, 256);
					if (wd != NULL) {
						printf("%s\n", wd);
						free(wd);
					} else {
						fprintf(stderr, "ERROR: getcwd: %s\n", strerror(errno));
						return -1;
					}
				}
				wait(&prev_return_value);
			} else if (strcmp(job.name, "cd") == 0) {

			} else {
				int which = 0;
				if (strcmp(job.name, "which") == 0) {
					which = 1;
					if (job.arguments->length != 1 || strcmp(al_get(job.arguments, 0), "cd") == 0 || strcmp(al_get(job.arguments, 0), "pwd") == 0 || strcmp(al_get(job.arguments, 0), "which") == 0) {
						clear_job(&job);
						return EXIT_FAILURE;
					} else {
						free(job.name);
						job.name = malloc(strlen(al_get(job.arguments, 0)) + 1);
						strcpy(job.name, al_get(job.arguments, 0));
						al_remove(job.arguments, 0);
					}
				}
				const char* dirs[3];
				dirs[0] = "/usr/local/bin/";
				dirs[1] = "/usr/bin/";
				dirs[2] = "/bin/";

				int fileExists = 0, perms = 0;

				for (int i = 0; i < 3; i++) {
					char* path = malloc(strlen(job.name) + 20);
					strcpy(path, dirs[i]);
					strcat(path, job.name);

					if (MYSH_DEBUG) {
						printf(";%s;\n", path);
					}

					if (access(path, F_OK) == 0) {
						fileExists = 1;
						if(access(path, X_OK) == 0) {
							if (MYSH_DEBUG) {
								//printf("%s\n", dirs[i]);
							}

							pid = fork();
							int child_exit_status;
							if (pid == 0) {
								if (job.path_std_in != NULL) {
									if (access(job.path_std_in, R_OK) == 0) {

									} else {

										free(path);
										clear_job(&job);
										exit(EXIT_SUCCESS);
									}
								}
								if (job.path_std_out != NULL) {

								}
								// if (job.path_std_in != NULL) {
								// 	int fd = open(job.path_std_in,)
								// }
								// dup2

								if (which) {
									printf("%s\n", path);
									free(path);
									clear_job(&job);
									exit(EXIT_SUCCESS);
								} else {
									// exec
									exit(EXIT_SUCCESS);
								}
							} else {
								wait(&child_exit_status);
							}

							free(path);
							clear_job(&job);	
							return child_exit_status;
						}
					}
					free(path);
				}

				if (!fileExists && which == 0) {
					if (!perms) {
						printf("mysh: %s: No such file or directory\n", job.name);
					} else {
					printf("mysh: %s: Permission denied\n", job.name);
					}
				}
				clear_job(&job);
				return EXIT_FAILURE;
			}
		}			
	} else {
		return EXIT_FAILURE;
	}
}

// configures shell before it can take user input
// sets mode and validates arguments
int init(int argc, char** argv) {
	if (argc == 1) {
		mode = INTERACTIVE;
		fd = STDIN_FILENO;
		printf("Starting mysh in interactive mode\n");
		return EXIT_SUCCESS;
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
						return EXIT_SUCCESS;
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
	return EXIT_FAILURE;
}

int check_conditionals (arraylist_t* arraylist) {
	int cmp = -1;
	if (strcmp(al_get(arraylist, 0), "then") == 0) {
		cmp = EXIT_SUCCESS;
	} else if (strcmp(al_get(arraylist, 0), "else") == 0) {
		cmp = EXIT_FAILURE;
	} else {
		return EXIT_SUCCESS;
	}

	if (prev_return_value == cmp) {
		al_remove(arraylist, 0);
		return EXIT_SUCCESS;
	}
	
	if (prev_return_value == -1) {
		fprintf(stderr, "mysh: cannot run conditional job when there is no previous state\n");
	}
	return EXIT_FAILURE;
}

int parse_args (arraylist_t* tokens, job_info* job) {
	if (tokens->length < 1) {
		fprintf(stderr, "mysh: syntax error\n");
		return EXIT_FAILURE;
	}

	// process redirects
	for (int i = 0; i < tokens->length; i++) {
		if (strcmp(al_get(tokens, i), ">") == 0) {
			if (i < tokens->length - 1) {
				if (job->path_std_out != NULL) {
					free(job->path_std_out);
					job ->path_std_out = NULL;
				}
				job->path_std_out = malloc(strlen(al_get(tokens, i + 1)) + 1);
				strcpy(job->path_std_out, al_get(tokens, i + 1));
				al_remove(tokens, i);
				al_remove(tokens, i);
				i -= 1;
			} else {
				fprintf(stderr, "mysh: syntax error\n");
				return EXIT_FAILURE;
			}
		} else if (strcmp(al_get(tokens, i), "<") == 0) {
			if (i < tokens->length - 1) {
				if (job->path_std_in != NULL) {
					free(job->path_std_in);
					job ->path_std_in = NULL;
				}
				job->path_std_in = malloc(strlen(al_get(tokens, i + 1)) + 1);
				strcpy(job->path_std_in, al_get(tokens, i + 1));
				al_remove(tokens, i);
				al_remove(tokens, i);
				i -= 1;
			} else {
				fprintf(stderr, "mysh: syntax error\n");
				return EXIT_FAILURE;
			}
		}
	}

	// find job name
	for (int i = 0; i < tokens->length; i++) {
		job->name = malloc(strlen(al_get(tokens, i)) + 1);
		strcpy(job->name, al_get(tokens, i));
		al_remove(tokens, i);
		break;
	}

	return EXIT_SUCCESS;
}

void exit_mysh() {
	printf("mysh: exiting\n");
	exit_shell = 1;
}

void clear_job(job_info* job) {
	if (job->name != NULL){
		free(job->name);
	}
	if (job->path_std_in != NULL) {
		free(job->path_std_in);
	}
	if(job->path_std_out != NULL) {
		free(job->path_std_out);
	}
}