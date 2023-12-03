#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include "../arraylist.h"
#include "../tokenizer.h"

int main(int argc, char** argv)
{
	arraylist_t* arr = al_create(5);
	
	char* line[28];
	line[0] = "cd subdir";
	line[1] = " echo hello";
	line[2] = "   cd sub  ";
	line[3] = "foo bar < baz | quux *.txt > spam";
	line[4] = "then foo < bar baz";
	line[5] = "cd boo<bar";
	line[6] = "exit";
	line[7] = "foo bar<baz";
	line[8] = "foo|bar";
	line[9] = "foo<bar<baz";
	line[10] = "src/*";
	line[11] = "src/tests/*";
	line[12] = "./src/tests/*";
	line[13] = "> *";
	line[14] = "< *.c";
	line[15] = "> *.c";
	line[16] = "| *"; // ?
	line[17] = "* |"; // ?
	line[18] = "<text";
	line[19] = "<text<";
	line[20] = "text<";
	line[21] = ">text";
	line[22] = ">text>";
	line[23] = "text>";
	line[24] = "|text";
	line[25] = "|text|";
	line[26] = "text|";
	line[27] = "../myshell/*";





	for (int i = 0; i < sizeof(line)/sizeof(char*); i++) {
		printf("LINE %d\n", i);
		printf("Return value of tokenizer: %d\n", tokenizer(arr, line[i]));

		al_print(arr);
		printf("\n\n\n");
		
		al_destroy(arr);
		arr = al_create(5);
	}

	al_destroy(arr);

	




	return 0;
}