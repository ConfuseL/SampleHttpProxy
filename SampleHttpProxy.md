![](https://img.shields.io/badge/language-C%2B%2B-blue.svg?style=flat-square)  ![](https://img.shields.io/badge/support-Linux-orange.svg?style=flat-square)

# Introduce

> 计网课设，功能比较简单。
>
> 支持以下功能：
>
> 报文缓存：存储在当前路径的Cache下，会在目的服务的304情况下发送给客户端。
>
> 用户过滤：打开ipFilter.f文件，一行一个ip，会吞掉来自该ip的所有请求并拒绝发送。
>
> 网站过滤：打开hostFilter.f文件，一行一个host，访问该host，会让客户端转向一个[静态网页](<http://120.77.249.7/strive.html>)。
>
> 多线程：每一次与客户端建立连接时，添加一个任务线程，线程中会根据请求头是否为长连接而持续通信，否则仅来回一次。

![](https://confusel-images.oss-cn-shenzhen.aliyuncs.com/GitHub%E5%9B%BE%E5%BA%93/%E8%AE%A1%E7%BD%91%E8%AF%BE%E8%AE%BE.png)

## Existing problems

> 1.有些时候会莫名其妙的被结束进程，但是命令行并没有core dump等提示。
>
> 2.访问某些可能同时又多个http请求的网页，如果数量很大很快，会导致SIGABRT中断。
>
> 3.访问一些以拥有缓存的网页，第一次访问大概率出现400，重新刷新访问即可。

# Install

## Download

```shell
git clone git@github.com:ConfuseL/SampleHttpProxy.git
```

## Quick compile install

```shell
cd SampleHttpProxy/
```

```
make
```

```
make install
```

## How to set proxy

windows

> 1. 打开代理设置：
>
>    Internet属性->连接->局域网设置->勾选为LAN使用代理服务器。
>
>    输入使用该程序的公网ip以及端口16924。

linux

> 1. 通过Super键(win键)打开搜索面板。
> 2. 搜索代理或者proxy
> 3. 进入网络设置
> 4. 进入网络代理选项卡，设置为手动代理，设置ip和端口16924

## How to use

> 在项目目录下运行可执行文件

```
./sampleHttpProxy 
```

> 执行ctrl+c可以关闭程序，以捕获终端信号，程序会在退出前关闭套接字。