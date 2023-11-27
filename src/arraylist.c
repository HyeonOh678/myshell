#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "arraylist.h"
#include <string.h>

    // ensure that definitions are consistent with header

#ifndef DEBUG
#define DEBUG 0
#endif

int al_init(arraylist_t *, unsigned);

// create storage and initialize length/size
// returns 1 on success, 0 for failure
int al_init(arraylist_t* L, unsigned capacity)
{
    L->head = (char**) malloc(capacity * sizeof(char*));
    
    if (L->head == NULL)
        return 0;

    L->length = 0;
    L->size = capacity;
    return 1;
}

arraylist_t *al_create(unsigned capacity)
{
    arraylist_t* L = malloc(sizeof(arraylist_t));
    if (L == NULL)
        return NULL;

    if (al_init(L, capacity)) {
	    return L;
    }

    free(L);
    return NULL;
}

unsigned al_length(arraylist_t *L)
{
    return L->length;
}

// add specified element to end of list
// returns 1 on success, 0 on failure
// assumes list has been initialized
int al_push(arraylist_t *L, char* str)
{
    char* str_space = malloc(strlen(str) + 1);
    strcpy(str_space, str);
    str = str_space;

    // check whether array is full
    if (L->size == L->length) {
        // increase capacity
        int new_size = L->size * 2;

        char** new_head = realloc(L->head, new_size * sizeof(char*));
        
        if (new_head == NULL)
            return 0;

        if (DEBUG)
            printf("Increased size to %d from %d\n", new_size, L->size);

        L->size = new_size;
        L->head = new_head;
    }

    L->head[L->length] = str;
    L->length++;

    return 1;
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

// // remove item from end of list
// // write item to dest (if dest is non-NULL)
// // return 1 on success, 0 on failure (i.e., list is empty)

// int al_pop(arraylist_t *L, elem_t *dest)
// {
//     if (L->length == 0) return 0;

//     L->length--;
//     if (dest) *dest = L->data[L->length];
//     if (DEBUG) printf("Removed %d; new length %d\n", L->data[L->length], L->length);

//     return 1;
// }

void al_destroy(arraylist_t *L)
{
    for(int i = 0; i < L->length; i++) {
        free(al_get(L, i));
    }
    free(L->head);
    free(L);
}

char* al_get(arraylist_t* L, unsigned index) {
    if(index >= L->length) {
        fprintf(stderr, "ERROR: Cannot access element %u of arraylist when length is %u\n", index, L->length);
        return NULL;
    } else {
        return L->head[index];
    }
}