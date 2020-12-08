#pragma once
//#include "spsc_queue.h"

void __cdecl process(void*);
void try_connect();

struct sending
{
	sending()
	{
		//data_to_send.clear();
		//request = 0;
	}

	//sending(std::string s) : data_to_send(std::move(s)) { request = 0; }
	//std::string data_to_send;
	//size_t request;
};

//extern spsc_queue<sending> data_to_send;
