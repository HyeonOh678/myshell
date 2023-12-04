#include <stdio.h>
#include <stdlib.h>
#include "../arraylist.h"

int main(int argc, char** argv) {
    for (int i = 0; i < 2; i++) {
        // initialized empty arraylist
        arraylist_t* my_al = al_create(1);
        al_print(my_al);


        // after adding elements
        for (int j = 0; j < 26; j++) {
            char x[] = {97 + j, 97 + j, 97 + j, 97 + j, 97 + j, '\0'};
            al_push(my_al, (char*) x);
        }
        al_print(my_al);

        // tests if remove works, remove a random element at a time
        int len = my_al->length;
        for (int i = 0; i < len; i++) {
            int r = rand() % my_al->length;
            printf("%d\n", r);
            al_remove(my_al, r);
            al_print(my_al);
        }

        // deconstructing
        al_destroy(my_al);
    }
}