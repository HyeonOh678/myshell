#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include "../arraylist.h"
#include "../tokenizer.h"

void display_array_list (arraylist_t* arraylist) {
    printf("---\n");
    printf("Length: %u\n", arraylist->length);
    printf("Size: %u\n", arraylist->size);
    for (int i = 0; i < arraylist->length; i++) {
        printf("a[%d]: %p\t\t *a[%d]: %s\n", i, arraylist->head[i], i, al_get(arraylist, i));
    }
    printf("---\n");
}

int main(int argc, char** argv)
{
	arraylist_t* arr = al_create(5);
	

	char* line1 = "cd subdir";
	char* line2 = " echo hello";
	char* line3 = "cd subdir  ";
	char* line4 = "foo bar < baz | quux *.txt > spam";
	char* line5 = "then foo < bar baz";
	
	al_push(arr, line1);
	al_push(arr, line2);
	al_push(arr, line3);
	al_push(arr, line4);
	al_push(arr, line5);

	display_array_list(arr);
	al_destroy(arr);
	




	return 0;
}
