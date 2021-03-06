#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <time.h>

int main(int argc, char* args[]){

    if(argc < 3){
        std::cout << "Usage: " << args[0] << " (cname/ip) port\n";
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

    //Bind the socket to the conexions
    bind(sd, res->ai_addr, res->ai_addrlen);

    freeaddrinfo(res);

    //Bucle ppal
    while(true){

        char buff[80];

        struct sockaddr cliente;
        socklen_t       clientelen = sizeof(struct sockaddr);

        //Receive a message and keep the client's information
        int bytes = recvfrom(sd, (void*)buff, 79, 0, &cliente, &clientelen);

        if( bytes == -1) {
            std::cerr << "Error: recibiendo socket\n";
            return -1;
        }

        buff[bytes] = '\0';

        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];
        getnameinfo(&cliente, clientelen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

        std::cout << bytes << " bytes from " << host << ":" << serv << std::endl;

        //If i receive a 'q', i close the server
        if(buff[0] == 'q'){
            std::cout << "Cerrando . . .\n";
            break;
        }

        //Get time
        time_t t = time(NULL);
        struct tm* t_ = localtime(&t);

        char timeBuff[80];
        memset((void*)&timeBuff, 0, 80);

        //Parse time
        if(buff[0] == 't'){
            strftime(timeBuff, 80, "%R", t_);
        } 
        else if(buff[0] == 'd'){
            strftime(timeBuff, 80, "%D", t_);
        }   

        //Send information to the client
        sendto(sd, timeBuff, 80, 0, &cliente, clientelen);
    }

    //Close the socket
    close(sd);

    return 0;
}