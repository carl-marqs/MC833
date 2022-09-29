#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "utils.c"

// Function designed for chat between client and server.
void loop(int connfd)
{
    char buffer[MAX_BUFFER_LENGTH];

    for (;;) // loop infinito
    {
        bzero(buffer, MAX_BUFFER_LENGTH);
   
        // read the message from client and copy it in buffer
        read(connfd, buffer, sizeof(buffer));

        // print buffer which contains the client contents
        printf("From client: %s\t To client : ", buffer);
        bzero(buffer, MAX_BUFFER_LENGTH);
        int n = 0;
        
        // copy server message in the buffer
        while ((buffer[n++] = getchar()) != '\n')
            ;
   
        // and send that buffer to client
        write(connfd, buffer, sizeof(buffer));
    }
}
   
// Driver function
int main(int argc, char **argv)
{
    // Verificar argumentos
    if (argc < 2)
    {
        printf("Usage: servidor <Port>\n");
        exit(1);
    }

    // Inicializar variáveis de socket e endereço
    int sockfd = Socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in servaddr = SocketAddress(AF_INET, "127.0.0.1", atoi(argv[1]));
    struct sockaddr_in connaddr;
   
    // 
    if ((bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) != 0)
    {
        printf("socket bind failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully binded..\n");
   
    // 
    if ((listen(sockfd, 10)) != 0)
    {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");
    int len = sizeof(connaddr);
   
    // 
    int connfd = accept(sockfd, (struct sockaddr*)&connaddr, &len);
    if (connfd < 0) {
        printf("server accept failed...\n");
        exit(0);
    }
    else
        printf("server accept the client...\n");
   
    // Loop de mensagens
    loop(connfd);
   
    // Fechar o socket
    close(sockfd);
}
