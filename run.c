#include<stdlib.h>
#include<stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
/*
* Assignment 2 part 2 - run.c
* 14CS30011 : Hiware Kaustubh Narendra
*/
/* execute our shell process from shell.c
*/
/* get sort1 executable first - gcc shell.c -o myshell
*  create xsort executable then - gcc run.c -o shellrun
*  Usage - ./shellrun
*/
int main(int argc,char* argv[])
{
    int ret;
    char* path = getcwd(cwd, sizeof(cwd)); // current folder
    char loc_sort[1000]; // location of executable sort1
    strcpy(loc_sort,path);
    strcat(loc_sort,"/myshell");

    char* cmd = loc_sort;
    int id,status = 0;
    id = fork();
    if(id == 0)
    {
        // open xterm and hold until user exits with Ctrl+C
        ret = execvp(cmd);//,args);
        // in case of error , print
        perror("Execvp failed :/ \n");
        exit(-1);
    }
    else
    {
        // parent process - wait for child to finish
        wait(&status);
        printf("Closed shell, exit status = %d\n",status);
    }

}
