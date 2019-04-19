#ifndef CACHEMANAGER_HPP_INCLUDED
#define CACHEMANAGER_HPP_INCLUDED
#include<string.h>
#include"HttpHeader.hpp"
#include<iostream>
class CacheManager
{
private:
    static CacheManager* instance;
    static pthread_once_t once;
    pthread_mutex_t file_mutex;
    const char * key = "\r\n";
	CacheManager(const CacheManager&);
	CacheManager& operator=(const CacheManager&);
    CacheManager()
    {

    };
public:
    static void Init()
    {
        instance=new CacheManager();
    }
    static CacheManager* GetIntance()
    {
        pthread_once(&once,Init);
        return instance;
    }
    void GetCacheName(char *url,char* nameStr)
    {
        char *p = nameStr;
        char *q = (char*)("./Cache/");
        while (*q != '\0') {
			*p++ = *q++;
		}
        while (*url != '\0') {
		if (*url != '/' && *url != ':' && *url != '.') {
			*p++ = *url;
		}
		url++;
	}
    }
    void MakeCache(char *buffer, char *url,int length)
    {
        char *p, *ptr, num[10], tempBuffer[1<<20+1];
        memcpy(tempBuffer, buffer, length);
        p = strtok_r(tempBuffer, key, &ptr);
        memcpy(num, &p[9], 3);
        pthread_mutex_lock(&file_mutex);
        if (strcmp(num, "200") == 0) {  //状态码是200时缓存
            char filename[100] = { 0 };
            GetCacheName(url, filename);
            FILE *out;
            if ((out=fopen(filename, "wb")) != NULL) {
                fwrite(buffer, sizeof(char), length, out);
                std::cout<<"\n来自"<<url<<" 的报文已缓存至 "<<filename<<std::endl;
                fclose(out);
            }

	   }
       pthread_mutex_unlock(&file_mutex);
    }
    bool TryGetCache(char *url,char *buffer,char *fileBuffer)
    {
        bool thereIs=false;
        if(strlen(url)>=90)
            return false;
        char filename[100] = { 0 }; 
        GetCacheName(url, filename);
        FILE *in;
        pthread_mutex_lock(&file_mutex);
            if ((in=fopen(filename, "rb")) != NULL) {
            fread(fileBuffer, sizeof(char), 1<<20, in);
            std::cout<<"\n"<<url<<" 存在缓存 成功读取 "<<std::endl;
            fclose(in);
            HttpHeader *header=new HttpHeader(fileBuffer);
            header->GetNewHttpHeader(buffer,header->date);
            thereIs= true;
        }
        pthread_mutex_unlock(&file_mutex);
        return thereIs;
    }
};
CacheManager* CacheManager::instance = NULL;
pthread_once_t CacheManager::once = PTHREAD_ONCE_INIT;
#endif // CACHEMANAGER_HPP_INCLUDED
