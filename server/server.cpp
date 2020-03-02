//#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;

int main(int argc,char** argv){
    if(argc!=2){
        cout<<"输入格式错误！"<<endl;
        return -1;
    }
    fstream f(argv[1],ios::in|ios::binary);
    if(!f){
        cout<<"文件不存在！"<<endl;
        return -1;
    }
    //创建并绑定套接字
    int serv_sock = socket(AF_INET,SOCK_STREAM,0);
    sockaddr_in serv_addr;
    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(1234);
    bind(serv_sock,(sockaddr*)&serv_addr,sizeof(serv_addr));

    listen(serv_sock,20);

    sockaddr clnt_addr;
    socklen_t clnt_addr_len = sizeof(clnt_addr);
    int clnt_sock = accept(serv_sock,&clnt_addr,&clnt_addr_len);

    //传输文件标题，以#结束
    string filename(argv[1]);
    filename.push_back('#');
    int pos = filename.find_last_of("/");
    if(pos!=string::npos){
        int n = filename.length() - pos;
        clog<<pos<<" "<<n<<endl;
        filename = filename.substr(pos,n);
    }
    write(clnt_sock,filename.c_str()+1,filename.length());

    //传输文件长度

    f.seekg(0, ios::end);                      // 设置指针到文件流尾部
    streampos ps = f.tellg();                  // 指针距离文件头部的距离，即为文件流大小
    f.seekg(0, ios::beg);                              // 指针重回文件流头部
    int len = ps;
    write(clnt_sock,(char*)&len,sizeof(int));

    //传输文件内容
    char *buf = new char[10];//一次10个字节
    cout<<"正在发送文件"<<endl;
    int left = len;
    while(left > 10){
        f.read(buf,sizeof(char)*10);
        write(clnt_sock,buf,sizeof(char)*10);
        left-=10;
    }
    while(left > 0){
        f.read(buf,sizeof(char));
        write(clnt_sock,buf,sizeof(char));
        left--;
    }
    delete buf;

    //如果客户端接收成功，客户端将发来"OK"
    buf = new char[2];
    read(clnt_sock,buf,sizeof(char)*2);
    if(!strcmp(buf,"OK")){
        cout<<"传输失败"<<endl;
        close(serv_sock);
        close(clnt_sock);
        return -1;
    }
    delete buf;
    cout<<"发送成功"<<endl;
    f.close();
    shutdown(serv_sock,SHUT_RDWR);
    shutdown(clnt_sock,SHUT_RDWR);

    return 0;
}