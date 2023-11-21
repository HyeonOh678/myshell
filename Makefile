CC = gcc
CFLAGS = -g -Wvla -Wall -fsanitize=address,undefined

mysh: obj/mysh.o obj/arraylist.o
	$(CC) $(CFLAGS) obj/mysh.o obj/arraylist.o -o mysh
	mv mysh bin

obj/arraylist.o: src/arraylist.c
	$(CC) $(CFLAGS) src/arraylist.c -c -o arraylist.o
	mv arraylist.o obj
	
obj/mysh.o: src/mysh.c
	$(CC) $(CFLAGS) src/mysh.c -c -o mysh.o
	mv mysh.o obj

clean:
	rm obj/* bin/*