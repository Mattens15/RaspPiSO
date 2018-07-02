#include <stdio.h>
#include <string.h>




//crating library for control camera from c or c++
void Capture()// this function capture the image and save it in img.jpg
{
    char command[50];
    strcpy(command,"raspistill -o test.jpg ");
    system(command);
   
    
}
