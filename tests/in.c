#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
    srand(time(NULL));
    int n = rand() % 10 + 5;
    printf("%d\n",n );
    int i,x=0;
    for(i = 0 ; i < n; i++ )
    {
        int toss = rand() % 4;
        if(toss < 2) printf("%d\n",x );
        else
        {
            x = rand() % 1000;
            printf("%d\n",x );
        }

    }


}
