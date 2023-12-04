#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "arraylist.h"
#include "tokenizer.h"
#include <string.h>
#include <glob.h>
#include <fcntl.h>

#define MAX_ARGS 100

char* argv[MAX_ARGS];

void tokenExpansion(arraylist_t* arr, char* token)
{
	//this is to search for filenames with wildcards
	glob_t result;
	int i;
	int argc = 0;
	int isDirectory = 0;
	int tokenLen = strlen(token);

	for(int i = 0; i < tokenLen; i++)
	{
		if(token[i] == '/')
		{
			isDirectory = 1;
			break;
		}
	}

	if(isDirectory == 1)
	{
		chdir(token);
	}

	if(glob(token, GLOB_NOCHECK | GLOB_TILDE, NULL, &result) == 0)
                {
                        for(i = 0; i < result.gl_pathc && argc < MAX_ARGS - 1;  i++)
                        {
                                argv[(argc)++] = strdup(result.gl_pathv[i]);
                        }
                }

	
	//now we push the array elements of argv to our arraylist_t arr
	for(int i = 0; i < argc; i++)
	{
		al_push(arr, argv[i]);
	}

	//free the glob and also the array
	globfree(&result);
	for(int i = 0; i < argc; i++)
	{
		free(argv[i]);
	}
	

}

int isWildCard(char* token)
{
	int isWildCard = 0;
        //first check if the token has the wildcard symbol in it *
        int tokenLen = strlen(token);
        for(int i = 0; i < tokenLen; i++)
        {
                if(token[i] == '*')
                {
                        isWildCard = 1;
                        break;
                }

        }

	return isWildCard;
}

int isPipe(char* token)
{
	int isPipe = 0;
        //first check if the token has the wildcard symbol in it *
        int tokenLen = strlen(token);
        for(int i = 0; i < tokenLen; i++)
        {
                if(token[i] == '|')
                {
                        isPipe = 1;
                        break;
                }

        }

        return isPipe;

}


int tokenizer(arraylist_t* arr, char* line)
{
	int lineSize = strlen(line);
	int i = 0;
	int left = 0;
	int isPrevTokenRedirect = 0;
	int isPrevWildCard = 0;
	int isLineInvalid = 1;

	while(i <= lineSize)
	{
		if(line[i] == ' ' && line[left] == ' ')
		{
			i++;
			left = i;
		}
		else if(i == lineSize && line[left] == ' ' && line[i] == ' ')
		{
			i++;
			left = i;
		}
	       	else if(line[i] == ' ' && line[left] != ' ')
		{
			int size = i - left + 1;
			char* str = malloc(sizeof(char)* size);
		        memcpy(str, &line[left], size);
			str[i - left] = '\0';
			//we will see if the token has a wildcard symbol 
                        if(isWildCard(str) == 0) //if the token doesn't contain wildcard, reset isPrevTokenRedirect to 0 anyways
                        {
                                al_push(arr, str);
                                isPrevTokenRedirect = 0;
                        }
                        else if(isWildCard(str) == 1 && isPrevTokenRedirect == 0) //if the token contains a wildcard but the prev token is not a redirect , expand token
                        {
                                tokenExpansion(arr, str);
				isPrevWildCard = 1;
                        }
                        else if(isWildCard(str) == 1 && isPrevTokenRedirect == 1) //if the token contains a wildcard and prev token is redirect, abort or send null
                        {
                                fprintf(stderr, "mysh: Invalid job: Cannot have wildcard after redirection or pipe.\n");
				tokenExpansion(arr, str);
                                isLineInvalid = 0;
                        }


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
		    //we will see if the token has a wildcard symbol
		     
                        if(isWildCard(str) == 0) //if the token doesn't contain wildcard, reset isPrevTokenRedirect to 0 anyways
                        {
                                al_push(arr, str);
				isPrevTokenRedirect = 0;
                        }
                        else if(isWildCard(str) == 1 && isPrevTokenRedirect == 0) //if the token contains a wildcard but the prev token is not a redirect , expand token
                        {
                                tokenExpansion(arr, str);
				isPrevWildCard = 1;
                        }
			else if(isWildCard(str) == 1 && isPrevTokenRedirect == 1) //if the token contains a wildcard and prev token is redirect, abort or send null
			{
				fprintf(stderr, "mysh: Invalid job: Cannot have wildcard after redirection or pipe.\n");
				printf("%s\n", str);
				tokenExpansion(arr, str);
                                isLineInvalid = 0;
			}

		    free(str);
		    //now we take the single char token
		    int size2 = 2;
            	    char* str2 = malloc(sizeof(char)* size2);
            	    memcpy(str2, &line[i], size2);
            	    str2[1] = '\0';
		    isPrevTokenRedirect = 1;
		    //check if the token before this is a wildcard and if the current token is a pipe
		    if(isPrevWildCard == 1 && isPipe(str2) == 1)
		    {
				fprintf(stderr, "mysh: Invalid job: Cannot have pipe after wildcard\n");
				isLineInvalid = 0;
		    }
		    else if(isPrevWildCard == 1 && isPipe(str2) == 0)
		    {
			    isPrevWildCard = 0;
		    }
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
		    isPrevTokenRedirect = 1;
	            //check if the token before this is a wildcard and if the current token is a pipe
                    if(isPrevWildCard == 1 && isPipe(str) == 1)
                    {
						fprintf(stderr, "mysh: Invalid job: Cannot have pipe after wildcard\n");
                        isLineInvalid = 0;
                    }
                    else if(isPrevWildCard == 1 && isPipe(str) == 0)
                    {
                            isPrevWildCard = 0;
                    }

		    al_push(arr,str);
		    i++;
		    left = i;
		    free(str);
		}
		else if(i == lineSize - 1 && line[left] != ' ' && line[i] != ' ')
                {
                        int size = i - left + 2;
                        char* str = malloc(sizeof(char)* size);
                        memcpy(str, &line[left], size);
                        str[i - left + 1] = '\0';

			//we will see if the token has a wildcard symbol 
			if(isWildCard(str) == 0) //if the token doesn't contain wildcard, reset isPrevTokenRedirect to 0 anyways
                        {
                                al_push(arr, str);
                                isPrevTokenRedirect = 0;
                        }
                        else if(isWildCard(str) == 1 && isPrevTokenRedirect == 0) //if the token contains a wildcard but the prev token is not a redirect , expand token
                        {
                                tokenExpansion(arr, str);
				isPrevWildCard = 1;
                        }
                        else if(isWildCard(str) == 1 && isPrevTokenRedirect == 1) //if the token contains a wildcard and prev token is redirect, abort or send null
                        {
                                fprintf(stderr, "mysh: Invalid job: Cannot have wildcard after redirection or pipe.\n");
				tokenExpansion(arr, str);
                                isLineInvalid = 0;			
                        }

			
                        //printf("token is: %s\n", str);
                        i++;
                        left = i;
                        free(str);

                }
		else
		{
			i++;
		}
		
		
	}

	return isLineInvalid;
}











