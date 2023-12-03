#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv) {
    int retval = 1;
    while (retval > 0) {
        //fflush(stdout);
        char* buf = malloc(2);
        retval = read(STDIN_FILENO, buf, 1);
        *(buf + 1) = '\0';
        if (strcmp(buf, "z") != 0) {
            printf(";%s;\n", buf);
        } else {
            free(buf);
            return EXIT_SUCCESS;
        }
        free(buf);
    }
}