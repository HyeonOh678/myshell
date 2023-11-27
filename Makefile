CC = gcc
CFLAGS = -g -Wvla -Wall -fsanitize=address,undefined

# executables
mysh: obj/mysh.o obj/arraylist.o
	$(CC) $(CFLAGS) obj/mysh.o obj/arraylist.o -o mysh

arraylist_tester: obj/arraylist_tester.o obj/arraylist.o
	$(CC) $(CFLAGS) obj/arraylist_tester.o obj/arraylist.o -o arraylist_tester

token_tester: obj/token_tester.o obj/arraylist.o
	$(CC) $(CFLAGS) obj/token_tester.o obj/arraylist.o -o token_tester

# object files
obj/arraylist.o: src/arraylist.c
	$(CC) $(CFLAGS) src/arraylist.c -c -o arraylist.o
	mv arraylist.o obj

obj/arraylist_tester.o: src/tests/arraylist_tester.c
	$(CC) $(CFLAGS) src/tests/arraylist_tester.c -c -o arraylist_tester.o
	mv arraylist_tester.o obj

obj/token_tester.o: src/tests/token_tester.c
	$(CC) $(CFLAGS) src/tests/token_tester.c -c -o token_tester.o
	mv token_tester.o obj

obj/mysh.o: src/mysh.c
	$(CC) $(CFLAGS) src/mysh.c -c -o mysh.o
	mv mysh.o obj

# misc
clean:
	rm obj/* bin/*
