#include "arraylist.h"

#ifndef MYSH_DEBUG
#define MYSH_DEBUG 0
#endif

typedef enum _mode_type {
	BATCH,
	INTERACTIVE,
	INVALID
} mode_type;

typedef struct _job_info {
	char* name;
	char* path_std_in;
	char* path_std_out;
	arraylist_t* arguments;
	int pipe_std_in;
	int pipe_std_out;
} job_info;

// global variables
mode_type mode = INVALID;
int fd; //fd for the shell's input
int exit_shell = 0;	// flag set when shell successfully terminates after finishing all jobs in batch mode or hitting exit in interactive
int prev_return_value = -1;	// return value of prev job created by the shell
int pid;

int init(int, char**);
int is_dir (char*);

char* readLine (char*);

int check_conditionals (arraylist_t*);
int create_run_job(arraylist_t*, int, int);
int parse_args (arraylist_t*, job_info*);

int set_std_in(job_info*);
int set_std_out(job_info*);

void print_job(job_info*);
char* null_wrapper(char*);
void clear_job(job_info*);
