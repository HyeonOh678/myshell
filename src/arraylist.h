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

// int al_pop(arraylist_t *, char*);
