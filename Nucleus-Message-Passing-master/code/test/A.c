#include "syscall.h"


int 
main()
{

	char * buffer;
    char* answer;
    int x;
    int* result;

	buffer = "Hello from program A";
  	x = SendMessage("../test/B",buffer, -1);

  	WaitAnswer(result,answer,x);
    
    
    buffer = "Hello from program A";
    x = SendMessage("../test/C",buffer, -1);
    
    WaitAnswer(result,answer,x);

    buffer = "Hello from program A";
    x = SendMessage("../test/B",buffer, -1);

    WaitAnswer(result,answer,x);
    
    Exit(0);
//  	printf("%s\n",buffer);

}
