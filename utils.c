#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

const int MAX_BUFFER_LENGTH = 4095;

int Socket(int family, int type, int flags)
{
    int sockfd;
    
    if ((sockfd = socket(family, type, flags)) < 0)
    {
        perror("Error creating Socket");
        exit(1);
    }
    
    return sockfd;
}

struct sockaddr_in SocketAddress(int family, char const *address, int port)
{
    struct sockaddr_in sockaddr;

    bzero(&sockaddr, sizeof(sockaddr));

    sockaddr.sin_family = family;
    sockaddr.sin_addr.s_addr = inet_addr(address);
    sockaddr.sin_port = htons(port);

    return sockaddr;
}

void PrintSocketInfo(int sockfd, struct sockaddr_in addr)
{
    socklen_t len = sizeof(addr);

    if (getsockname(sockfd, (struct sockaddr *) &addr, &len) < 0)
    {
        perror("Error getting socket info");
        exit(1);
    }

    printf("[Socket Info] IP: %s | Port: %d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
}

void PrintPeerInfo(int connfd, struct sockaddr_in addr)
{
    socklen_t len = sizeof(addr);

    if (getpeername(connfd, (struct sockaddr *) &addr, &len) < 0)
    {
        perror("Error getting peer info");
        exit(1);
    }

    printf("[ Peer Info ] IP: %s | Port: %d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port)); 
}
