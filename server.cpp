/*
** server.c -- a stream socket server demo
*/
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <string>
#include <iostream>
#include <sys/mman.h> 
#include "stack.hpp"

#define PORT "3490"  // the port users will be connecting to

#define BACKLOG 10	 // how many pending connections queue will hold
stack *global_stack;

/**
 * @brief A function to check if param a start with param b 
 * 
 * @return 1 if param a start with param b 
 */
int StartsWith(const char *a, const char *b)
{
   if(strncmp(a, b, strlen(b)) == 0) return 1;
   return 0;
}

void* thread_handler(void *new_fd){
    int socket=*(int*) new_fd;
    char recv_data[1024];
    while (1)
    {
        memset(recv_data, 0, 1024);
        if(recv(socket,recv_data,1024,0)==-1){
            perror("ERR:recv error");
        }
        /*PUSH*/
        if (StartsWith(recv_data,"PUSH"))
        {
            strncpy(recv_data,recv_data+5,sizeof(recv_data) - 5);
            string str(recv_data);
            PUSH(recv_data,global_stack);
            cout<<"pushed string:"<<str<<endl;
        }
        /*POP*/
        else if(StartsWith(recv_data,"POP")){
            string ans= POP(global_stack)+'\0';;
            if (send(socket,ans.data(),ans.size(),0)==-1)
            {
                perror("send");
            }
            cout<<"sent "<<ans.data()<<endl;
        }
        else if(StartsWith(recv_data,"TOP")){
            printf("TOP AT: %p\n", global_stack->addres_);
            string ans= TOP(*global_stack)+'\0';
            if (send(socket,ans.data(),ans.size(),0)==-1)
            {
                perror("send");
            }
            cout<<"sent "<<ans.data()<<endl;
        }
    }
    close(socket);
} 


void sigchld_handler(int s)
{
    (void)s; // quiet unused variable warning

    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void)
{
    file_de();
    global_stack = (struct stack*)mmap(NULL,sizeof(struct stack), PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED | MAP_ANON, -1, 0);
    memory_to_shared(global_stack);
    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;
    //memory_init(global_stack);
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                       sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    printf("server: waiting for connections...\n");

    while(1) {  // main accept() loop
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family,get_in_addr((struct sockaddr *)&their_addr),s, sizeof s);
        printf("server: got connection from %s\n", s);
        if (!(fork()))
        {
        thread_handler(&new_fd);
        close(new_fd);
        exit(0);
        }
    }

    return 0;
}