#ifndef HTTPHEADER_HPP_INCLUDED
#define HTTPHEADER_HPP_INCLUDED
#include<string.h>
const int maxLen=1<<10;
class HttpHeader
{
public  :
    char method[4]={0};
    char url[maxLen]={0};
    char host[maxLen]={0};
    char cookie[maxLen<<2]={0};
    char date[30]={0};
    bool isKeepAlive=false;
    HttpHeader(char * buffer)
    {
        char * analysis;
        char * remain;
        const char *key="\r\n";
        char *analysisBuffer=new char[strlen(buffer)+1];
        memcpy(analysisBuffer,buffer,strlen(buffer)+1);
        analysis=strtok_r(analysisBuffer,key,&remain);
        //GET http://confusel.tech/ HTTP/1.1
        if(analysis[0]=='G')
        {
            memcpy(method,"GET",3);
            memcpy(url,&analysis[4],strlen(analysis)-4-9);
        }
        else if(analysis[0]=='P')
        {
            memcpy(method,"POST",4);
            memcpy(url,&analysis[5],strlen(analysis)-5-9);
        }
        analysis=strtok_r(NULL,key,&remain);
        while(analysis)
        {
            if(analysis[0]=='H')
            {
                //Host: confusel.tech
                 memcpy(host,&analysis[6],strlen(analysis)-6);
            }
            else if(analysis[0]=='C')
            {
                //Cookie: nc_sameSiteCookielax=true; nc_sameSiteCookiestrict=true; nc_username=Con
                if(strlen(analysis)>8)
                {
                    char title[8];
                    memcpy(title,analysis,6);
                    if(!strcmp(title,"Cookie"))
                    {
                        memcpy(cookie,&analysis[8],strlen(analysis)-8);
                    }
                }
            }
            else if(analysis[0]=='P')
            {
                 //std::cout<<analysis<<std::endl;
                //Cookie: nc_sameSiteCookielax=true; nc_sameSiteCookiestrict=true; nc_username=Con
                if(strlen(analysis)>18)
                {
                    char *title=new char[18];
                    memcpy(title,analysis,16);
                    if(!strcmp(title,"Proxy-Connection"))
                    {
                        char status[100];
                        memcpy(status,&analysis[18],strlen(analysis)-18);
                       // std::cout<<status<<std::endl;
                        if(!strcasecmp(status,"keep-alive"))
                            isKeepAlive=true;
                    }
                }
            }

            if (strstr(analysis, "Date") != NULL) {
            memcpy(date, &analysis[6], strlen(analysis) - 6);
            }

            analysis=strtok_r(NULL,key,&remain);
        }
    }
    void GetNewHttpHeader(char *buffer,char *_date)
    {
        const char *_host = "Host";
        const char *newfield = "If-Modified-Since: ";
        char temp[1<<20];
        char *pos = strstr(buffer, _host);
        for (int i = 0; i < strlen(pos); i++) {
            temp[i] = pos[i];
        }
        *pos = '\0';
        while (*newfield != '\0') {  
            *pos++ = *newfield++;
        }
        while (*_date != '\0') {
            *pos++ = *_date++;
        }
        *pos++ = '\r';
        *pos++ = '\n';
        for (int i = 0; i < strlen(temp); i++) {
            *pos++ = temp[i];
        }
    }
    bool Is304(char * buffer)
    {
        char *p, *ptr, num[10]={0}, tempBuffer[1<<20]={0};
        const char * delim = "\r\n";
        memcpy(tempBuffer, buffer, strlen(buffer));
        p = strtok_r(tempBuffer, delim, &ptr);
        memcpy(num, &p[9], 3);
        if (strcmp(num, "304") == 0) {  
            std::cout<<"响应头回复304"<<std::endl;
            return true;
        }
        return false;
    }
};

#endif // PROXYSOCKET_HPP
