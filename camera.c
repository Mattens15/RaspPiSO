#include <stdio.h>
#include <string.h>
#include <time.h>



//crating library for control camera from c or c++
void Capture(int w,int h )// this function capture the image and save it in img.jpg
{   char w_s[10];
    char h_s[10];
    sprintf(w_s," -w %d ",w);
    sprintf(h_s,"-h %d ",h);
    
    char command[50];
    strcpy(command,"raspistill -o test.jpg" );
    strcat(command,w_s);
    strcat(command,h_s);
    printf("%s\n",command);
    system(command);
   
    
}
void startLoopCapturingTimer(int sec,int w,int h)
{//daimon capturing 
   while(1)
   {    printf("Sto catturando.....\n");
   	Capture(w,h);
        printf("Ho finito di catturare\n");
        printf("Sliping......\n");
   	sleep(sec);
   }
}
