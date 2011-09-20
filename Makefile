TARGETS=client server
all: ${TARGETS}

server: server.o server_tcp.o
	g++ -o server server.o server_tcp.o -Wall -D_REETRANT -lpthread

client: client.o client_tcp.o
	g++ -o client client.o client_tcp.o -Wall -D_REETRANT -lpthread

server.o: server.cc
	g++ -c server.cc -Wall -D_REETRANT -lpthread

server_tcp.o: server_tcp.cc
	g++ -c server_tcp.cc -Wall -D_REETRANT -lpthread

client_tcp.o: client_tcp.cc
	g++ -c client_tcp.cc -Wall -D_REETRANT -lpthread

clean:
	rm -rf *.o server client

