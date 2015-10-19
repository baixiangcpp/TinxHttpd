CC = g++
C11 = -std=c++11
THREAD = -lpthread
OBJECT = WebServer.o Epoll.o ThreadPool.o Request.o  MessageLog.o Daemonize.o
TinxHttpd:$(OBJECT)
	$(CC) -o TinxHttpd  main.cpp $(OBJECT) $(C11) $(THREAD)
WebServer.o:Epoll.o MessageLog.o Request.o WebServer.h WebServer.cpp
	$(CC) -c WebServer.cpp $(C11) 
Epoll.o:ThreadPool.o MessageLog.o Epoll.h Epoll.cpp
	$(CC) -c Epoll.cpp $(C11)
ThreadPool.o:Daemonize.o ThreadPool.h ThreadPool.cpp
	$(CC) -c ThreadPool.cpp $(C11) 
Request.o:MessageLog.o Request.h Request.cpp
	$(CC) -c Request.cpp $(C11)
Daemonize.o: MessageLog.o Daemonize.cpp Daemonize.h
	$(CC) -c Daemonize.cpp 
MessageLog.o: MessageLog.h MessageLog.cpp
	$(CC)  -c MessageLog.cpp
install:
	install ./TinxHttpd /usr/bin
	chmod +s /usr/bin/TinxHttpd
clean:
	rm $(OBJECT) -f 
