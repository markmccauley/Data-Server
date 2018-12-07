#ifndef _NETWORKREQUESTCHANNEL_H_                   
#define _NETWORKREQUESTCHANNEL_H_

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

#include <netinet/in.h>
#include <arpa/inet.h>


using namespace std;

class NetworkRequestChannel {

private:
	int sock;

public:

    /* -- CONSTRUCTOR/DESTRUCTOR */
    NetworkRequestChannel(int socket);

    // Network constructor for client
    NetworkRequestChannel(const string server_name, const string port);

    // Network constructor for server
    NetworkRequestChannel(const string _port_no, void * (*connection_handler)(void*));

    // Network destructor
    ~NetworkRequestChannel();

    //----- override functions -----
    // return string of character read from channel
    string cread();

    // write data to channel
    void cwrite(string _msg);

};

#endif