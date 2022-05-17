CC=g++


all: server client Test

client: client.cpp
	$(CC) client.cpp -o client
		./client "127.0.0.1"

server: server.cpp stack.cpp
	$(CC) server.cpp stack.cpp -lpthread -o server
		./server

test:
	$(CC) Test.cpp -o test
		./test


.PHONY: clean

clean:
	rm *.o server client test