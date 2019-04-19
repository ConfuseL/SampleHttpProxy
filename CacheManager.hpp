#ifndef CACHEMANAGER_HPP_INCLUDED
#define CACHEMANAGER_HPP_INCLUDED
#include<string.h>
#include<iostream>
class CacheManager
{
private:
    static CacheManager* instance;
    static pthread_once_t once;
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
        p = strtok_r(tempBuffer, key, &ptr);//提取第一行
        memcpy(num, &p[9], 3);
        if (strcmp(num, "200") == 0) {  //状态码是200时缓存
            //printf("url : %s\n", url);
            char filename[100] = { 0 };  // 构造文件名
            GetCacheName(url, filename);
            //printf("filename : %s\n", filename);
            FILE *out;
            if ((out=fopen(filename, "wb")) != NULL) {
                fwrite(buffer, sizeof(char), strlen(buffer), out);
                fclose(out);
                std::cout<<"\n来自"<<url<<" 的报文已缓存至 "<<filename<<std::endl;
            }
	}
}

};
CacheManager* CacheManager::instance = NULL;
pthread_once_t CacheManager::once = PTHREAD_ONCE_INIT;
#endif // CACHEMANAGER_HPP_INCLUDED
