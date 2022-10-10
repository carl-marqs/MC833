#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
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

char* CurrentTime()
{
    time_t now = time(NULL);
    char *time_str = ctime(&now);
    time_str[strlen(time_str)-1] = '\0';
    return time_str;
}

void SocketInfo(char *output, int sockfd, struct sockaddr_in addr)
{
    socklen_t len = sizeof(addr);

    if (getsockname(sockfd, (struct sockaddr *) &addr, &len) < 0)
    {
        perror("Error getting socket info");
        exit(1);
    }

    sprintf(output, "[Socket Info] Time: %s | IP: %s | Port: %d", CurrentTime(), inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
}

void PeerInfo(char *output, int connfd, struct sockaddr_in addr)
{
    socklen_t len = sizeof(addr);

    if (getpeername(connfd, (struct sockaddr *) &addr, &len) < 0)
    {
        perror("Error getting peer info");
        exit(1);
    }

    sprintf(output, "[ Peer Info ] Time: %s | IP: %s | Port: %d", CurrentTime(), inet_ntoa(addr.sin_addr), ntohs(addr.sin_port)); 
}
