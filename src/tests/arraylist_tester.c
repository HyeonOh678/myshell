#include <stdio.h>
#include <stdlib.h>
#include "../arraylist.h"

void display_array_list(arraylist_t*);

int main(int argc, char** argv) {
    // initialized empty arraylist
    arraylist_t* my_al = al_create(5);
    display_array_list(my_al);


    // after adding elements
    al_push(my_al, "hello");
    al_push(my_al, "world");
    al_push(my_al, "a");
    al_push(my_al, "b");
    al_push(my_al, "c");
    al_push(my_al, "d");
    al_push(my_al, "e");
    display_array_list(my_al);

    // deconstructing
    al_destroy(my_al);
}

void display_array_list (arraylist_t* arraylist) {
    printf("---\n");
    printf("Length: %u\n", arraylist->length);
    printf("Size: %u\n", arraylist->size);
    for (int i = 0; i < arraylist->length; i++) {
        printf("a[%d]: %p\t\t *a[%d]: %s\n", i, arraylist->head[i], i, al_get(arraylist, i));
    }
    printf("---\n");
}