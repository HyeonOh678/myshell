#ifndef ARRAYLIST_H
#define ARRAYLIST_H

typedef struct {
    char** head;
    unsigned length;  // number of Nodes in list
    unsigned size; // maximum number of Nodes list can currently store
} arraylist_t;

arraylist_t* al_create(unsigned);
unsigned al_length(arraylist_t *);
int al_push(arraylist_t *, char*);
void al_destroy(arraylist_t *);
char* al_get(arraylist_t*, unsigned);
void al_print(arraylist_t*);
int al_contains(arraylist_t*, char*);
int al_push(arraylist_t *, char*);
int al_remove(arraylist_t*, int);

// int al_pop(arraylist_t *, char*);

#endif