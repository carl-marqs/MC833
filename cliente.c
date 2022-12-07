#include <arpa/inet.h>
#include <ctype.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "utils.c"

void loop(int sockfd)
{
    char buffer[MAX_BUFFER_LENGTH];

    for (;;) // loop infinito
    {
        // Ler mensagem enviada pelo servidor
        bzero(buffer, sizeof(buffer));
        read(sockfd, buffer, sizeof(buffer));

        // Exibir mensagem do servidor
        printf("S: %s", buffer);

        // Critério de saída
        if (strcmp(buffer, "finalizar_chat\n") == 0)
            break;

        // Solicitar mensagem
        printf("C: ");
        bzero(buffer, sizeof(buffer));
        //scanf("%4094[^\n]", buffer);
        fgets(buffer, MAX_BUFFER_LENGTH, stdin);

        // Enviar mensagem
        write(sockfd, buffer, sizeof(buffer));

        // Critério de saída
        if (strcmp(buffer, "finalizar_chat\n") == 0)
            break;
    }
}
 
int main(int argc, char **argv)
{
    // Verificar argumentos
    if (argc < 3)
    {
        printf("Usage: cliente <IP> <Port>\n");
        exit(1);
    }

    // Inicializar variáveis de socket e endereço
    int sockfd = Socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in servaddr = SocketAddress(AF_INET, argv[1], atoi(argv[2]));

    // Exibir informações do servidor (argumentos)
    printf("[ Peer Info ] Time: %s | IP: %s | Port: %s\n", CurrentTime(), argv[1], argv[2]);
 
    // Conectar-se
    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0)
    {
        printf("Error connecting to the server\n");
        exit(2);
    }

    // Exibir informações do socket local
    char info[512];
    bzero(info, sizeof(info));
    SocketInfo(info, sockfd, servaddr);
    printf("%s\n", info);
 
    // Loop de mensagens
    loop(sockfd);
 
    // Fechar o socket
    close(sockfd);
}
