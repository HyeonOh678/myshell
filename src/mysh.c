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
			if (MYSH_DEBUG && exit_shell != 1) {
				fprintf(stderr, "ERROR: readLine returns null\n");
			}
			break;
		}

		if (MYSH_DEBUG) {
			printf(";%s;\n", input);
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
					if (check_conditionals(arraylist) == EXIT_SUCCESS) {
						int pipes = al_contains(arraylist, "|");
						if (pipes == 0) {
							create_run_job(arraylist, -1, -1);
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
							}

							if (MYSH_DEBUG) {
								al_print(left);
								al_print(right);
							}

							int pipe_fd[2] = {0, 0};
							if (pipe(pipe_fd) == 0) {
								create_run_job(left, -1, pipe_fd[1]);
								create_run_job(right, pipe_fd[0], -1);
							} else {
								fprintf(stderr, "Error occurred when creating pipe\n");
							}

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
			if (len == 0) {
				free(buffer);
				return NULL;
			}
			*i = '\0';
			return buffer;
		} else if (*i == '\n') {
			if (len == 0) {
				i--;
			} else {
				*i = '\0';
				return buffer;
			}
		} else {
			len++;
		}
		i++;
	}

	fprintf(stderr, "ERROR: This loop isn't supposed to terminate\n");
	free(buffer);
	return NULL;
}

int create_run_job(arraylist_t* tokens, int pipe_input_fd, int pipe_output_fd) {
	job_info job;

	job.name = NULL;
	job.arguments = tokens;
	job.path_std_in = NULL;
	job.path_std_out = NULL;
	job.pipe_std_in = -1;
	job.pipe_std_out = -1;

	if (pipe_input_fd != -1) {
		job.pipe_std_in = pipe_input_fd;
	}
	if (pipe_output_fd != -1) {
		job.pipe_std_out = pipe_output_fd;
	}

	int parse_args_rv = parse_args(tokens, &job);

	if (MYSH_DEBUG) {
		print_job(&job);
	}

	if (parse_args_rv == EXIT_SUCCESS) {
		if (job.name == NULL) {
			pid = fork();
			if (pid == 0) {
				set_std_in(&job);
				set_std_out(&job);
				exit(EXIT_SUCCESS);
			} else {
				wait(&prev_return_value);
				clear_job(&job);	
				return EXIT_SUCCESS;
			}
		} else if (strchr(job.name, '/') != NULL) {
			int fileExists = 0, perms = 0;
			if (access(job.name, F_OK) == 0) {
				fileExists = 1;
				if(access(job.name, X_OK) == 0) {
					pid = fork();
					if (pid == 0) {
						set_std_in(&job);
						set_std_out(&job);

						al_push(job.arguments, "");
						void* temp = job.arguments->head[job.arguments->length - 1];
						job.arguments->head[job.arguments->length - 1] = '\0';
						

						if (MYSH_DEBUG) {
							al_print(job.arguments);
						}

						int exec_rv = execv(job.name, job.arguments->head);
						if (exec_rv != 0) {
							free(temp);
							printf("ERROR: %s\n", strerror(errno));
							exit(errno);
						}
						
					} else {
						wait(&prev_return_value);
					}

					clear_job(&job);	
					return EXIT_FAILURE;
				}
			}

			if (!fileExists || !perms) {
				fprintf(stderr, "mysh: %s: %s\n", job.name, strerror(errno));
			}
			clear_job(&job);
			prev_return_value = 0;
			return EXIT_FAILURE;
		} else if (strcmp(job.name, "exit") == 0) {
			if (job.path_std_out != NULL) {
				if (fork() == 0) {
					set_std_out(&job);
					exit(EXIT_SUCCESS);
				} else {
					wait(&prev_return_value);
				}
			}
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
			if (job.arguments->length == 2) {
				if (chdir(al_get(job.arguments, 1)) != 0) {
					fprintf(stderr, "mysh: %s: %s\n", job.name, strerror(errno));
					clear_job(&job);
					return EXIT_FAILURE;
				}
				clear_job(&job);
				return EXIT_SUCCESS;
			} else {
				fprintf(stderr, "mysh: cd: expected 1 argument, got %d\n", job.arguments->length - 1);
				clear_job(&job);
				return EXIT_FAILURE;
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
					printf(";%s;\n", path);
				}

				if (access(path, F_OK) == 0) {
					fileExists = 1;
					if(access(path, X_OK) == 0) {
						pid = fork();
						if (pid == 0) {
							set_std_in(&job);
							set_std_out(&job);

							if (which) {
								printf("%s\n", path);
								free(path);
								exit(EXIT_SUCCESS);
							} else {
								al_push(job.arguments, "");
								void* temp = job.arguments->head[job.arguments->length - 1];
								job.arguments->head[job.arguments->length - 1] = '\0';
								
								if (MYSH_DEBUG) {
									al_print(job.arguments);
								}

								int exec_rv = execv(path, job.arguments->head);
								if (exec_rv != 0) {
									printf("ERROR: %s\n", strerror(errno));
									free(temp);
									exit(errno);
								}
							}
						} else {
							wait(&prev_return_value);
						}

						free(path);
						clear_job(&job);	
						return prev_return_value;
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

// configures shell before it can take user input
// sets mode and validates arguments
int init(int argc, char** argv) {
	if (argc == 1) {
		printf("Starting mysh in interactive mode\n");
		fd = STDIN_FILENO;
		mode = INTERACTIVE;
		return EXIT_SUCCESS;
	} else if (argc == 2) {
		if (access(argv[1], R_OK) == 0) {
			if (is_dir(argv[1]) == 0) {
				fd = open(argv[1], O_RDONLY);
				if (fd > 0) {
					mode = BATCH;
					return EXIT_SUCCESS;
				}
			} 
		}
	} else {
		errno = E2BIG;
	}

	fprintf(stderr, "mysh: ");
	if (argc == 2) {
		fprintf(stderr, "%s: ", argv[1]);
	}
	fprintf(stderr, "%s\n", strerror(errno));
	return EXIT_FAILURE;
}

// return 0 if arg is not a dir
// return 1 if arg is a dir
int is_dir (char* arg) {
	struct stat file_buffer;
	if (stat(arg, &file_buffer) == 0) {
		if (S_ISDIR(file_buffer.st_mode) == 0) {
			return 0;
		} else {
			errno = EISDIR;
			return 1;
		}
	} else {
		return 1;
	}
}

int check_conditionals (arraylist_t* arraylist) {
	if (strcmp(al_get(arraylist, 0), "then") == 0) {
		if (arraylist->length == 1) {
			fprintf(stderr, "mysh: syntax error: list a command after then\n");
			return EXIT_FAILURE;				
		}

		if (prev_return_value == EXIT_SUCCESS) {
			al_remove(arraylist, 0);
			return EXIT_SUCCESS;
		} else if (prev_return_value == -1) {
			fprintf(stderr, "mysh: cannot run conditional job when there is no previous state\n");
		}
		return EXIT_FAILURE;
	} else if (strcmp(al_get(arraylist, 0), "else") == 0) {
		if (arraylist->length == 1) {
			fprintf(stderr, "mysh: syntax error: list a command after else\n");
			return EXIT_FAILURE;
		}

		if (prev_return_value != EXIT_SUCCESS && prev_return_value != -1) {
			al_remove(arraylist, 0);
			return EXIT_SUCCESS;
		} else if (prev_return_value == -1) {
			fprintf(stderr, "mysh: cannot run conditional job when there is no previous state\n");
		}
		return EXIT_FAILURE;
	} else {
		return EXIT_SUCCESS;
	}
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
				printf("%s\n", job->path_std_in);
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
		break;
	}

	return EXIT_SUCCESS;
}

void clear_job(job_info* job) {
	if (job == NULL) {
		return;
	}
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

void print_job(job_info* job) {
	fprintf(stderr, "=== JOB ===\n");
	fprintf(stderr, "NAME: %s\n", null_wrapper(job->name));
	fprintf(stderr, "STDIN: %s\n", null_wrapper(job->path_std_in));
	fprintf(stderr, "STDOUT: %s\n", null_wrapper(job->path_std_out));
	fprintf(stderr, "PIPE_STD_IN: %d\n", job->pipe_std_in);
	fprintf(stderr, "PIPE_STD_OUT: %d\n", job->pipe_std_out);
	fprintf(stderr, "ARGS:\n");
	al_print(job->arguments);
	fprintf(stderr, "===========\n");
}

char* null_wrapper(char* field) {
	if (field != NULL) {
		return field;
	} else {
		return "NULL";
	}
}

int set_std_in (job_info* job) {
	if (job->path_std_in != NULL) {
		if (access(job->path_std_in, R_OK) == 0) {
			int std_in = open(job->path_std_in, O_RDONLY);
			dup2(std_in, STDIN_FILENO);
		} else {
			fprintf(stderr, "mysh: %s: %s\n", job->path_std_in, strerror(errno));
			exit(EXIT_FAILURE);
		}
	} else if (job->pipe_std_in != -1) {
		dup2(job->pipe_std_in, STDIN_FILENO);
	}
	return EXIT_SUCCESS;
}

int set_std_out (job_info* job) {
	if (job->path_std_out != NULL) {
		if (access(job->path_std_out, W_OK) == 0) {
			int std_out = open(job->path_std_out, O_WRONLY | O_TRUNC);
			dup2(std_out, STDOUT_FILENO);
		} else {
			int std_out = open(job->path_std_out, O_CREAT | O_WRONLY);
			chmod(job->path_std_out, S_IRUSR|S_IWUSR|S_IRGRP);
			dup2(std_out, STDOUT_FILENO);
		}
	} else if (job->pipe_std_out != -1) {
		dup2(job->pipe_std_out, STDOUT_FILENO);
	}
	return EXIT_SUCCESS;
}