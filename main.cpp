#include <iostream>
#include<thread>
#include<netinet/in.h>
#include<sys/wait.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<unistd.h>
#include"ProxySocket.hpp"
#include"HttpHeader.hpp"
using namespace std;
typedef  int ThreadHandle;
const int PROXYPORT=16924;
static int BACKLOG=4;
int proxySockfd;
struct sockaddr_in proxyAddr;

//初始化，为代理创建一个套接字
bool Init()
{
    if((proxySockfd=socket(AF_INET,SOCK_STREAM,0))==ERR)
    {
        cerr<<"无法为代理创建套接字"<<endl;
        return false;
    }
    proxyAddr.sin_family=AF_INET;
    proxyAddr.sin_port=htons(PROXYPORT);
    proxyAddr.sin_addr.s_addr=INADDR_ANY;
    if(bind(proxySockfd,(struct sockaddr *)&proxyAddr,sizeof(proxyAddr))==ERR)
    {
        cerr<<"无法为代理绑定套接字"<<endl;
        return false;
    }
        if(listen(proxySockfd,BACKLOG)==ERR)
    {
        cerr<<"无法正常监听端口"<<PROXYPORT<<endl;
        return false;
    }
    cout<<"代理初始化成功"<<endl;
    return true;
}

//代理服务 线程的run函数
void * ProxyServer(void * curRequest)
{
        cout<<"正在为fd："<<((ProxySocket *)curRequest)->client<<"服务"<<endl;
        char *Buffer=new char[1<<20];
        int recvStatu=recv(((ProxySocket*)curRequest)->client,Buffer,65507,0);
        cout<<recvStatu<<endl;

        char *cache=new char[strlen(Buffer)+1];
        memcpy(cache,Buffer,strlen(Buffer)+1);
        HttpHeader *header=new HttpHeader(Buffer);
        if(header!=NULL)
        {
            cout<<"请求方法:"<<header->method<<endl;
            cout<<"url:"<<header->url<<endl;
            cout<<"Host:"<<header->host<<endl;
            cout<<"Cookie:"<<header->cookie<<endl;
        }
        if(((ProxySocket*)curRequest)->TryConnect2Server(header->host))
        {
            cout<<"与"<<header->host<<"连接成功,开始通信"<<endl;
           // char *messagess="GET http://utsc.guet.edu.cn/ HTTP/1.1\r\nHost: utsc.guet.edu.cn\r\n\r\n";
          //  char * messagess="GET http://utsc.guet.edu.cn/ HTTP/1.1\r\nHost: utsc.guet.edu.cn\r\nProxy-Connection: keep-alive\r\nCache-Control: max-age=0\r\nUpgrade-Insecure-Requests: 1\r\nUser-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/72.0.3626.96 Safari/537.36\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8\r\nAccept-Language: zh-CN,zh;q=0.9\r\nCookie: ASP.NET_SessionId=kz5ebu45dtxt2pjk24z5nr45\r\n\r\n";
           //\r\nUser-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/72.0.3626.96 Safari/537.36\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8\r\nAccept-Encoding: gzip, deflate\r\nAccept-Language: zh-CN,zh;q=0.9\r\nCookie: ASP.NET_SessionId=kz5ebu45dtxt2pjk24z5nr45
            cout<<Buffer<<endl;
            send(((ProxySocket*)curRequest)->server,Buffer,strlen(Buffer),0);
            //int Left=1<<20;
            //int nCount=0;
            //recvStatu=1;
            //int i=1;
            //while(1)
            //{
           // cout<<i++<<endl;
            //recvStatu=recv(((ProxySocket*)curRequest)->server,Buffer+nCount,Left,0);
            //if(recvStatu<0)recvStatu=recv(((ProxySocket*)curRequest)->server,Buffer,1<<20,0);
            //
            if(recvStatu=recv(((ProxySocket*)curRequest)->server,Buffer,1<<20,0)<0)
            {
                cerr<<"无法接受服务器发来的http报文"<<errno<<endl;
                sleep(0.2);
                close(((ProxySocket*)curRequest)->server);
            //    break;
            //}
            //if(recvStatu==0)
            //{
              //  cout<<"接收完毕"<<endl;
              //  Buffer[nCount+1]=0;
              //  break;
            //}
            //Left-=recvStatu;
            //nCount+=recvStBufferatu;
            }
            //recvStatu=recv(((ProxySocket*)curRequest)->server,Buffer,1<<20,0);

            cout<<Buffer<<endl;
            struct sockaddr_in clientAddr;
                socklen_t len =sizeof(clientAddr);
            if((getsockname(((ProxySocket*)curRequest)->client,(struct sockaddr*)&clientAddr,&len))==0)
            {
                cout<<"客户端端口号"<<ntohs(clientAddr.sin_port)<<endl;
            }
            int _size=send(((ProxySocket*)curRequest)->client,Buffer,strlen(Buffer),0);
            cout<<"send返回数据大小"<<_size<<endl;
            cout<<"缓存区大小"<<strlen(Buffer)<<endl;
        }
        cout<<"over"<<endl;
        close(((ProxySocket*)curRequest)->client);
        return NULL;
}

int main()
{
        if(!Init())
        return -1;
    int clientSockfd;

    while(1)
    {
        cout<<"等待新用户连接"<<endl;
        clientSockfd=accept(proxySockfd,NULL,NULL);
        if(clientSockfd!=0)
        {
        ProxySocket *curRequest=new ProxySocket();
        curRequest->client=clientSockfd;
        cout<<"新连接客户端 fd："<<clientSockfd<<endl;
        pthread_t newId;
        ThreadHandle handle=pthread_create(&newId,NULL,ProxyServer,curRequest);
        close(handle);
        }
        sleep(0.2);
    }
    close(proxySockfd);
    return 0;
}
