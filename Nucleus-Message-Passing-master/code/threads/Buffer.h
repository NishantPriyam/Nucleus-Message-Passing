//Buffer.h

#ifndef BUFFER_H
#define BUFFER_H

class Buffer {
	
	
public:
	char* receiver;
	char* sender;

	char* message;

	int id;

	bool valid;

	bool operator== (Buffer &b) { return this->id == b.id;}

};

#endif