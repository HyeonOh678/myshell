#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

//this will parse the arguments given to mysh
//first let's make parse for batch mode for now

#define DEBUG 0
#define BUFFLEN 1

void parser(char* name)
{
	char buffer[BUFFLEN];

	int position = 0;
	int bytes = 0;
	int commandLength = 0;
	int fd;
	int start;

	char* cmd = NULL; //this will be the command that is parsed, for example echo 
	fd = open(name, O_RDONLY);

	if (fd < 0) {
		perror(name);
		exit(EXIT_FAILURE);
	}

	while((bytes = read(fd, buffer, BUFFLEN) > 0)
	{
		start = 0;
		//we will parse the first command here
		

	}		
			












}
