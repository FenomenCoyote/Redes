#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <errno.h>

int main(int argc, char* args[]){

    if(argc < 3){
        std::cout << "Usage: " << args[0] << " (cname/ip) port\n";
        return 0;
    }

    struct addrinfo hints;
    struct addrinfo *res;

    memset((void*)&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int rc = getaddrinfo(args[1], args[2], &hints, &res);

    if( rc != 0 ){
        std::cerr << "Error: " << gai_strerror(rc) << std::endl;
        return -1;
    }

    int sd = socket(res->ai_family, res->ai_socktype, 0);

    if( sd == -1 ){
        std::cerr << "Error: creacion soccket\n";
        return -1;
    }

    bind(sd, res->ai_addr, res->ai_addrlen);

    connect(sd, res->ai_addr, res->ai_addrlen);

    char buff[80];
    //Bucle ppal
    while(true){

        memset((void*)&buff, 0, 80);

        std::cin >> buff;

        send(sd, buff, 80, 0);

        int bytes = recv(sd, (void*)buff, 79, 0);

        if( bytes <= 0) {
            std::cerr << "FIN CONEXION\n";
            break;
        }

        buff[bytes] = '\0';

        std::cout << buff << std::endl;
    }

    freeaddrinfo(res);
    close(sd);

    return 0;
}