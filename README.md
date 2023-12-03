# My Shell 

#### Project 3 for Systems Programming (CS214) at Rutgers University New Brunswick

## Collaborators:

Dhvani Patel (dgp69)

Hyeon Min Oh (hmo18)

## How to Use

To compile our program:

    make mysh
    
Mysh can be run in two ways:

Interactive mode: Commands can be entered in real time

    ./mysh
    
Batch mode: Takes a file of shell scripts named file_name and executes them line by line
    
    ./mysh file_name

## Design

For all our design decisions we referenced the behavior of bash on the ilabs.

### Some Notable Cases of Shell Behavior

#### Conflicts between piping and redirection

1. In the case that a pipeline is established between two processes but the output of the first process is redirected somewhere else, like below:

        foo > bar | baz

    The second process should have nothing to read from standard input.

2. 


## Testing




-- delete before turning in --
turn mysh debug off
delete .git

/*

edge cases

cd
cd -
cd ~

not implemented, only need

cd /PATH/

*/

tokenizer:
- fix memory leak
- make sure wildcard expansion in another path doesn't change wd for mysh
- make sure it errors if you try to input a wildcard after > or <
    - what abt before or after pipe?

src/*
src/tests/*
< *
> *
< *.c
> *.c

wildcards before or after pipe?
* | *

pipe|
t>
t<


how are you gonna store args, argc, argv[]
    - nvm dont remove just make a new arraylist per process of just arguments

when piping where does the shell store input for the dependent process?

- check not 2 pipes
- if pipe, ????
    - check if argument follows

for one job:
    - after checking for /
    - not /
        - BUILT IN COMMANDS
            - check not first command is >, <, |
            - EXIT
            - CD
            - PWD
            - WHICH
            - THEN
            - ELSE
                - assemble args list, if >, < check if argument follows