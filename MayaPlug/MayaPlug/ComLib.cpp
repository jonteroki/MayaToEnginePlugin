#include "ComLib.h"

ComLib::ComLib(const std::string & secret, const size_t & buffSize, TYPE type)
{
	hFileMap = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		(DWORD)0,
		buffSize*mSize,
		(LPCSTR) secret.data());

	//freemem = buffSize * mSize;
	totalMemory = (buffSize * mSize) - 2 * sizeof(size_t);
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		//std::cout << "exists!" << std::endl;
	}
	if (hFileMap == FALSE)
	{
		std::cout << "File mapping failed" << GetLastError() << std::endl;
	}
	else
	{
		std::cout << "File mapping succeeded" << std::endl;

		mData = MapViewOfFile(hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		

		head = (size_t*)mData;
		tail = head + 1;
		base = (char*)(head + 2);
	}
	
	
	if (type == TYPE::PRODUCER)
	{
		*head = 0;
		*tail = 0;
	}
}

bool ComLib::send(const void * msg, const size_t length)
{
	HEADER header = { MSGTYPE::REAL, length };
	
	//compute freespace
	size_t tempTail = *tail;
   	if (*head < tempTail)
		freemem = tempTail - *head;
	else
		freemem = totalMemory - *head;
	//std::cout << sizeof(HEADER) << std::endl;

	if (length + sizeof(HEADER) * 3 < freemem)//is not bigger than freemem)
	{
		//std::cout << *head << std::endl;
		memcpy(base + (*head), &header, sizeof(HEADER));
		memcpy(base + (*head) + sizeof(HEADER), msg, length);
		//std::cout << ((HEADER*)(base + (*head)))->length << std::endl;
		*head += sizeof(HEADER) + length;
		//std::cout << (char*)msg << std::endl;
		return true;
	}
	else
	{
		//double check if head is < tail---> return false!
		//tail != 0
		if (*head < tempTail || tempTail == 0)
		{
			return false;
		}

		HEADER h = { FAKE, 0 };
		memcpy(base + (*head), &h, sizeof(HEADER));
		//reset the head to 0
		*head = 0;

		return send(msg, length);
	}
	return false;
}

bool ComLib::recv(char * msg, size_t & length)
{
	//std::cout << ((HEADER*)(base + (*tail)))->length << std::endl;
	//std::cout << sizeof(HEADER) << std::endl;
	if (head != tail)
	{
		HEADER header;

		//tempHead

		memcpy(&header, base + (*tail), sizeof(HEADER));

		if (header.msgType == MSGTYPE::REAL)
		{
			length = header.length;
			//std::cout << length << std::endl;
			//std::cout << *tail << std::endl;
			memcpy(msg, base + (*tail) + sizeof(HEADER), length);
			*tail += sizeof(HEADER) + length;
			//std::cout << *tail << std::endl;
			

			return true;
		}
		else if (header.msgType == MSGTYPE::FAKE)
		{
			if(*head != 0)
				*tail = 0;
			return recv(msg, length);
		}
	}
	else
		return false;
}

size_t ComLib::nextSize()
{
	if (*head != *tail)
	{
		HEADER header;
		memcpy(&header, base + (*tail), sizeof(HEADER));

		return header.length;
	}
	else
		return -1;
	//return size_t();
}

ComLib::~ComLib()
{
	UnmapViewOfFile((LPCVOID)mData);
	CloseHandle(hFileMap);
}
