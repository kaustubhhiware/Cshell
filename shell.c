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
#include <fcntl.h>
#define BUFSIZE 1000
#define INPBUF 100
#define ARGMAX 10
#define GREEN "\x1b[92m"
#define BLUE "\x1b[94m"
#define DEF "\x1B[0m"
#define CYAN "\x1b[96m"
// colour guide - https://github.com/shiena/ansicolor/blob/master/README.md
/*
* Assignment 2 part 2 - shell.c
* 14CS30011 : Hiware Kaustubh Narendra
* Usage - make clean ; clear ; make ; ./Cshell
*/
/* implement shell functions
*/
/* Instructions to support :
*  cd <dir>                                 - Done cd
*  pwd                                      - Done pwd
*  mkdir <dir>                              - Done mkdir
*  rmdir <dir>                              - Done rmdir
*  ls (support ls -l)                       - Done ls & -l
*  cp <file1> <file2>                       - Done cp
*  exit                                     - Done exit
*  execute any other command like ./a.out   - TODo [WIP]
* support background execution - &          - Done &
* redirect input output >, <                - TODo
*  a.out | b.out - must support a| b| c     - TODo
* Additional - clear
* Additional - screenfetch
* Additional - up down keys, history- ?
* Additional - tab keys autocompletiton
*/

struct _instr
{
    char * argval[INPBUF];
    int argcount;
};
typedef struct _instr instruction;

char *input,*input1;
//char input[1000];
int exitflag = 0;
int filepid,fd[2];
char cwd[BUFSIZE];
char* argval[ARGMAX]; // our local argc, argv
int argcount = 0,inBackground = 0;
int externalIn=0,externalOut=0;
char inputfile[INPBUF],outputfile[INPBUF];
void screenfetch();
void about();
void getInput();
int function_exit();
void function_pwd(char*, int);
void function_cd(char*);
void function_mkdir(char*);
void function_rmdir(char*);
void function_clear();
void nameFile(struct dirent*, char*);
void function_ls();
void function_lsl();
void function_cp(char*, char*);
void executable();
void pipe_dup(int, instruction*);
void run_process(int, int, instruction*);

/*Stop processes if running in terminal(a.out), close terminal if only Ctrl+C*/
void stopSignal()
{

    if(filepid!=0)
    {
        int temp = filepid;
        kill(filepid, SIGINT);
        filepid = 0;

    }
}

int main(int argc, char* argv[])
{
    signal(SIGINT,stopSignal);
    int i;
    int pipe1 = pipe(fd);
    function_clear();
    screenfetch();
    function_pwd(cwd,0);

    while(exitflag==0)
    {
        externalIn = 0; externalOut = 0;inBackground = 0;
        printf("%s%s ~> ",DEF,cwd ); //print user prompt
        // scanf("%s",input); - fails due to spaces, tabs
        getInput();

        if(strcmp(argval[0],"exit")==0 || strcmp(argval[0],"z")==0)
        {
            function_exit();
        }
        else if(strcmp(argval[0],"screenfetch")==0 && !inBackground)
        {
            screenfetch();
        }
        else if(strcmp(argval[0],"about")==0 && !inBackground)
        {
            about();
        }
        else if(strcmp(argval[0],"pwd")==0 && !inBackground)
        {
            function_pwd(cwd,1);
        }
        else if(strcmp(argval[0],"cd")==0 && !inBackground)
        {
            char* path = argval[1];
            function_cd(path);
        }
        else if(strcmp(argval[0],"mkdir")==0 && !inBackground)
        {
            char* foldername = argval[1];
            function_mkdir(foldername);
        }
        else if(strcmp(argval[0],"rmdir")==0 && !inBackground)
        {
            char* foldername = argval[1];
            function_rmdir(foldername);
        }
        else if(strcmp(argval[0],"clear")==0 && !inBackground)
        {
            function_clear();
        }
        else if(strcmp(argval[0],"ls")==0 && !inBackground)
        {
            char* optional = argval[1];
            if(strcmp(optional,"-l")==0 && !inBackground)
            {
                function_lsl();
            }
            else function_ls();
        }
        else if(strcmp(argval[0],"cp")==0 && !inBackground)
        {
            char* file1 = argval[1];
            char* file2 = argval[2];
            if(argcount > 2 && strlen(file1) > 0 && strlen(file2) > 0)
            {
                function_cp(file1,file2);
            }
            else
            {
                printf("+--- Error in cp : insufficient parameters\n");
            }
        }
        else
        {
            executable();
        }

    }

}


/*get input containing spaces and tabs and store it in argval*/
void getInput()
{
    fflush(stdout); // clear all previous buffers if any
    input = NULL;
    ssize_t buf = 0;
    getline(&input,&buf,stdin);
    // Copy into another string if we need to run special executables
    input1 = (char *)malloc(strlen(input) * sizeof(char));
    strncpy(input1,input,strlen(input));
    argcount = 0;inBackground = 0;
    while((argval[argcount] = strsep(&input, " \t\n")) != NULL && argcount < ARGMAX-1)
    {
        // do not consider "" as a parameter
        if(sizeof(argval[argcount])==0)
        {
            free(argval[argcount]);
        }
        else argcount++;
        if(strcmp(argval[argcount-1],"&")==0)
        {
            inBackground = 1; //run in inBackground
            return;
        }
    }
    free(input);
}


// Next 2 functions are called by executable() */
/* use execvp to run the command, check path, and handle erors*/
void runprocess(char * cli, char* args[], int count)
{
    /*int i;
    printf("open file now\n");
    printf("cli : %s\n",cli );
    for(i = 0; i < count; i++ )
    {
        printf("arg %d %s \n",i,args[i] );
    }
    printf("\n" );
    */
    int ret = execvp(cli, args);
    char* pathm;
    pathm = getenv("PATH");// check for all paths , executing non./
    char path[1000];
    strcpy(path, pathm);
    strcat(path,":");
    strcat(path,cwd);
    char * cmd = strtok(path, ":\r\n");
    while(cmd!=NULL)
    {
       char loc_sort[1000];
        strcpy(loc_sort, cmd);
        strcat(loc_sort, "/");
        strcat(loc_sort, cli);
        printf("execvp : %s\n",loc_sort );
        ret = execvp(loc_sort, args);
        if(ret==-1)
        {
            perror("+--- Error in running executable ");
            exit(0);
        }
        cmd = strtok(NULL, ":\r\n");
    }
}


/* create pipes if required and send proper formatted commands to run_process */
void pipe_dup(int n, instruction* command)
{
    int in = 0,fd[2], i;
    int pid, status,pipest;

    if(externalIn)
    {
        in = open(inputfile, O_RDONLY); // open the file
        if(in < 0)
        {
            perror("+--- Error in executable : input file ");
        }
    }
    //printf("%d %d\n",command[0].argcount,n );
    // enters only in case of pipes
    for (i = 1; i < n; i++)
    {
        /*printf("Input ");
        int k;
        for(k=0;k<command[0].argcount;k++)
        {
            printf("%s ",command[0].argval[k]);
        }*/
        //printf("i is in loop : %d %s\n",i-1,command[i-1].argval[0]);
        pipe (fd);// fd[0] => fd[1] i.e, r=>w
        int id = fork();
        if (id==0)
        {

            if (in!=0)
            {
                dup2(in, 0);
                close(in);
            }
            if (fd[1]!=1)
            {
                dup2(fd[1], 1);
                close(fd[1]);
            }

            //printf("pipe_dup send %s to runprocess\n",command[i].argval[0]);
            runprocess(command[i-1].argval[0], command[i-1].argval,command[i-1].argcount);
            exit(0);

        }
        else wait(&pipest);
        close(fd[1]);
        in = fd[0]; // store input for next child, it there is one
    }
    i--; // for final case
    // keep a copy of current file descriptor
    if(in != 0)
    {
        dup2(in, 0);
    }
    if(externalOut)
    {
        int ofd = open(outputfile, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        dup2(ofd, 1);
    }
    //  printf("cli sent from pipe_dup : %s\n",command[i].argval[0]);
    runprocess(command[i].argval[0], command[i].argval, command[i].argcount);
}


/* executables like ./a.out */
void executable()
{
    instruction command[INPBUF];
    int i=0,j=1,status;
    char* curr = strsep(&input1," \t\n");// need to do all over again
                                // since we need to identify distinct commands
    command[0].argval[0] = curr;

    while(curr!=NULL)
    {
        curr = strsep(&input1, " \t\n");
        if(curr==NULL)
        {
            command[i].argval[j++] = curr;
        }
        else if(strcmp(curr,"|")==0)
        {
            command[i].argval[j++] = NULL;
            command[i].argcount = j;
            j = 0;i++;// move to the next instruction
        }
        else if(strcmp(curr,"<")==0)
        {
            externalIn = 1;
            curr = strsep(&input1, " \t\n");
            strcpy(inputfile, curr);
        }
        else if(strcmp(curr,">")==0)
        {
            externalOut = 1;
            curr = strsep(&input1, " \t\n");
            strcpy(outputfile, curr);
        }
        else if(strcmp(curr, "&")==0)
        {
            inBackground = 1;
        }
        else
        {
            command[i].argval[j++] = curr;
        }
    }
    command[i].argval[j++] = NULL; // handle last command separately
    command[i].argcount = j;
    i++;

    // parent process waits for execution and then reads from terminl
    filepid = fork();
    if(filepid == 0)
    {
        pipe_dup(i, command);
    }
    else
    {
        if(inBackground==0)
        {
            waitpid(filepid,&status, 0);
        }
        else
        {
            printf("+--- Process running in inBackground. PID:%d\n",filepid);
        }
    }
    filepid = 0;
    free(input1);
}


/* copy one file to another */
void function_cp(char* file1, char* file2)
{
    FILE *f1,*f2;
    struct stat t1,t2;
    f1 = fopen(file1,"r");
    if(f1 == NULL)
    {
        perror("+--- Error in cp file1 ");
        return;
    }
    f2 = fopen(file2,"r");// if file exists
    if(f2)
    {
        // file2 exists
        // file1 must be more recently updated
        stat(file1, &t1);
        stat(file2, &t2);
        if(difftime(t1.st_mtime,t2.st_mtime) < 0)
        {
            printf("+--- Error in cp : %s is more recently updated than %s\n",file2,file1);
            fclose(f1);
            fclose(f2);
            return;
        }
    }
    f2 = fopen(file2,"ab+"); // create the file if it doesn't exist
    fclose(f2);

    f2 = fopen(file2,"w+");
    if(f2 == NULL)
    {
        perror("Error in cp file2 ");
        fclose(f1);
        return;
    }
    //if(access(file2,W_OK)!=0 || access(file1,R_OK)!=0 || access(file2,F_OK)!=0)
    if(open(file2,O_WRONLY)<0 || open(file1,O_RDONLY)<0)
    {
        perror("Error in cp access ");
        return;
    }
    char cp;
    while((cp=getc(f1))!=EOF)
    {
        putc(cp,f2);
    }

    fclose(f1);
    fclose(f2);
}


/* Just a fancy name printing function*/
void nameFile(struct dirent* name,char* followup)
{
    if(name->d_type == DT_REG)          // regular file
    {
        printf("%s%s%s",BLUE, name->d_name, followup);
    }
    else if(name->d_type == DT_DIR)    // a directory
    {
        printf("%s%s/%s",GREEN, name->d_name, followup);
    }
    else                              // unknown file types
    {
        printf("%s%s%s",CYAN, name->d_name, followup);
    }
}


/*ls -l  lists date permissions etc*/
void function_lsl()
{
    int i=0,total=0;
    char timer[14];
    struct dirent **listr;
    struct stat details;
    int listn = scandir(".",&listr,0,alphasort);
    if(listn > 0)
    {
        printf("%s+--- Total %d objects in this directory\n",CYAN,listn-2);
        for ( i = 0; i < listn; i++)
        {
            if(strcmp(listr[i]->d_name,".")==0 || strcmp(listr[i]->d_name,"..")==0)
            {
                continue;
            }
            else if(stat(listr[i]->d_name,&details)==0)
            {
                // example - -rwxrwxr-x 1 user user  8872 Jan 26 10:19 a.out*
                // owner permissions - group permissions - other permissions
                // links associated - owner name - group name
                // file size (bytes) - time modified - name
                total += details.st_blocks; // block size
                // owner permissions - group permissions - other permissions
                printf("%s%1s",DEF,(S_ISDIR(details.st_mode)) ? "d" : "-");
                printf("%s%1s",DEF,(details.st_mode & S_IRUSR) ? "r" : "-");
                printf("%s%1s",DEF,(details.st_mode & S_IWUSR) ? "w" : "-");
                printf("%s%1s",DEF,(details.st_mode & S_IXUSR) ? "x" : "-");
                printf("%s%1s",DEF,(details.st_mode & S_IRGRP) ? "r" : "-");
                printf("%s%1s",DEF,(details.st_mode & S_IWGRP) ? "w" : "-");
                printf("%s%1s",DEF,(details.st_mode & S_IXGRP) ? "x" : "-");
                printf("%s%1s",DEF,(details.st_mode & S_IROTH) ? "r" : "-");
                printf("%s%1s",DEF,(details.st_mode & S_IWOTH) ? "w" : "-");
                printf("%s%1s ",DEF,(details.st_mode & S_IXOTH) ? "x" : "-");
                // links associated - owner name - group name
                printf("%2ld ",(unsigned long)(details.st_nlink));
                printf("%s ",(getpwuid(details.st_uid))->pw_name);
                printf("%s ",(getgrgid(details.st_gid))->gr_name);
                // file size (bytes) - time modified - name
                printf("%5lld ",(unsigned long long)details.st_size);
                strftime (timer,14,"%h %d %H:%M",localtime(&details.st_mtime));
                printf("%s ",timer);
                nameFile(listr[i],"\n");
            }
        }
        printf("%s+--- Total %d object contents\n",CYAN,total/2);
    }
    else
    {
            printf("+--- Empty directory\n" );
    }
}

/* list cwd contents*/
void function_ls()
{
    int i=0;
    struct dirent **listr;
    int listn = scandir(".", &listr, 0, alphasort);
    if (listn >= 0)
    {
        printf("%s+--- Total %d objects in this directory\n",CYAN,listn-2);
        for(i = 0; i < listn; i++ )
        {
            if(strcmp(listr[i]->d_name,".")==0 || strcmp(listr[i]->d_name,"..")==0)
            {
                continue;
            }
            else nameFile(listr[i],"    ");
            if(i%8==0) printf("\n");
        }
        printf("\n");
    }
    else
    {
        perror ("+--- Error in ls ");
    }

}


/* clear the screen*/
void function_clear()
{
    const char* blank = "\e[1;1H\e[2J";
    write(STDOUT_FILENO,blank,12);
}

/* remove folder */
void function_rmdir(char* name)
{
    int statrm = rmdir(name);
    if(statrm==-1)
    {
        perror("+--- Error in rmdir ");
    }
}


/* Make folder */
void function_mkdir(char* name)
{
    int stat = mkdir(name, 0777);// all appropriate permissions
    if(stat==-1)
    {
        perror("+--- Error in mkdir ");
    }
}


/*change directory functionality*/
void function_cd(char* path)
{
    int ret = chdir(path);
    if(ret==0) // path could be changed if cd successful
    {
        function_pwd(cwd,0);
    }
    else perror("+--- Error in cd ");
}


/*Implement basic exit*/
int function_exit()
{
    exitflag = 1;
    return 0; // return 0 to parent process in run.c
}

/* Implement pwd function in shell - 1 prints, 0 stores*/
void function_pwd(char* cwdstr,int command)
{
    char temp[BUFSIZE];
    char* path=getcwd(temp, sizeof(temp));
    if(path != NULL)
    {
        strcpy(cwdstr,temp);
        if(command==1)  // check if pwd is to be printed
        {
            printf("%s\n",cwdstr);
        }
    }
    else perror("+--- Error in getcwd() : ");

}

/* mimic screenfetch like logo functionality from ubuntu*/
void screenfetch()
{
    char* welcomestr = "\n                           ./+o+-\n                  yyyyy- -yyyyyy+\n               ://+//////-yyyyyyo\n           .++ .:/++++++/-.+sss/`\n         .:++o:  /++++++++/:--:/-\n        o:+o+:++.`..```.-/oo+++++/\n       .:+o:+o/.          `+sssoo+/\n  .++/+:+oo+o:`             /sssooo.\n /+++//+:`oo+o               /::--:.\n \\+/+o+++`o++o               ++////.\n  .++.o+++oo+:`             /dddhhh.\n       .+.o+oo:.          `oddhhhh+\n        \\+.++o+o``-````.:ohdhhhhh+\n         `:o+++ `ohhhhhhhhyo++os:\n           .o:`.syhhhhhhh/.oo++o`\n               /osyyyyyyo++ooo+++/\n                   ````` +oo+++o\\:    CShell\n                          `oo++.    Made by Kaustubh Hiware\n\n";

    printf("%s",welcomestr);
}


/* about this shell*/
void about()
{
    // Source - http://ascii.co.uk/art/seashell
    char* descr = "           _.-''|''-._\n        .-'     |     `-.\n      .'\\       |       /`.\n    .'   \\      |      /   `.        Cshell\n    \\     \\     |     /     /        Made by @kaustubhhiware\n     `\\    \\    |    /    /'\n       `\\   \\   |   /   /'\n         `\\  \\  |  /  /'\n        _.-`\\ \\ | / /'-._ \n       {_____`\\\\|//'_____}\n               `-'\n\n";

    printf("%s",descr);
}
