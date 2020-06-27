# Architecture
---
> In case of developing this shell. I started by implementing a prompt.
> Indeed, this was necessary to easily test the chdir function and to stay in the shell.
> Thus, I needed to implement two main main functions one for read the line entered by the user and another one to split the line in args where the first argument is the command and the others are its arguments.


> In order to simplify my code, I separated functions and prototypes with headers and sources.
> I created some string utils to help me in certain function with strings.
> Thus, the core of the project is in dumsh.c/dumsh.h.


> I thought to be more efficient and maintainable to create an array of function which I could execute easily where it contains the address of each built in commands related to an array of strings which are the built in command names.


> In order to execute the commands, I created the function dumsh_execute which verify if the command entered belongs to the built in commands implemented or if it is a program like bash commands or others (even /bin/bash).


> Inside this one, I created a function dumsh_launch which launch another program and if there are any redirections then it wait for pid to finish else we don't wait for pid to finish and rewrite prompt to the terminal.


> For the first redirection, as we needed to write the standard error at the end of the file separated by #, I thought it was interesting to save the standard error for built in commands. But, I had a problem to save standard error belonging to a forked pid. Therefore, I just saved it to an err.tmp file and appended the filename with the content of err.tmp and then removed it when exited.
> For the second redirection, it was more simple, I just redirect the standard output into the filename and saved the name concatened with .err extension and redirected standard error into it.