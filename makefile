CC=g++

all: server1 server2 server3 client1 client2

utilities: utilities.cpp utilities.h
	$(CC) -o utilities utilities.cpp -lsocket -lnsl -lresolv

server1: dhtserver1.cpp utilities.cpp
	$(CC) -o server1 dhtserver1.cpp utilities.cpp -lsocket -lnsl -lresolv

server2: dhtserver2.cpp utilities.cpp
	$(CC) -o server2 dhtserver2.cpp utilities.cpp -lsocket -lnsl -lresolv

server3: dhtserver3.cpp utilities.cpp
	$(CC) -o server3 dhtserver3.cpp utilities.cpp -lsocket -lnsl -lresolv

client1: client1.cpp utilities.cpp
	$(CC) -o client1 client1.cpp utilities.cpp -lsocket -lnsl -lresolv

client2: client2.cpp utilities.cpp
	$(CC) -o client2 client2.cpp utilities.cpp -lsocket -lnsl -lresolv

clean :
	rm server1 server2 server3 client1 client2