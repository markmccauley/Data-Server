# makefile

all: dataserver client

NetworkRequestChannel.o: NetworkRequestChannel.h NetworkRequestChannel.cpp
	g++ -g -w -Wall -O1 -std=c++11 -c NetworkRequestChannel.cpp

BoundedBuffer.o: BoundedBuffer.h BoundedBuffer.cpp
	g++ -g -w -Wall -O1 -std=c++11 -c BoundedBuffer.cpp

Histogram.o: Histogram.h Histogram.cpp
	g++ -g -w -Wall -O1 -std=c++11 -c Histogram.cpp

dataserver: dataserver.cpp NetworkRequestChannel.o
	g++ -g -w -Wall -O1 -std=c++11 -o dataserver dataserver.cpp NetworkRequestChannel.o -lpthread -lrt

client: client.cpp NetworkRequestChannel.o BoundedBuffer.o Histogram.o 
	g++ -g -w -Wall -O1 -std=c++11 -o client client.cpp NetworkRequestChannel.o BoundedBuffer.o Histogram.o -lpthread -lrt

clean:
	rm -rf *.o fifo* mq* shm* dataserver client
