#include "syscall.h"


int
main()
{
    char * buffer;
    int x;
    int* result;
    
    x = WaitMessage("../test/A",buffer,-1);
    //cout << buffer << "\n";
    
    buffer = "Answer from program C to program A";
    
    
    SendAnswer(result,buffer,x);
}

