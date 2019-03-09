#include "syscall.h"



int
main()
{
    char * buffer;
    int x;
    
    x = WaitMessage("../test/F",buffer,-1);    //cout << buffer << "\n";
    
    Exit(0);
}

