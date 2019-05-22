#pragma once
#include <iostream>
#include <Windows.h>
#include <string>


class ComLib
{
public :
	enum TYPE { PRODUCER, CONSUMER };
	enum MSGTYPE { REAL, FAKE };

	HANDLE hFileMap;
	void* mData;
	bool exists = false;
	unsigned int mSize = 1 << 10;

	unsigned int freemem;
	unsigned int totalMemory;
	size_t* head;
	size_t* tail;
	char * base;

	ComLib(const std::string& secret, const size_t& buffSize, TYPE type);
	bool send(const void* msg, const size_t length);
	bool recv(char* msg, size_t& length);

	size_t nextSize();

	~ComLib();
	
	struct HEADER
	{
		MSGTYPE msgType;
		size_t length;
	};
};