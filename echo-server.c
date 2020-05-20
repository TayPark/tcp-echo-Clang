/*
  Server: socket() -> bind() -> listen() -> (Connection requested) accept() 
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
#define ECHOPORT 9999

void error_handling(char *message);

int main(int argc, char **argv) {
        // 서버는 서버-클라이언트 소켓을 모두 가져야한다.
        int serv_sock, clnt_sock;
        char message[BUFSIZE];
        int str_len;
        struct sockaddr_in serv_addr, clnt_addr;
        int clnt_addr_size;


        // 서버 소켓 생성부. IPv4, Stream방식을 사용함을 정의. 
        serv_sock = socket(PF_INET, SOCK_STREAM, 0);  
        printf("Creating socket...\n");  
        if(serv_sock == -1) {
                error_handling("Socket creation error\n");
                close(serv_sock);
        }

        // sockaddr_in 구조체 사용 전 초기화
        memset(&serv_addr, 0, sizeof(serv_addr));

        serv_addr.sin_family = AF_INET;
        // 주소 자동 지정
        serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        serv_addr.sin_port = htons(ECHOPORT);
        printf("Binding...\n");
        // 소켓에 주소 할당 및 에러 핸들링. 실패시 close
        if( bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1 ) {
                close(serv_sock);
                error_handling("Binding error\n");
        }
        // 바인딩 성공시 리스닝 상태로 진입. backlog가 0이면 default 값이나 로컬 에코 서버이므로 무의미.
        if( listen(serv_sock, 0) == -1 )  {
                close(serv_sock);
                error_handling("Listening error\n");
        }
        else {
                printf("Server is listening on port %d...\n", ECHOPORT);
        }

        clnt_addr_size = sizeof(clnt_addr);
        
        // 커넥션 셋업 요청 수락 및 에러핸들링
        printf("Connection requested: %d", clnt_sock);
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
        if(clnt_sock == -1) {
                close(clnt_sock);
                error_handling("Connection setup error: accpet()\n");
        }

        // 클라이언트로부터 메시지 읽은 값 처리
        while((str_len = read(clnt_sock, message, BUFSIZE)) != 0 ) {
                if (strlen(message) > BUFSIZE) { // 지정된 윈도우 사이즈보다 클 경우 잘라서 보낸다.
                        
                } else {
                        write(clnt_sock, message, str_len);
                        write(1, message, str_len);
                }
                
        }
        /* 
          클라이언트가 종료요청을 했을 때 클라이언트 소켓 close. 
          서버는 다른 연결을 위해 프로그램이 끝날 때 까지 리스닝상태.
        */
        close(clnt_sock);      
        return 0;
}

void error_handling(char *message) {
        fputs(message, stderr);
        fputc('\n', stderr);
        exit(1);
}

