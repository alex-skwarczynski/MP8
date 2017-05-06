/* 
File: requestchannel.C

Author: R. Bettati
Department of Computer Science
Texas A&M University
Date  : 2012/07/11

*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <cassert>
#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <errno.h>

#include "reqchannel.h"

using namespace std;

/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR/DESTRUCTOR FOR CLASS   R e q u e s t C h a n n e l  */
/*--------------------------------------------------------------------------*/


//Create a NetworkRequestChannel based on the given socket
 NetworkRequestChannel::NetworkRequestChannel(int socket){
	 sock = socket;
 }

//client
NetworkRequestChannel::NetworkRequestChannel(const string _server_host_name, const unsigned short _port_no) 
{  
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        cout << "client: error creating socket" << endl;
    }
    struct sockaddr_in ip4addr;
    ip4addr.sin_family = AF_INET;
    ip4addr.sin_port = htons(_port_no);
    inet_pton(AF_INET, _server_host_name.c_str(), &ip4addr.sin_addr);
    cout << "attempting to connect" << endl;

    if (connect(sock, (struct sockaddr*)&ip4addr, sizeof ip4addr) == -1) {
        cout << "client: connection error" << endl;
    }

    cout << "finished connecting" << endl;
}

//Constructor used in Server Side to Create Channel with Specific Port Number and back log.
NetworkRequestChannel::NetworkRequestChannel(const unsigned short _port_no, void * (*connection_handler) (void*), int back_log)
{  
	struct sockaddr_in ip4addr;
    struct sockaddr_storage their_addr;
    int new_sock;
    socklen_t sin_size;

    ip4addr.sin_family = AF_INET;
    ip4addr.sin_port = htons(_port_no);
    inet_pton(AF_INET, "127.0.0.1", &ip4addr.sin_addr);

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1){
        cout << "server: error creating socket" << endl;
    }
    
    if (bind(sock, (struct sockaddr*)&ip4addr, sizeof ip4addr) == -1){
        cout << "server: error binding socket" << endl;
    }

    
    if (listen(sock, back_log) == -1){
        cout << "server: error listening to the socket" << endl;
    }

    pthread_t server_threads[back_log];
    int i = 0;

    cout << "Server: waiting for connections" << endl;

    while(1) {
        sin_size = sizeof their_addr;
        new_sock = accept(sock, (struct sockaddr *)&their_addr, &sin_size);
        
        cout << "New connection from " << new_sock << endl;

        pthread_create(&server_threads[i], NULL, connection_handler, (void *) (intptr_t) new_sock);
        i++;
    }
}

//close socket
NetworkRequestChannel::~NetworkRequestChannel() 
{     
    close(sock);
}

/*--------------------------------------------------------------------------*/
/* READ/WRITE FROM/TO REQUEST CHANNELS  */
/*--------------------------------------------------------------------------*/

const int MAX_MESSAGE = 255;

string NetworkRequestChannel::send_request(string _request) 
{
	
}

string NetworkRequestChannel::cread() 
{

}

int NetworkRequestChannel::cwrite(string _msg) 
{

}