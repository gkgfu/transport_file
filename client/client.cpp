#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;
int main(){
    //创建套接字并绑定
    int sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    sockaddr_in addr;
    memset(&addr,0,sizeof(addr));
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1234);
    connect(sock,(sockaddr*)&addr,sizeof(addr));
    char c;
    string filename;
    
    //循环读入文件名称
    while(true){
        read(sock,&c,sizeof(char));
        if(c=='#')
            break;
        filename.push_back(c);
    }
    cout<<"文件名称:"<<filename<<endl;
    
    //读取文件长度
    int file_len;
    read(sock,&file_len,sizeof(int));
    cout<<"文件长度:"<<file_len<<endl;

    char *buf = new char[10];
    fstream f(filename,ios::out|ios::binary);
    int left = file_len;
    while(left > 10){
        read(sock,buf,sizeof(char)*10);
        f.write(buf,sizeof(char)*10);
        left-=10;
    }
    while(left > 0){
        read(sock,buf,sizeof(char));
        f.write(buf,sizeof(char));
        left--;
    }
    *buf = EOF;
    f.write(buf,sizeof(char));
    delete buf;
    buf = new char[2];
    strcpy(buf,"OK");
    write(sock,buf,sizeof(char)*2);
    cout<<"传输完毕"<<endl;
    close(sock);
    f.close();
    return 0;
}