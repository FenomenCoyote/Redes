#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <time.h>
#include <thread>
#include <mutex>

#define MAX_THREAD 5

class ServerEmployee {
public:
    ServerEmployee(int sd) : sd(sd){ }

    void work() {
        char buff[80];
        while(true) {
            struct sockaddr cliente;
            socklen_t       clientelen = sizeof(struct sockaddr);

            int bytes = recvfrom(sd, (void*)buff, 79, 0, &cliente, &clientelen);

            if( bytes == -1) {
                std::cerr << "Error: recibiendo socket\n";
                return;
            }

            buff[bytes] = '\0';

            for(int i = 4; i > 0; --i){
                std::cout << "thread [" << std::this_thread::get_id() << "] ready in " << i << " seconds . . ." << std::endl;
                sleep(1);
            }

            char host[NI_MAXHOST];
            char serv[NI_MAXSERV];
            getnameinfo(&cliente, clientelen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

            std::cout << bytes << " bytes from " << host << ":" << serv << " in thread with id: " << std::this_thread::get_id() << std::endl;

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
    }

private:
    int sd;
};

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

    ServerEmployee* employees[MAX_THREAD];
    for(int i = 0; i < MAX_THREAD; ++i){
        employees[i] = new ServerEmployee(sd);
        ServerEmployee* se = employees[i];
        std::thread([&se](){
            se->work();
        }).detach();
    }

    char quit = 'n';
    while(quit != 'q') {
        sleep(1);
        std::cin.get(quit);
    }

    for(int i = 0; i < MAX_THREAD; ++i){
        delete employees[i];
    }

    close(sd);

    return 0;
}