#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include "utils.c"


typedef void Sigfunc(int);

void sig_chld(int);
Sigfunc* Signal(int signo, Sigfunc *func);


void loop(int connfd, struct sockaddr_in connaddr)
{
    char buffer[MAX_BUFFER_LENGTH];
       
    // Obter informações do cliente
    char info[512];
    bzero(info, sizeof(info));
    PeerInfo(info, connfd, connaddr);

    // Obter nome do arquivo
    char filename[32];
    sprintf(filename, "%d.txt", ntohs(connaddr.sin_port));

    // Abrir o arquivo
    FILE *stream = fopen(filename, "w");
    if (!stream)
    {
        perror("Error creating file");
        exit(7);
    }

    // Escrever informações no arquivo
    bzero(info, sizeof(info));
    PeerInfo(info, connfd, connaddr);
    fprintf(stream, "%s\n\n", info);

    // Fechar o arquivo
    fclose(stream);

    for (;;) // loop infinito
    {
        // Solicitar mensagem
        printf("S: ");
        bzero(buffer, sizeof(buffer));
        //scanf("%4094[^\n]", buffer);
        fgets(buffer, MAX_BUFFER_LENGTH, stdin);

        // Enviar mensagem
        write(connfd, buffer, sizeof(buffer));
        
        // Abrir o arquivo
        stream = fopen(filename, "a");
        if (!stream)
        {
            perror("Error appending file");
            exit(8);
        }

        // Escrever no arquivo
        fprintf(stream, "S: %s", buffer);
  
        // Fechar o arquivo
        fclose(stream);

        // Critério de saída
        if (strcmp(buffer, "finalizar_chat\n") == 0)
            break;

        // Ler mensagem recebida
        bzero(buffer, sizeof(buffer));
        read(connfd, buffer, sizeof(buffer));

        // Exibir mensagem do cliente
        printf("C: %s", buffer);

        // Abrir o arquivo
        stream = fopen(filename, "a");
        if (!stream)
        {
            perror("Error appending file");
            exit(8);
        }

        // Escrever no arquivo
        fprintf(stream, "C: %s", buffer);
  
        // Fechar o arquivo
        fclose(stream);

        // Critério de saída
        if (strcmp(buffer, "finalizar_chat\n") == 0)
            break;
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

    // Chamar waitpid()
    Signal(SIGCHLD, sig_chld);
   
    // Fluxo concorrente
    socklen_t len = sizeof(connaddr); 
    for (;;)
    {
        // Aceita a conexão de um cliente
        int connfd;
        if ( (connfd = accept(sockfd, (struct sockaddr*)&connaddr, &len)) < 0) {
            if (errno == EINTR)
                continue;
            else
            {
                //perror("accept error");
                exit(4);
            }
        }

        // Exibe informações de conexão
        char info[512];
        bzero(info, sizeof(info));
        PeerInfo(info, connfd, connaddr);
        printf("%s\n", info); // Exibir na saída padrão
        FILE *stream = fopen("Connections.txt", "a");
        if (!stream)
        {
            perror("Error opening file");
            exit(5);
        }
        fprintf(stream, "Connected    - %s\n", info);
        fclose(stream);

        // Cria um processo filho
        int childpid = fork();
        if (childpid < 0)
        {
            printf("Error creating child process\n");
            exit(6);
        }
        else if (childpid == 0)
        {
            // Fecha a conexão de escuta
            close(sockfd);
        
            // Loop de mensagens
            loop(connfd, connaddr);

            // Exibe informações de desconexão
            bzero(info, sizeof(info));
            PeerInfo(info, connfd, connaddr);
            //printf("Disconnected - %s\n", info); // Exibir na saída padrão
            stream = fopen("Connections.txt", "a");
            if (!stream)
            {
                perror("Error opening file");
                exit(5);
            }
            fprintf(stream, "Disconnected - %s\n", info);
            fclose(stream);

            // Fechar a conexão com o cliente
            close(connfd);
        }
        
        // Fecha a conexão
        close(connfd);  
    }
}

Sigfunc* Signal(int signo, Sigfunc *func)
{
    struct sigaction act, oact;
    act.sa_handler = func;
    sigemptyset (&act.sa_mask); /* Outros sinais não são bloqueados*/
    act.sa_flags = 0;
    if (signo == SIGALRM) { /* Para reiniciar chamadas interrompidas */
        #ifdef SA_INTERRUPT
            act.sa_flags |= SA_INTERRUPT; /* SunOS 4.x */
        #endif
    } else {
        #ifdef SA_RESTART
            act.sa_flags |= SA_RESTART; /* SVR4, 4.4BSD */
        #endif
    }
    if (sigaction (signo, &act, &oact) < 0)
        return (SIG_ERR);
    return (oact.sa_handler);
 }

void sig_chld(int signo) {
    pid_t pid;
    int stat;
    while ( (pid = waitpid(-1, &stat, WNOHANG)) > 0)
        printf("child %d terminated\n", pid);
    return;
}