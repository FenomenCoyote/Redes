#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <string.h>
#include <iostream>

int main(int argc, char* args[]){

    if(argc < 2){
        std::cout << "Usage: " << args[0] << " something\n";
        return 0;
    }

    struct addrinfo hints;
    struct addrinfo *res;

    memset((void*)&hints, 0, sizeof(struct addrinfo));

    //No hints
    hints.ai_family = AF_UNSPEC;

    int rc = getaddrinfo(args[1], NULL, &hints, &res);

    if( rc != 0){
        std::cerr << "Error: " << gai_strerror(rc) << std::endl;
        return -1;
    }

    //Iterate the list
    for(addrinfo* i = res; i != NULL; i = i->ai_next) {
        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];

        getnameinfo(i->ai_addr, i->ai_addrlen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST);

        std::cout << host << "    " << i->ai_family << "    " << i->ai_socktype << std::endl;
    }

    freeaddrinfo(res);

    return 0;
}