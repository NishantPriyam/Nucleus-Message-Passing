#include "syscall.h"


int
main()
{
    
    char * buffer;
    char* answer;
    int x;
    int* result;
    
    buffer = "Hello from program F";
    
    x = SendMessage("../test/G",buffer, -1);
    
    WaitAnswer(result,answer,x);
    
    Exit(0);
    
    
}

