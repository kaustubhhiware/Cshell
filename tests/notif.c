#include <stdlib.h>
#include <string.h>
#include <unistd.h>
int main(int argc, char *argv[])
{
    char command[100], msg[100];


    strcpy(command,"notify-send ");
    strcpy(msg,"\"Hello World\"");
    strcat(command,msg);
    while(1)
    {
        system(command);
        sleep(5);
    }
    return 0;
}
