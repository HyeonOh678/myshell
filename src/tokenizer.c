#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include "arraylist.h"
#include "tokenizer.h"

//this will parse the arguments given to mysh
//first let's make parse for batch mode for now

#define DEBUG 0
#define BUFFLEN 1

void tokenizer(arraylist_t* arr, char* line)
{
	
	char* token;
        char* line2 = strdup(line);
       
        token = strtok(line2, " ");
        do
        {
	    al_push(arr, token);
            printf("token: %s\n", token);
        }
        while (token = strtok(NULL, " "));


}
