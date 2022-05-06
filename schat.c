#include <stdio.h>  
#include <sys/socket.h> 
#include <arpa/inet.h>  
#include <unistd.h>
#include <stdlib.h> //atoi 
#include <string.h>
#include <poll.h>
#include <netinet/in.h>


int main(int argc , char *argv[]){
    int port;
    int connection;
    char *buffer = (char *)malloc(sizeof(char)*4096);
    char *buffer1 = (char *)malloc(sizeof(char)*4096);
    int listener;
    int s;
    ssize_t message;
    int c;

    struct pollfd *fds;
    fds = calloc(2, sizeof(struct pollfd));
    int timeout_msecs = 60000;

    if(argc == 1 ){ //server mode
        port = 0xc000 | (random()&0x3fff);
        printf("The server is now listening on port %d\n", port);

        struct sockaddr_in ipOfServer;
        memset(&ipOfServer, 0, sizeof(struct sockaddr_in));
        ipOfServer.sin_family = AF_INET;
        ipOfServer.sin_addr.s_addr = htonl(INADDR_ANY);
        ipOfServer.sin_port = htons(port);

        listener = socket(AF_INET, SOCK_STREAM, 0);

        // and we need to tell the OS that this socket will use the address created for it
        bind(listener, (struct sockaddr*)&ipOfServer , sizeof(ipOfServer));

        // wait for connections; if too many at once, suggest the OS queue up 20
        listen(listener , 20);
        system("host $HOSTNAME"); // display all this computer's IP addresses
        printf("The server is now listening on port %d\n", port); // and listening port

        connection = accept(listener, (struct sockaddr*)NULL, NULL);
        close(listener);
        while(1){

            fds[0].fd = 0;
            fds[0].events = POLLIN;
            fds[1].fd = connection;
            fds[1].events = POLLIN;

            int p = poll(fds, 2, timeout_msecs);
            if(p){ //succeeded
        
                if(fds[0].revents & POLLIN){
                    message = read(0, buffer1, 4096);
                    write(connection, buffer1, message);
                }
                if(fds[1].revents & POLLIN){
                    message = read(connection, buffer1, 4096);
                    write(1, buffer1, message);
                }
            }
        }
        free(fds);
        free(buffer1);
        // close(connection);
    }

    if(argc > 1){ //client mode
        const char *ip_address_of_server;

        ip_address_of_server = argv[1];
        port = atoi(argv[2]);
        printf("ip address: %s\n", ip_address_of_server);

        struct sockaddr_in ipOfServer;
        memset(&ipOfServer, 0, sizeof(struct sockaddr_in));
        ipOfServer.sin_family = AF_INET;
        ipOfServer.sin_port = htons(port);
        ipOfServer.sin_addr.s_addr = inet_addr(ip_address_of_server);

        s = socket(AF_INET, SOCK_STREAM, 0);
        if(s == -1){
            puts("error creating an unbound socket");
            return 0;
        }
        //Connect to Remote server 
        c = connect(s, (struct sockaddr*)&ipOfServer, sizeof(ipOfServer));
        if(c == -1){
            puts("connection to socket error");
            return 0;
        }

        while(1){

            fds[0].fd = 0;
            fds[0].events = POLLIN;
            fds[1].fd = s;
            fds[1].events = POLLIN;
        
            int p = poll(fds, 2, timeout_msecs);

            if(p){ //succeeded
        
                if(fds[0].revents & POLLIN){
                    message = read(0, buffer, 4096);
                    write(s, buffer, message);
                }
                if(fds[1].revents & POLLIN){
                    message = read(s, buffer, 4096);
                    write(1, buffer, message);
                }            
            }
        }
        free(fds);
        free(buffer);
        close(s);
        //close(c);
    
    }

    return 0;
}