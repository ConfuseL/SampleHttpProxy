cc=g++
exe=sampleHttpProxy
obj=main.o
$(exe):$(obj)
	$(cc) -o $(exe) $(obj) -lpthread
main.o:  main.cpp ProxySocket.hpp HttpHeader.hpp ThreadPool.hpp Filter.hpp CacheManager.hpp
	$(cc) -c main.cpp -std=gnu++11 
clean:
	rm *.o $(exe)
