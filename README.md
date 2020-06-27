# Dumsh
---
## Authors: Yaniv Benichou.
---
> This program launch a shell with built-in commands and which can launch other programs. <br />
> It can be opened in a **terminal**.


> It's written in __Low Level C UNIX language__. <br />
> It's for an university **project C using low level programming**.

---
## Built-in commands:
	> * cd pathToGo [>][1,2][filename] - change directory
	> * help [>][1,2][filename] - print the help of the shell
	> * exit - exit the shell and return to the parent shell
---
## Redirections:
> The shell provides two redirections:
> * >1 filename - Write standard output into the filename and append with 80 '#' then apppend with standard error.
> * >2 filename - Write standard output into the filename then write the standard error into filename.err.
---
### Example of usage:
	make
	>> cd ..
	>> cd >1 fic.txt
	>> ls
	>> ls -R / >1 fic.txt
	>> exit
