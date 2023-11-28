#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "arraylist.h"
#include "tokenizer.h"
#include <string.h>


void tokenizer(arraylist_t* arr, char* line)
{
	int lineSize = strlen(line);
	int i = 0;
	int left = 0;
	while(i <= lineSize)
	{
		if(line[i] == ' ' && line[left] == ' ')
		{
			i++;
			left++;
		}
		else if(i == lineSize && line[left] == ' ' && line[i] == ' ')
		{
			i++;
		}
		else if(i == lineSize && line[left] != ' ' && line[i] != ' ')
                {
                        int size = i - left + 1;
                        char* str = malloc(sizeof(char)* size);
                        memcpy(str, &line[left], size);
                        str[i - left] = '\0';
			al_push(arr, str);
                        //printf("token is: %s\n", str);
                        i++;
                        left = i;
                        free(str);

                }
	       	else if(line[i] == ' ' && line[left] != ' ')
		{
			int size = i - left + 1;
			char* str = malloc(sizeof(char)* size);
		        memcpy(str, &line[left], size);
			str[i - left] = '\0';
			al_push(arr, str);
			//printf("token is: %s\n", str);
			i++;
			left = i;
			free(str);	
		}
		else if((line[i] == '<' || line[i] == '>' || line[i] == '|') && (line[left] != ' ' && line[left] != '<' && line[left] != '>' && line[left] != '|'))
		{
		    int size = i - left + 1;
			char* str = malloc(sizeof(char)* size);
		    memcpy(str, &line[left], size);
			str[i - left] = '\0'; 
			al_push(arr, str);
		    free(str);
		    //now we take the single char token
		    int size2 = 2;
            char* str2 = malloc(sizeof(char)* size2);
            memcpy(str2, &line[i], size2);
            str2[1] = '\0';
            al_push(arr,str2);
            i++;
            left = i;
            free(str2);
		}
		else if(line[i] == '<' || line[i] == '>' || line[i] == '|')
		{
		    int size = 2;
		    char* str = malloc(sizeof(char) * size);
		    memcpy(str, &line[i], size);
		    str[1] = '\0';
		    al_push(arr,str);
		    i++;
		    left = i;
		    free(str);
		}	
		else
		{
			i++;
		}
		
		
	}


}
