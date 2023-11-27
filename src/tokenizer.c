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
	int left = 0;
        int right = 0;
        int lineSize = strlen(line);
        char* token = NULL;
        int position = left;
        
	//pass through every character in line
        while(right <= lineSize)
        {	
		//first edge case: if both pointers are at a whitespace, move both
                if(line[right] == ' ' && line[left] == ' ')
                {
                    right++;
                    left++;
                }
                else if(right == lineSize) //second edge case: if the right pointer is at the last index, move it
                {
                    right++;
                }
                else if(line[right] == ' ' || line[right] == '\0' && line[left] != ' ')
                {
                        //allocate space for token from right - left
                        token = realloc(token, right - left);
                        position = left;
                        for(int i = 0; i < right - left; i++)
                        {
                                token[i] = line[position];
                                position++;
                        }
			
			//here we will append the token into the arraylist by calling the push function
			al_push(arr, token);
                        printf("token is: %s \n", token);
                        
                        right++;
                        left = right;
                        token = NULL;
                        

                }
                else 
                {
                    right++;
                    
                }
                       
                
                    

        }


}
