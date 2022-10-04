#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "utils.c"

void loop(int connfd, struct sockaddr_in connaddr)
{
    char buffer[MAX_BUFFER_LENGTH];
    const char *commands[4] = { "ifconfig", "pwd", "ls -l", "EXIT" };

    for (int i = 0; i < 4; i++)
    {
        // Enviar comando
        write(connfd, commands[i], sizeof(commands[i]));

        // Critério de saída
        if (i >= 3)
            break;

        // Ler o resultado dos comandos
        bzero(buffer, sizeof(buffer));
        read(connfd, buffer, sizeof(buffer));

        // Obter nome do arquivo
        char filename[32];
        sprintf(filename, "%d", ntohs(connaddr.sin_port));
        strcat(filename, " - ");
        strcat(filename, commands[i]);
        strcat(filename, ".txt");

        // Abrir o arquivo
        FILE *stream = fopen(filename, "w");
        if (!stream)
        {
            perror("Error opening file");
            exit(6);
        }

        // Escrever no arquivo
        fprintf(stream, "%s", buffer);
  
        // Fechar o arquivo
        fclose(stream);
    }
}

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
   
    // Associar conexões
    if ((bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) != 0)
    {
        printf("Binding error\n");
        exit(2);
    }
   
    // Entrar em modo de escuta
    if ((listen(sockfd, 10)) != 0)
    {
        printf("Error listening to clients\n");
        exit(3);
    }
   
    // Fluxo concorrente
    socklen_t len = sizeof(connaddr); 
    for (;;)
    {
        // Aceita a conexão de um cliente
        int connfd = accept(sockfd, (struct sockaddr*)&connaddr, &len);
        if (connfd < 0)
        {
            //printf("Error accepting client\n");
            exit(4);
        }

        // Exibe informações do cliente
        PrintPeerInfo(connfd, connaddr);

        // Cria um processo filho
        int childpid = fork();
        if (childpid < 0)
        {
            printf("Error creating child process\n");
            exit(5);
        }
        else if (childpid == 0)
        {
            // Fecha a conexão de escuta
            close(sockfd);
        
            // Loop de mensagens
            loop(connfd, connaddr);

            // Fechar a conexão com o cliente
            close(connfd);
        }
        
        // Fecha a conexão
        close(connfd);  
    }
}
