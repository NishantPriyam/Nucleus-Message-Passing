#include "syscall.h"


int
main()
{
    
    char * buffer;
    int x;
    
    buffer = "Hello from program D";
    x = SendMessage("../test/E",buffer, -1);
    
    Exit(0);
    
    
}

