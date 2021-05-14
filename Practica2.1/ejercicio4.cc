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

    //IPV_4 and tcp conexions
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

    freeaddrinfo(res);

    listen(sd, 16);

    struct sockaddr cliente;
    socklen_t       clientelen = sizeof(struct sockaddr);

    int cliente_sd = accept(sd, &cliente, &clientelen);

    if(cliente_sd < 0){
        std::cout << errno << std::endl;
        return -1;
    }

    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];
    getnameinfo(&cliente, clientelen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
    std::cout << "CONEXION DESDE . . . Host: " << host << "    " << "  Port: " << serv << std::endl;

    //Bucle ppal
    while(true){

        char buff[80];

        int bytes = recv(cliente_sd, (void*)buff, 79, 0);

        if( bytes <= 0) {
            std::cerr << "FIN CONEXION\n";
            break;
        }

        buff[bytes] = '\0';

        send(cliente_sd, buff, bytes, 0);
    }

    close(cliente_sd);
    close(sd);

    return 0;
}