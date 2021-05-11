#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <time.h>
#include <thread>

#define MAX_THREAD 5

void msg(int sd, int id) { 

    char buff[80];

    struct sockaddr cliente;
    socklen_t       clientelen = sizeof(struct sockaddr);

    int bytes = recvfrom(sd, (void*)buff, 79, 0, &cliente, &clientelen);

    if( bytes == -1) {
        std::cerr << "Error: recibiendo socket\n";
        return;
    }

    buff[bytes] = '\0';

    for(int i = 8; i > 0; --i){
        std::cout << "thread [" << id << "] ready in " << i << " seconds . . ." << std::endl;
        sleep(1);
    }

    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];
    getnameinfo(&cliente, clientelen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

    std::cout << bytes << " bytes from " << host << ":" << serv << " in thread with id: " << id << std::endl;

    if(buff[0] == 'q'){
        std::cout << "Cerrando . . .\n";
        return;
    }

    time_t t = time(NULL);
    struct tm* t_ = localtime(&t);

    char timeBuff[80];
    memset((void*)&timeBuff, 0, 80);

    if(buff[0] == 't'){
        strftime(timeBuff, 80, "%R", t_);
    } 
    else if(buff[0] == 'd'){
        strftime(timeBuff, 80, "%D", t_);
    }   

    sendto(sd, timeBuff, 80, 0, &cliente, clientelen);
}


int main(int argc, char* args[]){

    if(argc < 3){
        std::cout << "Usage: " << args[0] << " (cname/ip) port\n";
        return 0;
    }

    struct addrinfo hints;
    struct addrinfo *res;

    memset((void*)&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

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

    std::thread pool[MAX_THREAD];

    for(int i = 0; i < MAX_THREAD; ++i){
        pool[i] = std::thread([sd, i](){
            while(true){
                msg(sd, i);
            }
        });
    }

    for(int i = 0; i < MAX_THREAD; ++i){
        pool[i].join();
    }

    close(sd);

    return 0;
}