# My Shell 

#### Project 3 for Systems Programming (CS214) at Rutgers University New Brunswick

## Collaborators:

Dhvani Patel (dgp69)

Hyeon Min Oh (hmo18)

## How to Compile

**Please compile in directory myshell**

To compile mysh:

    make mysh
    
Mysh can be run in two ways:

Interactive mode: Commands can be entered in real time

    ./mysh
    
Batch mode: Takes a file of shell scripts named file_name and executes them line by line
    
    ./mysh file_name

## Design

For all our design decisions we referenced the behavior of bash on the ilabs.

### Some Notable Cases of Shell Behavior

#### Conditional Statements

When defining the behavior of conditional statements that would run after checking the return value of the previous job, we decided:
- At the very beginning, the state would be neither true nor false because no previous job was run. At least one job needs to be executed after starting mysh to use then foo or else foo
- The state of the previous job would be set to EXIT_FAILURE (1) when the user passes invalid arguments to mysh. The next job would not run if preceded by then, but would run if preceded by else
    - Ex: User enters then after a pipe
    - Ex: User enters wildcards after input or output redirection
- If a conditional statement fails to execute, the exit status of the previous command will not be updated and will be the same as the previous previous command. This will continue until a job is successfully run

#### Conflicts between piping and redirection

1. In the case that a pipeline is established between two processes but the output of the first process is redirected somewhere else, like below:

        foo > bar | baz

    The second process should have nothing to read from standard input.

2. In the case that a pipeline is established between two processes, but the input of the second process is redirected to another file, the second file will read from the redirection, not the pipe

        foo | baz < bar

    baz will set stdin to bar

## Testing

### Testing For Tokenizer/Parser

To run the tests for the tokenizer, type make token_tester, then ./token_tester.

After retrieving a command line, the line would be passed on to the tokenizer function to parse the line into tokens. 
The tokens would then be added onto the arraylist to then be executed later. 

The tokenizer made sure to follow the rules of the project which involves using the whitespace character
as a delimiter while also counting the redirection characters < and > and the pipe character | as 
tokens of their own. Edge cases were thoroughly tested which are described below. 

Test Cases: 
line[0] = "cd subdir"; //This test case would parse the line as cd and subdir as two separate tokens.

line[1] = " echo hello"; //This test case would have a whitespace in the front which we would ignore.

line[2] = "   cd sub  "; //this test case would have whitespaces in both the front and end which we would ignore.

line[3] = "foo bar < baz | quux *.txt > spam"; //this test case would test to see if both redirect and pipe characters would be counted as tokens of themselves

line[5] = "cd boo<bar"; //this test case would see if boo<bar would be separated into boo < bar

line[9] = "foo<bar<baz"; //this test case would see if multiple redirection tokens could be separated despite not having whitespaces between them

line[10] = "src/*"; //this test case would see if the wildcard expansion worked

line[13] = "> *"; //this test case would be invalid as a wildcard token cannot be right after a redirect token

line[14] = "< *.c"; //this test case would be invalid as a wildcard token cannot be right after a redirect token but this time with a file extension

line[16] = "| *"; //this test case would be invalid as a wildcard cannot be right after a pipe

line[17] = "* |"; //this test case would be invalid as a wildcard cannot be right before a pipe



### Testing The Arraylist

To run the tests for the arraylist, type make arraylist_tester, then ./arraylist_tester.

In batch mode, each line in the file would be sent to the tokenizer to parse the line for tokens.
The tokens would then be pushed onto the arraylist to be executed later. 

    arraylist_t* my_al = al_create(1); //here, the arraylist was created
    al_print(my_al); 

The arraylist is created and is empty, displaying the length as 0 with the size as 1. 

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

In the code shown above, 5 of the same characters were added to the arraylist as one line.
In the beginning, the entire alphabet was displayed with the arraylist having 26 different lines
with each line having 5 characters of the same character. 

In each iteration, a character from the end of the alphabet was removed, shrinking the line count by one.
For example, the first iteration was displayed as:

Length: 26
Size: 32

    a[0]: 	0x602000000050		 *a[0]: 	;aaaaa;
    a[1]: 	0x602000000070		 *a[1]: 	;bbbbb;
    a[2]: 	0x6020000000b0		 *a[2]: 	;ccccc;
    a[3]: 	0x6020000000d0		 *a[3]: 	;ddddd;
    a[4]: 	0x6020000000f0		 *a[4]: 	;eeeee;
    a[5]: 	0x602000000110		 *a[5]: 	;fffff;
    a[6]: 	0x602000000130		 *a[6]: 	;ggggg;
    a[7]: 	0x602000000150		 *a[7]: 	;hhhhh;
    a[8]: 	0x602000000170		 *a[8]: 	;iiiii;
    a[9]: 	0x602000000190		 *a[9]: 	;jjjjj;
    a[10]: 	0x6020000001b0		 *a[10]: 	;kkkkk;
    a[11]: 	0x6020000001d0		 *a[11]: 	;lllll;
    a[12]: 	0x6020000001f0		 *a[12]: 	;mmmmm;
    a[13]: 	0x602000000210		 *a[13]: 	;nnnnn;
    a[14]: 	0x602000000230		 *a[14]: 	;ooooo;
    a[15]: 	0x602000000250		 *a[15]: 	;ppppp;
    a[16]: 	0x602000000270		 *a[16]: 	;qqqqq;
    a[17]: 	0x602000000290		 *a[17]: 	;rrrrr;
    a[18]: 	0x6020000002b0		 *a[18]: 	;sssss;
    a[19]: 	0x6020000002d0		 *a[19]: 	;ttttt;
    a[20]: 	0x6020000002f0		 *a[20]: 	;uuuuu;
    a[21]: 	0x602000000310		 *a[21]: 	;vvvvv;
    a[22]: 	0x602000000330		 *a[22]: 	;wwwww;
    a[23]: 	0x602000000350		 *a[23]: 	;xxxxx;
    a[24]: 	0x602000000370		 *a[24]: 	;yyyyy;
    a[25]: 	0x602000000390		 *a[25]: 	;zzzzz;

The second iteration would remove the 26th line by removing a random alphabet character and so forth
until number of lines hit 0. 

After the tests were completed, the array was destroyed. 
al_destroy(my_al);

### Testing Mysh

We made mock programs which interact with arguments, take input from stdin, and write to stdout to test the mysh's pipe and redirection functionality. We also wrote some shell scripts in src/tests to run in mysh's batch mode. We even spawned mysh within mysh to test its robustness.

The following are useful to test mysh but not necessary for mysh to run:

reader: reads from stdin, prints to stdout, stopping after the first encounter of z

        make reader
        ./reader

./print_args: prints the number of args and each arg to std out

        make print_args
        ./print_args