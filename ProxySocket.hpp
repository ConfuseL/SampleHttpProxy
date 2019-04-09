#ifndef PROXYSOCKET_HPP_INCLUDED
#define PROXYSOCKET_HPP_INCLUDED
#include<netdb.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<iostream>
#include<errno.h>
const int ERR=-1;
class ProxySocket
{
    public:
    int server;
    int client;
    bool TryConnect2Server(char *host)
    {
        std::cout<<"开始连接服务器"<<std::endl;
        struct sockaddr_in serveraddr;
        serveraddr.sin_family=AF_INET;
        serveraddr.sin_port=htons(80);
        //这个函数可以通过域名获得主机的信息
        hostent * h=gethostbyname(host);
        if(!h)
        {

            std::cerr<<"无法获取主机信息 "<<errno<<std::endl;
            return false;
        }
        //获得主机网络地址
        in_addr inAddr=*((in_addr *)*h->h_addr_list);
        serveraddr.sin_addr.s_addr=inet_addr(inet_ntoa(inAddr));
        if((server=socket(AF_INET,SOCK_STREAM,0))==ERR)
        {
            std::cerr<<"尝试与主机连接时，无法创建套接字"<<std::endl;
            return false;
        }
       // std::cout<<"ip :"<<(inet_ntoa(inAddr))<<std::endl;
        if(connect(server,(struct sockaddr *)&serveraddr,sizeof(serveraddr))==ERR)
        {
            std::cerr<<"尝试与主机连接失败 "<<errno<<std::endl;
            close(server);
            return false;
        }
        return true;
    }
    void OverConnection()
    {
        close(client);
        close(server);
    }
    char* GetClientIp()
    {
        char *emptyString=(char *)"";
            struct sockaddr_in clientAddr;
            socklen_t len =sizeof(clientAddr);
            if((getsockname(client,(struct sockaddr*)&clientAddr,&len))==0)
            {
                return inet_ntoa(clientAddr.sin_addr);
            }
        else
            return emptyString;
    }
};


#endif // PROXYSOCKET_HPP
