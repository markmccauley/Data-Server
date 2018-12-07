#include "NetworkRequestChannel.h"

NetworkRequestChannel::NetworkRequestChannel(int socket) {
    sock = socket;
}

// client constructor
NetworkRequestChannel::NetworkRequestChannel(const string server_name, const string port) { 
    struct addrinfo hints, *res, *p;
    int sockfd;  
    
    // load up address structs with getaddrinfo()
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    int status;

    if ((status = getaddrinfo(server_name.c_str(), port.c_str(), &hints, &res)) != 0) {
        cerr << "getaddrinfo: " << gai_strerror(status) << endl;
        exit(1);
    }

    // loop through results and connect to first available
    for(p = res; p != NULL; p = p->ai_next) {

        // create a socket
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd < 0) {
            perror("Cannot create socket");
            continue;
        }

        // connect
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("Cannot connect");
            continue;
        }
        
        break;
    }

    if (p == NULL) {
        cerr << "Client failed to connect" << endl;
        exit(1);
    }

    cout << "Successfully connected to " << server_name << endl;
    freeaddrinfo(res); // all done with this structure
    
    sock = sockfd;

}

// server constructor
NetworkRequestChannel::NetworkRequestChannel(const string _port_no, void * (*connection_handler) (void*)) {  
    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *serv, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, _port_no.c_str(), &hints, &serv)) != 0) {
        cerr << "getaddrinfo: " << gai_strerror(rv) << endl;
        exit(1);
    }

    // bind to the first result available
    for(p = serv; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("server: socket");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            exit(1);
        }

        break;
    }

    freeaddrinfo(serv); // all done with this structure

    if (p == NULL)  {
        cerr << "Server failed to bind" << endl;
        exit(1);
    }

    if (listen(sockfd, 20) == -1) {
        perror("listen");
        exit(1);
    }

    cout << "server: waiting for connections..." << endl;

    int *new_sock;
    sin_size = sizeof their_addr;

    while(1) {  // main accept() loop
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }
        cout << "server: got connection" << endl;
        
        pthread_t sniff;
        new_sock = (int*)malloc(1);
        *new_sock = new_fd;
        
        // Copy new_fd into class attribute
        sock = new_fd;   
     
        if( pthread_create(&sniff, NULL, *connection_handler, (void*) new_sock ) < 0)
        {
            perror("could not create thread");
            exit(1);
        }

    }
}

// close socket
NetworkRequestChannel::~NetworkRequestChannel() 
{     
    close(sock);
}

const int MAX_MESSAGE = 255;

string NetworkRequestChannel::cread() 
{
    char buf[MAX_MESSAGE];
 
    // receive message
    int n = recv(sock, buf, sizeof(buf), 0);
    buf[n] = '\0';
 
    return buf;
}

void NetworkRequestChannel::cwrite(string _msg) {
    char buf[MAX_MESSAGE];
 
    sprintf(buf, _msg.c_str());

    // send message
    int n = send(sock, buf, strlen(buf)+1, 0);
}