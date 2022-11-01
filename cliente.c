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
        // Ler os comandos enviados pelo servidor
        bzero(buffer, sizeof(buffer));
        read(sockfd, buffer, sizeof(buffer));

        // Verificar se é o comando de saída
        if ((strncmp(buffer, "EXIT", 4)) == 0)
            break;
        
        // Dormir 5 segundos para facilitar nos testes
        //sleep(5);

        // Exibir comando ao contrário
        //char rev[32];
        //bzero(rev, sizeof(rev));
        //for (size_t i = 0; i < strlen(buffer); i++)
        //    rev[i] = toupper(buffer[strlen(buffer) - i - 1]);
        //printf("Command: %s\n", rev); 

        // Executar o comando recebido
        //FILE *stream = popen(buffer, "r");
        //if (!stream)
        //{
        //    perror("Error executing command");
        //    exit(1);
        //}

        // Obter a saída do comando
        //size_t n = 0;
        //bzero(buffer, sizeof(buffer));
        //while (fgets(buffer + n, sizeof(buffer), stream)) 
        //    n += strlen(buffer + n);
        //pclose(stream);
        //printf("%s\n", buffer); // debug: exibir no terminal o resultado do comando

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
