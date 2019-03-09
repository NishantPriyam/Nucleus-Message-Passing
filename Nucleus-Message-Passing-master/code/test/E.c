#include "syscall.h"


int
main()
{
    char * buffer;
    int x;
    int* result;
    
    x = WaitMessage("../test/D",buffer,-1);
    //cout << buffer << "\n";
    
    buffer = "Answer from program D to program E";
    
    SendAnswer(result,buffer,x);
    
    Exit(0);
}
