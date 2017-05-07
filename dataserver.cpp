#include <cassert>
#include <cstring>
#include <sstream>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

#include "reqchannel.h"

using namespace std;


static int nthreads = 0;

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

void handle_process_loop(NetworkRequestChannel & _channel);



/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- THREAD FUNCTIONS */
/*--------------------------------------------------------------------------*/

void * handle_data_requests(void * args) {

	NetworkRequestChannel * data_channel =  (NetworkRequestChannel*)args;

	// -- Handle client requests on this channel. 

	handle_process_loop(*data_channel);

	// -- Client has quit. We remove channel.

	delete data_channel;
	return nullptr;
}

/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- INDIVIDUAL REQUESTS */
/*--------------------------------------------------------------------------*/

void process_hello(NetworkRequestChannel & _channel, const string & _request) {
	_channel.cwrite("hello to you too");
}

void process_data(NetworkRequestChannel & _channel, const string &  _request) {
	usleep(1000 + (rand() % 5000));
	//_channel.cwrite("here comes data about " + _request.substr(4) + ": " + int2string(random() % 100));
	_channel.cwrite(to_string(rand() % 100));
}

/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- THE PROCESS REQUEST LOOP */
/*--------------------------------------------------------------------------*/

void process_request(NetworkRequestChannel & _channel, const string & _request) {
	// cout << "process request function" << endl;
	if (_request.compare(0, 5, "hello") == 0) {
		process_hello(_channel, _request);
	}
	else if (_request.compare(0, 4, "data") == 0) {
		process_data(_channel, _request);
	}
	/*
	else if (_request.compare(0, 9, "newthread") == 0) {
	process_newthread(_channel, _request);
	}
	*/
	else {
		_channel.cwrite("unknown request");
	}
	// cout << "process request function end" << endl;
}

void handle_process_loop(NetworkRequestChannel & _channel) {
	// cout << "handle process loop" << endl;
	for(;;) {
		// cout << "reading" << endl;
		string request = "";
		request = _channel.cread();

		if (request.compare("quit") == 0) {
			_channel.cwrite("bye");
			usleep(10000);          // give the other end a bit of time.
			break;                  // break out of the loop;
		}
		// cout << "processing" << endl;
		process_request(_channel, request);
	}

}

void* connection_handler(void* sock)
{
	cout << "Connection handler start." << endl;
	int s = (intptr_t) sock;
	NetworkRequestChannel dummy = NetworkRequestChannel(s);
	handle_process_loop(dummy);
	cout << "this is the connection handler for port: " << s << endl;
}

/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

int main(int argc, char ** argv) 
{
	if(argc != 5)
	{
		return 0;
	}
	else
	{
		int serverport_num;
		int backlog;
		int ch;
		opterr = 0;
		while((ch = getopt(argc,argv,"p:l:"))!= -1)
		{
			switch(ch)
			{
			case 'p':
				serverport_num = atoi(optarg);
			case 'l':
				backlog = atoi(optarg);
			default:
				break;
			}
		}
		int port_num = serverport_num;
		NetworkRequestChannel control_channel(port_num, connection_handler, backlog);
	}

}


