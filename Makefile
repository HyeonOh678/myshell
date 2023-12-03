CC = gcc
CFLAGS = -g -Wvla -Wall -fsanitize=address,undefined -std=gnu18

# executables
mysh: obj/mysh.o obj/arraylist.o obj/tokenizer.o
	$(CC) $(CFLAGS) obj/mysh.o obj/arraylist.o obj/tokenizer.o -o mysh

arraylist_tester: obj/arraylist_tester.o obj/arraylist.o
	$(CC) $(CFLAGS) obj/arraylist_tester.o obj/arraylist.o -o arraylist_tester

token_tester: obj/token_tester.o obj/arraylist.o obj/tokenizer.o
	$(CC) $(CFLAGS) obj/token_tester.o obj/arraylist.o obj/tokenizer.o -o token_tester

print_args: obj/print_args.o
	$(CC) $(CFLAGS) obj/print_args.o -o print_args

reader: obj/reader.o
	$(CC) $(CFLAGS) obj/reader.o -o reader


# object files
obj/arraylist.o: src/arraylist.c
	$(CC) $(CFLAGS) src/arraylist.c -c -o arraylist.o
	mv arraylist.o obj

obj/tokenizer.o: src/tokenizer.c
	$(CC) $(CFLAGS) src/tokenizer.c -c -o tokenizer.o
	mv tokenizer.o obj

obj/arraylist_tester.o: src/tests/arraylist_tester.c
	$(CC) $(CFLAGS) src/tests/arraylist_tester.c -c -o arraylist_tester.o
	mv arraylist_tester.o obj

obj/token_tester.o: src/tests/token_tester.c
	$(CC) $(CFLAGS) src/tests/token_tester.c -c -o token_tester.o
	mv token_tester.o obj

obj/mysh.o: src/mysh.c
	$(CC) $(CFLAGS) src/mysh.c -c -o mysh.o
	mv mysh.o obj

obj/print_args.o: src/tests/print_args.c
	$(CC) $(CFLAGS) src/tests/print_args.c -c -o print_args.o
	mv print_args.o obj

obj/reader.o: src/tests/reader.c
	$(CC) $(CFLAGS) src/tests/reader.c -c -o reader.o
	mv reader.o obj

# misc
clean:
	rm obj/*
