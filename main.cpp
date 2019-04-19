#include<iostream>
#include<signal.h>
#include<thread>
#include<netinet/in.h>
#include<sys/wait.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<unistd.h>
#include"ProxySocket.hpp"
#include"HttpHeader.hpp"
#include"ThreadPool.hpp"
#include"Filter.hpp"
#include"CacheManager.hpp"
#include<time.h>
using namespace std;
typedef  int ThreadHandle;
const int PROXYPORT=16924;
static int BACKLOG=4;
int proxySockfd;
struct sockaddr_in proxyAddr;
Filter *filter;
const char * staticHtml="GET /strive.html HTTP/1.1\r\nHost: 120.77.249.7\r\n\r\n";
ThreadPool *threadPool;
//初始化，为代理创建一个监听套接字
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
    int sock_reuse=1;
    setsockopt(proxySockfd,SOL_SOCKET,SO_REUSEADDR,(char *)&sock_reuse,sizeof(sock_reuse)); 
    if(bind(proxySockfd,(struct sockaddr *)&proxyAddr,sizeof(proxyAddr))==ERR)
    {
        cerr<<"无法为代理绑定套接字"<<errno<<endl;
        return false;
    }
        if(listen(proxySockfd,BACKLOG)==ERR)
    {
        cerr<<"无法正常监听端口"<<PROXYPORT<<endl;
        return false;
    }
    cout<<"代理初始化成功"<<endl;
    //初始化过滤对象
    filter=new Filter();
    return true;
}

void FreePort(int ignore)
{
    delete threadPool;
    close(proxySockfd);
    cout<<"\n简易Http代理关闭端口 正常退出"<<endl;
    exit(0);
}

//代理服务 线程的任务函数
void * ProxyServer(void * curRequest)
{
        bool longConnection=false;
        bool first=true;
        bool skip=false;
        bool cache=false;
	    time_t timep;
        char *Buffer=new char[1<<20];
        char *cacheFile=new char[1<<20];
	   int recvStatu;
       int _size;
        struct timeval timeout = {5, 0};
        setsockopt(((ProxySocket*)curRequest)->client, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval));
	recvStatu=recv(((ProxySocket*)curRequest)->client,Buffer,1<<20,0);
        if(recvStatu==-1)
        {
            //cerr<<"无法接受客户端消息,错误码："<<errno<<endl;
          // time (&timep);
        //cout<<ctime(&timep)<<((ProxySocket*)curRequest)->GetClientIp()<<" fd: "<<((ProxySocket*)curRequest)->client<<"结束通信"<<endl;

        ((ProxySocket*)curRequest)->OverConnection();

	    return NULL;
        }
        HttpHeader *header=new HttpHeader(Buffer);
        if(header!=NULL)
        {
            cout<<"请求方法:"<<header->method<<endl;
            cout<<"url:"<<header->url<<endl;
            cout<<"Host:"<<header->host<<endl;
            //cout<<"Cookie:"<<header->cookie<<endl;
            //cout<<"是否长连接:"<<header->isKeepAlive<<endl;
            longConnection=header->isKeepAlive;
            cache=CacheManager::GetIntance()->TryGetCache(header->url,Buffer,cacheFile);
        }
	else
	{
	time (&timep);
	cout<<ctime(&timep)<<((ProxySocket*)curRequest)->GetClientIp()<<"与"<<header->host<<" fd: "<<((ProxySocket*)curRequest)->client<<"结束通信"<<endl;

        ((ProxySocket*)curRequest)->OverConnection();
        return NULL;
	}
        if(filter->JudgeHost(header->host))
        {
            cout<<"检测到过滤目标，正在转移网址"<<endl;
            memcpy( header->host,"120.77.249.7",strlen("120.77.249.7"));
            skip=true;
        }
        if(((ProxySocket*)curRequest)->TryConnect2Server(header->host))
        {
            cout<<"与"<<header->host<<"连接成功,开始通信"<<endl;
        do
        {
            if(!first)
            {
            char *Buffer=new char[1<<20];
            recvStatu=recv(((ProxySocket*)curRequest)->client,Buffer,1<<20,0);
            }
            if(recvStatu==-1)
            break;
            if(first)
            {
                first=false;
                if(skip)
                    send(((ProxySocket*)curRequest)->server,staticHtml,strlen(staticHtml),0);
                else
                    send(((ProxySocket*)curRequest)->server,Buffer,strlen(Buffer),0);
            }
            else
                send(((ProxySocket*)curRequest)->server,Buffer,strlen(Buffer),0);
            int Left=1<<20;
            int nCount=0;
            recvStatu=1;
            struct timeval timeout = {2, 0};
            setsockopt(((ProxySocket*)curRequest)->server, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval));
            memset(Buffer, 0x00, sizeof (char) * (1<<20));
            while(1)
            {
            recvStatu=recv(((ProxySocket*)curRequest)->server,Buffer+nCount,Left,0);
            if(recvStatu<0)
            {
            if(errno==EINTR)
            {
               // cout<<"尝试跳过"<<endl;
                continue;
            }
            else if(errno==EWOULDBLOCK||errno==EAGAIN)
            {
                //cerr<<"请求超时 结束读取"<<endl;
                sleep(0.2);
                break;
            }
            }

            if(recvStatu==0)
            {
               // cout<<"接收正常完毕 长度:"<<nCount<<endl;
                Buffer[nCount+1]=0;
                break;
            }
            Left-=recvStatu;
            nCount+=recvStatu;
            }
            if(nCount==0)
            {
                  time (&timep);
        cout<<ctime(&timep)<<((ProxySocket*)curRequest)->GetClientIp()<<"与"<<header->host<<" fd: "<<((ProxySocket*)curRequest)->client<<"结束通信"<<endl;
((ProxySocket*)curRequest)->OverConnection();
	       	    return NULL;
            }       
            if(cache&&header->Is304(Buffer))
            {
                _size=send(((ProxySocket*)curRequest)->client,cacheFile,1<<20,0);
                cout<<"成功发送缓存"<<endl;
            }
            else
            {
                CacheManager::GetIntance()->MakeCache(Buffer,header->url,nCount);
                _size=send(((ProxySocket*)curRequest)->client,Buffer,nCount,0);
            }    
            //cout<<"发送数据大小"<<_size<<"实际数据大小:"<<nCount<<endl;
            if(_size==-1)
                cout<<"错误码:"<<errno<<endl;


        }
        while(longConnection);
        }
        time (&timep);
        cout<<ctime(&timep)<<((ProxySocket*)curRequest)->GetClientIp()<<"与"<<header->host<<" fd: "<<((ProxySocket*)curRequest)->client<<"结束通信"<<endl;

	((ProxySocket*)curRequest)->OverConnection();
        return NULL;
}

int main()
{
    signal(SIGINT, FreePort);
    if(!Init())
        return -1;
    int clientSockfd;
    //最大线程数为10的线程池
    threadPool=new ThreadPool(10);
    while(1)
    {
       // cout<<"等待新用户连接"<<endl;
        clientSockfd=accept(proxySockfd,NULL,NULL);
        //cout<<clientSockfd<<" ";
        if(clientSockfd!=-1||clientSockfd!=0)
        {
        ProxySocket *curRequest=new ProxySocket();
        curRequest->client=clientSockfd;
        //cout<<"新连接客户端"<<curRequest->GetClientIp()<<endl;
        if(filter->JudgeIp(curRequest->GetClientIp()))
            cout<<"客户端"<<curRequest->GetClientIp()<<"是过滤对象,屏蔽"<<endl;
        else
            threadPool->Add(ProxyServer,curRequest);
        }
        sleep(0.2);
    }
    cout<<"GG"<<endl;
    return 0;
}
