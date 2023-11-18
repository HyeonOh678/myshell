CC = gcc
CFLAGS = -g -std=c99 -Wvla -Wall -fsanitize=address,undefined

mysh: mysh.o tokenizer.o commands.o arraylist.o
	$(CC) $(CFLAGS) $^ -o $@

mysh.o tokenizer.o commands.o arraylist.o: mysh.h

mysh.h: arraylist.h

clean:
	rm -f *.o mysh

test0: mysh
	./mysh


