#include <cstring>
#include <cstring>
#include <netdb.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <thread>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <vector>
#include <math.h>
#include <unistd.h>
using namespace std;

class TCPRequestChannel{
private:
    int sockfd;

    int server (string port){

        struct addrinfo hints, *serv;
        struct sockaddr_storage their_addr;
        socklen_t sin_size;
        char s [INET6_ADDRSTRLEN];
        int rv;

        memset(&hints, 0 ,sizeof(hints));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;

        if((rv = getaddrinfo(NULL, port.c_str(), &hints, &serv)) != 0){
            cerr << "getaddrinfo: " << gai_strerror(rv) << endl;
            return -1;
        }

        if((sockfd = socket(serv->ai_family, serv->ai_socktype, serv->ai_protocol)) == -1){
            perror("server: socket");
            return -1;
        }

        if(bind(sockfd, serv->ai_addr,serv->ai_addrlen) == 1){
            close(sockfd);
            perror("server: bind");
            return -1;
        }

        freeaddrinfo(serv);

        if(listen(sockfd,20) == -1){
            perror("listen");
            exit(1);
        }
    }

    int client (string host, string port){
        struct addrinfo hints, *res;

        memset(&hints,0, sizeof(hints));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        int status;

        if((status = getaddrinfo(host.c_str(), port.c_str(), &hints, &res)) != 0){
            cerr << "getaddrinfo: " << gai_strerror(status) << endl;
            return -1;
        }

        sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if(sockfd < 0){
            perror("Cannot create socket");
            return -1;
        }

        if(connect(sockfd, res->ai_addr, res->ai_addrlen) < 0){
            perror("Cannot Connect");
            return -1;
        }
    }

public:
    TCPRequestChannel (const string _host, const string _port, int _side){
        if(_side == 0){
            server(_port);
        } else {
            client(_host, _port);
        }
    }

    TCPRequestChannel(int _s){
        sockfd = _s;
    }

    ~TCPRequestChannel(){
        close(sockfd);
    }

    int cread(void* buf, int len){
        return recv(sockfd, buf, len, 0);
    }

    int cwrite(void* buf, int len){
        return send(sockfd, buf, len, 0);
    }

    int getsocket(){
        return sockfd;
    }

};