#include <stdio.h>
#include <stdlib.h>
#include "../arraylist.h"

void display_array_list(arraylist_t*);

int main(int argc, char** argv) {
    for (int i = 0; i < 2; i++) {
        // initialized empty arraylist
        arraylist_t* my_al = al_create(1);
        display_array_list(my_al);


        // after adding elements
        for (int j = 0; j < 26; j++) {
            char x[] = {97 + j, 97 + j, 97 + j, 97 + j, 97 + j, '\0'};
            al_push(my_al, (char*) x);
        }
        display_array_list(my_al);

        // deconstructing
        al_destroy(my_al);
    }
}

void display_array_list (arraylist_t* arraylist) {
    printf("---\n");
    printf("Length: %u\n", arraylist->length);
    printf("Size: %u\n", arraylist->size);
    for (int i = 0; i < arraylist->length; i++) {
        printf("a[%d]: \t%p\t\t *a[%d]: \t%s\n", i, arraylist->head[i], i, al_get(arraylist, i));
    }
    printf("---\n");
}