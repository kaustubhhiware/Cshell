#include<stdlib.h>
#include<stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <grp.h>
#include <pwd.h>
#include <time.h>
/*
* Assignment 2 part 2 - shell.c
* 14CS30011 : Hiware Kaustubh Narendra
*/
/* implement basic shell functions
*/
/* Instructions to support :
*  cd <dir>
*  pwd
*  mkdir <dir>
*  rmdir <dir>
*  ls (support ls -l)
*  cp <file1> <file2>
*  exit
*  execute any other function like ./a.out
*/
void screenfetch();

int main()
{
    screenfetch();
}


/* mimic screenfetch functionality from ubuntu*/
void screenfetch()
{
    char* welcomestr = "\n                           ./+o+-\n                  yyyyy- -yyyyyy+\n               ://+//////-yyyyyyo\n           .++ .:/++++++/-.+sss/`\n         .:++o:  /++++++++/:--:/-\n        o:+o+:++.`..```.-/oo+++++/\n       .:+o:+o/.          `+sssoo+/\n  .++/+:+oo+o:`             /sssooo.\n /+++//+:`oo+o               /::--:.\n \\+/+o+++`o++o               ++////.\n  .++.o+++oo+:`             /dddhhh.\n       .+.o+oo:.          `oddhhhh+\n        \\+.++o+o``-````.:ohdhhhhh+\n         `:o+++ `ohhhhhhhhyo++os:\n           .o:`.syhhhhhhh/.oo++o`\n               /osyyyyyyo++ooo+++/\n                   ````` +oo+++o\\:\n                          `oo++.\n\n";
    printf(" Welcome to Cshell by @kaustubhhiware\n%s",welcomestr);


}
