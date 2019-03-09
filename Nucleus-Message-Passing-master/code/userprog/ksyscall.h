/**************************************************************
 *
 * userprog/ksyscall.h
 *
 * Kernel interface for systemcalls 
 *
 * by Marcus Voelp  (c) Universitaet Karlsruhe
 *
 **************************************************************/

#ifndef __USERPROG_KSYSCALL_H__ 
#define __USERPROG_KSYSCALL_H__ 

#include "kernel.h"
#include <string>





void SysHalt()
{
  kernel->interrupt->Halt();
}


int SysAdd(int op1, int op2)
{
  return op1 + op2;
}

char* ConvertIntoString(int val,int size) {
	char* ch = new char[2*size];
	DEBUG(dbgSys, "Writing using Systemcall.\n");
	for(int i=0;i<size;i++){
		//If there is a page fault then return false.
		kernel->machine->ReadMem(val + i,1,(int*)&ch[i]);
		
	}

	return ch;
}



void Sys_SendMessage(char* receiverName, char* message, int bufferId) {	

	kernel->bufferPool[bufferId].sender = kernel->currentThread->getName();
	kernel->bufferPool[bufferId].receiver = receiverName;
	kernel->bufferPool[bufferId].message = message;
	kernel->bufferPool[bufferId].id = bufferId;

	string s(receiverName);

	cout << "Program '" << kernel->currentThread->getName() << "' is sending message to program '" << receiverName << "'\n";	

	 if(kernel->threadMap.find(s)->second != NULL) {	 	
		Thread* receiver = kernel->threadMap.find(s)->second;
		receiver->messageQueue->Append(kernel->bufferPool[bufferId]);
		if(!kernel->scheduler->readyList->IsInList(receiver)) {			
			IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);
			kernel->scheduler->ReadyToRun(receiver);
			(void) kernel->interrupt->SetLevel(oldLevel);
		}
	}

}

void Sys_SendMessageHelper(int receiverName,int message,int bufferId) {
	char* receiver = ConvertIntoString(receiverName,9);
	char* mess = ConvertIntoString(message,21);

	Sys_SendMessage(receiver,mess,bufferId);
}




int Sys_WaitMessage(char* senderName,int bufferId) {	
	string s(senderName);
	cout << "Program '" << kernel->currentThread->getName() << "' is Waiting for message from program '" << senderName << "'\n";	
	while(kernel->currentThread->messageQueue->IsEmpty()) {
		IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);
		kernel->currentThread->Sleep(FALSE);
		(void) kernel->interrupt->SetLevel(oldLevel);
	}	


	Buffer buffer = kernel->currentThread->messageQueue->RemoveFront();
	Thread* sender = kernel->threadMap.find(s)->second;

	cout << "Message received by current program '" << kernel->currentThread->getName() << "' is: '" << buffer.message << "'\n";		


	if (sender!=NULL) {

	kernel->bufferThreadMap[buffer.id] = kernel->currentThread;
	//cout << "Message received by current program '" << kernel->currentThread->getName() << "' is: '" << buffer.message << "'\n";		

	if(!kernel->scheduler->readyList->IsInList(sender)){		
		IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);
		kernel->scheduler->ReadyToRun(sender);
		(void) kernel->interrupt->SetLevel(oldLevel);
	}
	}

	return buffer.id;
	
}

int Sys_WaitMessageHelper(int senderName,int message,int bufferId) {
	char* sender = ConvertIntoString(senderName,9);
	return Sys_WaitMessage(sender,bufferId);
}

void Sys_SendAnswer(char* answer,int bufferId) {
	string s(kernel->bufferPool[bufferId].sender);
	Thread* sender = kernel->threadMap.find(s)->second;


	if(sender != NULL) {	
		//int id = kernel->bufferThreadMap.find(sender)->second;
		kernel->bufferPool[bufferId].valid = TRUE;
		kernel->bufferPool[bufferId].message = answer;
		sender->messageQueue->Append(kernel->bufferPool[bufferId]);
		cout << "Program '" << kernel->currentThread->getName() << "' is Sending answer to program '" << s << " using buffer id " << bufferId << "\n";	
	}

}

void Sys_SendAnswerHelper(int result,int message,int bufferId) {
	//char* sender = ConvertIntoString(senderName,9);
	char* mess = ConvertIntoString(message,34);
	Sys_SendAnswer(mess,bufferId);
}

int Sys_WaitAnswer(int result, int answer, int bufferId) {	
	string s(kernel->bufferPool[bufferId].receiver);
	int id;

	while(kernel->currentThread->messageQueue->IsEmpty()) {		
		if (kernel->threadMap.find(s)->second!= NULL) {			
			IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);
			kernel->currentThread->Sleep(FALSE);
			(void) kernel->interrupt->SetLevel(oldLevel);
		}
		else{
			kernel->bufferAllocMap->Clear(bufferId);
	 		kernel->bufferThreadMap.erase(bufferId);
			cout << "Kernel Response because either program does not exist or it finished before sending an answer\n";
			return 0;
		}
	}

		
	if (kernel->threadMap.find(s)->second != NULL || kernel->bufferPool[bufferId].valid) {
		
		Buffer buffer = kernel->currentThread->messageQueue->RemoveFront();
		

	 	cout << "Answer received by current program '" << kernel->currentThread->getName() << "' is: '" << buffer.message << "'\n";
	 }
	 else{

	 	kernel->bufferAllocMap->Clear(bufferId);
	 	kernel->bufferThreadMap.erase(bufferId);
	 	cout << "Kernel Response because either program does not exist or it finished before sending an answer\n";
	 	return 0;
	 }

	 return 1;

	} 

int Sys_WaitAnswerHelper(int result, int answer, int bufferId) {
	cout << "Program '" << kernel->currentThread->getName() << "' is Waiting for answer from program '" << kernel->bufferPool[bufferId].receiver << "'\n";			
	return Sys_WaitAnswer(result,answer,bufferId);
}




#endif /* ! __USERPROG_KSYSCALL_H__ */
