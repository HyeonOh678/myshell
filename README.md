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

#### Conditional Statements

When defining the behavior of conditional statements that would run after checking the return value of the previous job, we decided:
- At the very beginning, the state would be neither true nor false because no previous job was run. At least one job needs to be executed after starting mysh to use then foo or else foo
- The state of the previous job would be set to EXIT_FAILURE (1) when the user passes invalid arguments to mysh. The next job would not run if preceded by then, but would run if preceded by else
- If a conditional statement fails to execute, the exit status of the previous command will not be updated and will be the same as the previous previous command. This will continue until a job is successfully run

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

" args in quotes "
error msg for wildcard before and after pipe works?

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


how to implement piping and redirection for built-in commands? cd, pwd, which
    - pwd, which separate processes
    - cd?
then and if ????
do we have to do error redirection?

make pwd and which their own child processes
figure out wtf to do for cd

how does shell behave if < > or | are the first arg, or >, < are directly after a pipe

exit and cd need to pipe and redirect without multiprocessing

set prev return value

test if conditional is false, then = 0, else = 1
test if conditional is true

check if then after pipe invalid
check if anything after <>
check if args before and after |