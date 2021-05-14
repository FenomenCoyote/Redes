#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <time.h>

int main(int argc, char* args[]){

    if(argc < 4){
        std::cout << "Usage: " << args[0] << " ip port msg\n";
        return 0;
    }

    struct addrinfo hints;
    struct addrinfo *res;

    memset((void*)&hints, 0, sizeof(struct addrinfo));

    //IPV_4 and udp conexions
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    int rc = getaddrinfo(args[1], args[2], &hints, &res);

    if( rc != 0 ){
        std::cerr << "Error: " << gai_strerror(rc) << std::endl;
        return -1;
    }

    //Create a socket
    int sd = socket(res->ai_family, res->ai_socktype, 0);

    if( sd == -1 ){
        std::cerr << "Error: creacion soccket\n";
        return -1;
    }

    //Send msg
    sendto(sd, args[3], 80, 0, res->ai_addr, res->ai_addrlen);

    //If I expect a msg
    if(args[3][0] != 'q'){
        //Wait msg
        char buff[80];

        struct sockaddr cliente;
        socklen_t       clientelen = sizeof(struct sockaddr);

        int bytes = recvfrom(sd, (void*)buff, 79, 0, res->ai_addr, &res->ai_addrlen);

        if( bytes == -1) {
            std::cerr << "Error: recibiendo socket\n";
            return -1;
        }

        buff[bytes] = '\0';

        std::cout << buff << std::endl;
    }

    freeaddrinfo(res);
    close(sd);

    return 0;

}