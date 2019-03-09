// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "ksyscall.h"
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	is in machine.h.
//----------------------------------------------------------------------



void
ExceptionHandler(ExceptionType which)
{
    int type = kernel->machine->ReadRegister(2);

    DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

    switch (which) {
    case SyscallException:
      switch(type) {
      case SC_Halt:
	DEBUG(dbgSys, "Shutdown, initiated by user program.\n");

	SysHalt();

	ASSERTNOTREACHED();
	break;

      case SC_Add:
	DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");
	
	/* Process SysAdd Systemcall*/
	int result;
	result = SysAdd(/* int op1 */(int)kernel->machine->ReadRegister(4),
			/* int op2 */(int)kernel->machine->ReadRegister(5));

	DEBUG(dbgSys, "Add returning with " << result << "\n");
	/* Prepare Result */
	kernel->machine->WriteRegister(2, (int)result);
	
	/* Modify return point */
	{
	  /* set previous programm counter (debugging only)*/
	  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

	  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
	  
	  /* set next programm counter for brach execution */
	  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
	}

	return;
	
	ASSERTNOTREACHED();

	break;

	case SC_SendMessage:

	if(kernel->currentThread->messageLimit < kernel->messageCounterLimit) {

	int id;
	char* buffer;
	bool flag;
	buffer = ConvertIntoString(kernel->machine->ReadRegister(4),9);
	
	for (std::map<int,Thread*>::iterator it=kernel->bufferThreadMap.begin(); it!=kernel->bufferThreadMap.end(); ++it)
    {
    	Buffer temp = kernel->bufferPool[it->first];
    	if (strcmp(kernel->currentThread->getName(),temp.sender) == 0 && strcmp(buffer,temp.receiver) == 0) {
    		id = it->first;
    		flag = true;
    		break;
    	}
    }		
	
	if(!flag) {
		id = kernel->bufferAllocMap->FindAndSet();
	}	 
	
	

	Sys_SendMessageHelper((int)kernel->machine->ReadRegister(4),(int)kernel->machine->ReadRegister(5),id);

	kernel->currentThread->messageLimit++;

	kernel->machine->WriteRegister(2, (int)id);

	//cout << (int)kernel->machine->ReadRegister(4) << " = " << 

	}
	else{
		cout << "Message Limit per program Exceeded \n";
		Exit(0);
	}

	{
	  /* set previous programm counter (debugging only)*/
	  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

	  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
	  
	  /* set next programm counter for brach execution */
	  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
	}

	return;

	break;

	case SC_WaitMessage:

	int bufferId;
	bufferId = Sys_WaitMessageHelper((int)kernel->machine->ReadRegister(4),(int)kernel->machine->ReadRegister(5),(int)kernel->machine->ReadRegister(6));

	kernel->machine->WriteRegister(5, (int)kernel->bufferPool[bufferId].message);
	
	kernel->machine->WriteRegister(2, (int)bufferId);


	{
	  /* set previous programm counter (debugging only)*/
	  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

	  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
	  
	  /* set next programm counter for brach execution */
	  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
	}

	return;

	break;

	case SC_SendAnswer:


	Sys_SendAnswerHelper((int)kernel->machine->ReadRegister(4),(int)kernel->machine->ReadRegister(5),(int)kernel->machine->ReadRegister(6));

	kernel->machine->WriteRegister(4, 1);

	{
		  /* set previous programm counter (debugging only)*/
		  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

		  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
		  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
		  
		  /* set next programm counter for brach execution */
		  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
	}

	return;

	break;

	case SC_WaitAnswer:

	//int buffId;
	//buffId = (int)kernel->machine->ReadRegister(6);
	int res;
	res = Sys_WaitAnswerHelper((int)kernel->machine->ReadRegister(4),(int)kernel->machine->ReadRegister(5),(int)kernel->machine->ReadRegister(6));
	
	//cout << "Answer received by current program '" << kernel->currentThread->getName() << "' is: '" << ans << "'\n";

	kernel->machine->WriteRegister(4,(int)res);

	{
		  /* set previous programm counter (debugging only)*/
		  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

		  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
		  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
		  
		  /* set next programm counter for brach execution */
		  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
	}

	return;

	break;

	case SC_Exit:

	// int idRem;
	// if (kernel->bufferThreadMap.count(kernel->currentThread) > 0) {
	// 	idRem = kernel->bufferThreadMap.find(kernel->currentThread)->second;
	// 	kernel->bufferAllocMap->Clear(idRem);
	// }

	kernel->currentThread->Finish();


	return;

	break;


      default:
	cerr << "Unexpected system call " << type << "\n";
	break;
      }
      break;
    default:
      cerr << "Unexpected user mode exception" << (int)which << "\n";
      break;
    }
//    ASSERTNOTREACHED();
}
