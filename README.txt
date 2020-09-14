Basic shell program that is similar to bash (with more limited functionality). The shell allows for redirection of standard output and of standard input. The shell also supports both foreground and background processes. The shell supports three built in commands: exit, cd, and status. Other standard commmands are supported as well but these are executed directly with exec system calls. It also supports comments which are lines starting with the # character. 

To compile this program, enter "make" into the command line (without the quotation marks). The executable will be named smallsh

Standard input redirection is supported by the < character. Ex. wc < filename
Standard output redirection is supported by the < character. Ex. ls > filename
The exit command exits the shell and takes no arguments. All running processes/jobs that were started by the shell are terminated before the shell exits. 
Commands can be executed in the background by placing a & character at the end of the command. Ex. sleep 30 &
Any instance of $$ in a command will be replaced by the process ID of the shell itself. 
Blank lines are allowed and have no effect.
Comment lines are any lines that start with the # character. 

Signals:
A CTRL-C command terminates the foreground command being run by the shell (if any). The command has no effect on any background processes running from the shell.
A CTRL-Z command places the shell into foreground-only mode. Any commands that are entered with a & character at the end (indicating that they should be run in the background) will still be executed in the foreground. Foreground-only mode is exited by issuing the CTRL-Z command again. 


