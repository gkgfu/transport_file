#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(){
    int serv_sock = socket(AF_INET,SOCK_STREAM,0);

    sockaddr_in serv_addr;
    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(1234);
    bind(serv_sock,(sockaddr*)&serv_addr,sizeof(serv_addr));
    
    listen(serv_sock,20);
    sockaddr_in clnt_addr;
    socklen_t clnt_addr_len;
    int clnt_sock = accept(serv_sock,(sockaddr*)&clnt_addr,&clnt_addr_len);

    char buf[]="hello world\n";
    write(clnt_sock,buf,sizeof(buf));

    shutdown(clnt_sock,SHUT_RDWR);
    close(serv_sock);

    return 0;

}