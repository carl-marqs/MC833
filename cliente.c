#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>
#include "utils.c"

void loop(int sockfd)
{
    char buffer[MAX_BUFFER_LENGTH];

    for (;;) // loop infinito
    {
        // Ler os comandos enviados pelo servidor
        bzero(buffer, sizeof(buffer));
        read(sockfd, buffer, sizeof(buffer));

        // Verificar se é o comando de saída
        if ((strncmp(buffer, "exit", 4)) == 0)
            break;

        // Executar o comando recebido
        FILE *stream = popen(buffer, "r");
        if (!stream)
        {
            perror("Error executing command");
            exit(1);
        }

        // Obter a saída do comando
        bzero(buffer, sizeof(buffer));
        while (fgets(buffer, sizeof(buffer), stream) != NULL) 
            ;
        pclose(stream);

        // Devolver o resultado
        write(sockfd, buffer, sizeof(buffer));
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

    // Exibir informações do servidor (argumentos) e seu socket local
    PrintSocketInfo(sockfd, servaddr);
    printf("[Peer Info] IP: %s | Port: %s\n", argv[1], argv[2]);
 
    // Conectar-se
    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0)
    {
        printf("Error connecting to the server\n");
        exit(1);
    }
 
    // Loop de mensagens
    loop(sockfd);
 
    // Fechar o socket
    close(sockfd);
}
