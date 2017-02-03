Cshell: myshell run.c
	gcc run.c -o Cshell

myshell: shell.c
	gcc shell.c -o myshell

clean:
	rm Cshell myshell
