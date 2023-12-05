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

							int pipe_filedes[2] = {0, 0};
							pipe((int*)pipe_filedes);
							create_run_job(left, NULL, (char*)&pipe_filedes[1]);
							create_run_job(right, (char*)&pipe_filedes[0], NULL);

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

int create_run_job(arraylist_t* tokens, char* input_stream, char* output_stream) {
	job_info job;
	job.name = NULL;
	job.arguments = tokens;
	job.path_std_in = NULL;
	job.path_std_in = NULL;
	job.reads_from_pipe = -1;
	job.writes_to_pipe = -1;

	if (input_stream != NULL) {
		//job.reads_from_pipe = *((int*)input_stream);
	}
	if (output_stream != NULL) {
		//job.writes_to_pipe = *((int*)output_stream);
	}

	int parse_args_rv = parse_args(tokens, &job);

	if (MYSH_DEBUG) {
		// print_job(&job);
	}

	if (parse_args_rv == EXIT_SUCCESS) {
		if (strchr(job.name, '/') != NULL) {
			int fileExists = 0, perms = 0;
			if (access(job.name, F_OK) == 0) {
				fileExists = 1;
				if(access(job.name, X_OK) == 0) {
					pid = fork();
					int child_exit_status;
					if (pid == 0) {
						if (set_std_in(&job) != EXIT_SUCCESS) {
							clear_job(&job);
							return EXIT_FAILURE;
						}
						set_std_out(&job);

						al_push(job.arguments, "");
						job.arguments->head[job.arguments->length - 1] = '\0';
						
						int exec_rv = execv(job.name, job.arguments->head);
						if (exec_rv != 0) {
							printf("ERROR: %s\n", strerror(errno));
						}
						
					} else {
						wait(&child_exit_status);
					}

					clear_job(&job);	
					return child_exit_status;
				}
			}

			if (!fileExists || !perms) {
				fprintf(stderr, "mysh: %s: %s\n", job.name, strerror(errno));
			}
			clear_job(&job);
			return EXIT_FAILURE;
		} else if (strcmp(job.name, "exit") == 0) {
			clear_job(&job);
			printf("mysh: exiting\n");
			exit_shell = 1;
			return EXIT_SUCCESS;
		} else if (strcmp(job.name, "pwd") == 0) {
			int id = 1;
			id = fork();
			if (id == 0) {
				set_std_out(&job);

				char* wd = getcwd(NULL, 256);
				if (wd != NULL) {
					printf("%s\n", wd);
					free(wd);
					exit(EXIT_SUCCESS);
				} else {
					fprintf(stderr, "ERROR: getcwd: %s\n", strerror(errno));
					exit(errno);
				}
			} else {
				wait(&prev_return_value);
				clear_job(&job);
				return prev_return_value;
			}
		} else if (strcmp(job.name, "cd") == 0) {
			if (job.arguments->length == 1) {
					fprintf(stderr, "mysh: cd: expected 1 argument, got 0\n");
					clear_job(&job);
					return EXIT_FAILURE;				
			} else if (job.arguments->length == 2) {
				if (chdir(al_get(job.arguments, 1)) != 0) {
					fprintf(stderr, "mysh: %s: %s\n", job.name, strerror(errno));
					clear_job(&job);
					return EXIT_FAILURE;
				}
				clear_job(&job);
				return EXIT_SUCCESS;
			}
		} else {
			int which = 0;
			if (strcmp(job.name, "which") == 0) {
				which = 1;
				if (job.arguments->length != 2 || strcmp(al_get(job.arguments, 1), "cd") == 0 || strcmp(al_get(job.arguments, 1), "pwd") == 0 || strcmp(al_get(job.arguments, 1), "which") == 0) {
					clear_job(&job);
					return EXIT_FAILURE;
				} else {
					free(job.name);
					job.name = malloc(strlen(al_get(job.arguments, 1)) + 1);
					strcpy(job.name, al_get(job.arguments, 1));
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
					// fprintf(stderr, "\"%s\"\n", path);
				}

				if (access(path, F_OK) == 0) {
					fileExists = 1;
					if(access(path, X_OK) == 0) {
						pid = fork();
						int child_exit_status;
						if (pid == 0) {
							if (set_std_in(&job) != EXIT_SUCCESS) {
								clear_job(&job);
								return EXIT_FAILURE;
							}
							set_std_out(&job);

							if (which) {
								printf("%s\n", path);
								free(path);
								clear_job(&job);
								exit(EXIT_SUCCESS);
							} else {
								al_push(job.arguments, "");
								job.arguments->head[job.arguments->length - 1] = '\0';

								int exec_rv = execv(path, job.arguments->head);
								if (exec_rv != 0) {
									printf("ERROR: %s\n", strerror(errno));
								}
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
	} else {
		return EXIT_FAILURE;
	}
}
