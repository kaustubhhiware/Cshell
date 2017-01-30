//Assignment -3 11CS30036 11CS30044
//relevant headers
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <dirent.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>
#include <fcntl.h>
#include <signal.h>

//data structure for a typical command
typedef struct _command
{
   char * arguments[100];
   int numArgs;
}command;

int epid;
int fd[2];  //for pipe

//handler routine for parent
void signal_Handler()
{
   if(epid!=0)
   {
      int tempEpid = epid;
      epid = 0;
      kill(tempEpid, SIGINT);
   }
}

//handler routine for signal to input processing child
void signal_Handler_Input()
{
   printf("\n");
   close(fd[0]);
   write(fd[1], "", 1000);
   exit(0);
}

//helps in creation of date
void makeDate(time_t temp, char * date)
{
   struct tm *ts;
   ts = localtime(&temp);

   switch(ts->tm_mon)
   {
      case 0:
         strcpy(date, "Jan");
         break;
      case 1:
         strcpy(date, "Feb");
         break;
      case 2:
         strcpy(date, "Mar");
         break;
      case 3:
         strcpy(date, "Apr");
         break;
      case 4:
         strcpy(date, "May");
         break;
      case 5:
         strcpy(date, "Jun");
         break;
      case 6:
         strcpy(date, "Jul");
         break;
      case 7:
         strcpy(date, "Aug");
         break;
      case 8:
         strcpy(date, "Sep");
         break;
      case 9:
         strcpy(date, "Oct");
         break;
      case 10:
         strcpy(date, "Nov");
         break;
      case 11:
         strcpy(date, "Dec");
         break;
   }

   char day[100];
   sprintf(day, " %d %d:%d", ts->tm_mday, ts->tm_hour, ts->tm_min);
   strcat(date, day);
}

//for opening the file
void openFile(char * executable, char * arguments[],int count)
{
    int i;
    printf("Exec : %s\n",executable);
    for(i=0;i<count;i++)
    {
        printf("arg %d %s\n",i,arguments[i] );
    }
   execv(executable, arguments);
   char* pPath;
   pPath = getenv("PATH");

   char tempPath[1000];
   strcpy(tempPath, pPath);
   char * pch = strtok(tempPath, ":\r\n");
   while(pch!=NULL)
   {
      char path[1000];
      strcpy(path, pch);
      strcat(path, "/");
      strcat(path, executable);
      execv(path, arguments);
      pch = strtok(NULL, ":\r\n");
   }
      printf("Could not open file\n");
}

int backgFlag = 0, inFlag = 0, outFlag = 0;
char infile[100], outfile[100];

//for spawning the procedure
void spawn_proc(int in, int out, command * cmd)
{
  int pid;

  if ((pid = fork ()) == 0)
    {
      if (in != 0)
        {
          dup2 (in, 0);
          close (in);
        }

      if (out != 1)
        {
          dup2 (out, 1);
          close (out);
        }
      printf("%s sent as cli from proc\n",cmd->arguments[0] );
      openFile(cmd->arguments[0], cmd->arguments,cmd->numArgs);
      //execvp (cmd->arguments[0], (char * const *)cmd->arguments);
    }
}

void fork_pipes (int n, command * cmd)
{
   int i;
   int pid, status;
   int in, fd [2];

   /* The first process should get its input from the original file descriptor 0.  */
   if(inFlag)
   {
      in = open(infile, O_RDONLY);
   }
   else
      in = 0;

   /* Note the loop bound, we spawn here all, but the last stage of the pipeline.  */
   for (i = 0; i < n - 1; ++i)
      {
         pipe (fd);

         /* f [1] is the write end of the pipe, we carry `in` from the prev iteration.  */
      spawn_proc (in, fd [1], cmd + i);

      /* No need for the write and of the pipe, the child will write here.  */
      close (fd [1]);

      /* Keep the read end of the pipe, the next child will read from there.  */
      in = fd [0];
    }

  /* Last stage of the pipeline - set stdin be the read end of the previous pipe
     and output to the original file descriptor 1. */
   if (in != 0)
    dup2 (in, 0);

   if(outFlag)
   {
      int ofd = open(outfile, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
      dup2(ofd, 1);
   }
   printf("%s sent as cli from pipe\n",cmd[i].arguments[0] );
   openFile(cmd[i].arguments[0], cmd[i].arguments,cmd[i].numArgs);
   //execvp(cmd [i].arguments [0], (char * const *)cmd [i].arguments);
}

//input routine through child process
void input(char* line)
{
   fflush(stdout);
   signal(SIGINT, signal_Handler_Input);
   if(gets(line) == NULL)
   {
      close(fd[0]);
      write(fd[1], "exit", 1000);
      printf("\n");
      exit(0);
   }
   close(fd[0]);
   write(fd[1], line, 1000);
   exit(0);
}

//processing phase
void processing(char* line,int inputChild,char* pch,int status)
{
   waitpid(inputChild, &status, 0);
   close(fd[1]);
   read(fd[0], line, 1000);

   if(strcmp(line,"") == 0 || line[0] == '\n')//Enter hit
      return;

   pch = strtok(line, " \n\r");

   //cd command
   if(strcmp(pch, "cd")==0)
   {
      pch = strtok(NULL, "\n\r");
      status = chdir(pch);
      if(status == -1)
         printf("Could not change directory\n");
   }
   //pwd command
   else if(strcmp(pch, "pwd")==0)
   {
      char path[100];
      getcwd(path, 100);
      printf("%s\n", path);
   }
   //mkdir command
   else if(strcmp(pch, "mkdir")==0)
   {
      pch  = strtok(NULL, "\n\r");
      status = mkdir(pch, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

      if(status == -1)
         printf("Could not create directory\n");
   }
   //rmdir command
   else if(strcmp(pch, "rmdir")==0)
   {
      pch  = strtok(NULL, "\n\r");
      status = rmdir(pch);

      if(status == -1)
         printf("Could not remove directory\n");
   }
   //exit command
   else if(strcmp(pch, "exit")==0 || pch[0] == EOF)
   {
      exit(0);
   }
   //for executing processes
   else
   {
      //Parsing the command
      command arr[100];
      int commIdx = 0, argIdx = 1;
      arr[0].arguments[0] = pch;

      while(pch!=NULL)
      {
         pch = strtok(NULL, " \r\n");

         if(pch != NULL && strcmp(pch, "|") == 0)
         {
            arr[commIdx].arguments[argIdx++] = NULL;
            arr[commIdx].numArgs = argIdx;
            argIdx = 0;
            commIdx++;
         }
         else if(pch != NULL && strcmp(pch, "<") == 0)
         {
            inFlag = 1;
            pch = strtok(NULL, " \r\n");
            strcpy(infile, pch);
         }
         else if(pch != NULL && strcmp(pch, ">") == 0)
         {
            outFlag = 1;
            pch = strtok(NULL, " \r\n");
            strcpy(outfile, pch);
         }
         else if(pch != NULL && strcmp(pch, "&") == 0)
            backgFlag = 1;
         else//Argument of command[commIdx]
         {
            arr[commIdx].arguments[argIdx++] = pch;
         }
      }
      //Terminating last command
      arr[commIdx].arguments[argIdx++] = NULL;
      arr[commIdx].numArgs = argIdx;
      commIdx++;

      epid = fork();
      if(epid == 0)
         fork_pipes(commIdx, arr);
      else
      {
         if(!backgFlag)
            waitpid(epid, &status, 0);
         epid = 0;
      }
   }
}

//main function
int main ()
{
   signal(SIGINT, signal_Handler);

   char * line = (char *)malloc(1000*sizeof(char));
   char * pch;
   int status;

   while (1)
   {
      inFlag = 0;outFlag = 0;backgFlag = 0;

      printf("myshell> ");
      fflush(stdout);
      pipe(fd);

      //a child is forked here to handle the input effectively
      int inputChild = fork();
      if(inputChild == 0)
      {
         input(line);  //gathering input
      }
      else
      {
         processing(line,inputChild,pch,status);  //processing input
      }
   }
   exit(0);
}
