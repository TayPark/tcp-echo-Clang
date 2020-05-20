/* 
  Client: socket() -> connect(), connection request 
  -> send() or recv() -> (if it needs) close()
*/
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define BUFSIZE 10
#define SERVER_IP "127.0.0.1"
#define ECHOPORT 9999

void error_handling(char *message);

int main(int argc, char **argv) {
        int sock;
        struct sockaddr_in serv_addr;
        char message[BUFSIZE];
        int str_len;

        // 클라이언트 소켓 생성부.IPv4, Stream 방식 정의
        sock = socket(PF_INET, SOCK_STREAM, 0);
        printf("Creating socket...\n");
        if(sock == -1) {
                error_handling("Socket creation error\n");
                close(sock);
        }
        
        memset(&serv_addr, 0, sizeof(serv_addr));

        serv_addr.sin_family = AF_INET;
        // define에 지정된 IP, Port 사용
        serv_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
        serv_addr.sin_port = htons(ECHOPORT);

        printf("Connecting to Host: %s:%d...\n", SERVER_IP, ECHOPORT);
        if( connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
                close(sock);
                error_handling("Connection setup error: connect() error\n");
        } else {
                printf("Connection setup completed\n");
        }

        while(1) {
                fputs("Enter message return (Type 'exit' to quit) : ", stdout);
                fgets(message, BUFSIZE, stdin);

                if (strlen(message) >= BUFSIZE) {
                        printf("Overflow occured\n");
                }
                
                // 종료 문구 기입시 종료
                if(!strcmp(message, "exit\n")) {
                        break;
                }

                write(sock, message, strlen(message));

                str_len = read(sock, message, BUFSIZE);
                // message[str_len] = 0;
                printf("Echo from server : %s \n", message);
        }          
        close(sock);
        return 0;
}

void error_handling(char *message) {
        fputs(message, stderr);
        fputc('\n', stderr);
        exit(1);
}