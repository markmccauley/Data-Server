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

#include "NetworkRequestChannel.h"

using namespace std;

void handle_network_process_loop(NetworkRequestChannel &channel);

void* handle_data_requests(void * args) {
	NetworkRequestChannel * data_channel =  (NetworkRequestChannel*) args;

	handle_network_process_loop(*data_channel);

	delete data_channel;
	return NULL;
}

void process_network_request(NetworkRequestChannel & _channel, string & _request) {
	if (_request.compare(0, 5, "hello") == 0) {
		_channel.cwrite("hello to you too");
	}
	else if (_request.compare(0, 4, "data") == 0) {
		usleep(1000 + (rand() % 5000));
		_channel.cwrite(to_string(rand() % 100));
	} else {
		_channel.cwrite("unknown request");
	}

}

void handle_network_process_loop(NetworkRequestChannel & _channel) {
	for(;;) {
		string request = _channel.cread();

		if (request.compare("quit") == 0) {
			_channel.cwrite("bye");
			usleep(10000);          // give the other end a bit of time.
			break;                  // break out of the loop;
		}
		process_network_request(_channel, request);
	}

}

void *connection_handler(void *sock)
{
	int s = *(int*)sock;
	NetworkRequestChannel _channel(s);
	
	handle_network_process_loop(_channel);

    close(s);
    free(sock); 
}

/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

int main(int argc, char ** argv) {
	int opt = 0;
	string i = "n";
	string serverport_num = "11111";
	while((opt = getopt(argc,argv,"i:p:"))!= -1) {
		switch(opt) {
			case 'i':
				i = strdup(optarg);
			case 'p':
				serverport_num = optarg;
				break;
		}
	}
		
	cout << "Server port number: " << serverport_num << endl;
		
	string port_num = serverport_num;
	NetworkRequestChannel server_side(port_num, connection_handler);
		
}