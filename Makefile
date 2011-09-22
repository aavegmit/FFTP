TARGETS=client server
all: ${TARGETS}

server: server.o server_tcp.o server_udp.o fileIO.o shared.o
	g++ -o server server.o server_tcp.o server_udp.o shared.o fileIO.o -Wall -D_REETRANT -lpthread

client: client.o client_tcp.o client_udp.o shared.o
	g++ -o client client.o client_tcp.o client_udp.o shared.o -Wall -D_REETRANT -lpthread

server.o: server.cc
	g++ -c -g server.cc -Wall -D_REETRANT -lpthread

client.o: client.cc
	g++ -c client.cc -Wall -D_REETRANT -lpthread

server_tcp.o: server_tcp.cc
	g++ -c -g server_tcp.cc -Wall -D_REETRANT -lpthread

client_tcp.o: client_tcp.cc
	g++ -c client_tcp.cc -Wall -D_REETRANT -lpthread

shared.o: shared.cc
	g++ -c shared.cc -Wall -D_REETRANT -lpthread

server_udp.o: server_udp.cc
	g++ -c -g server_udp.cc -Wall -D_REETRANT -lpthread

client_udp.o: client_udp.cc
	g++ -c client_udp.cc -Wall -D_REETRANT -lpthread

fileIO.o: fileIO.cc
	g++ -c -g fileIO.cc -Wall -D_REETRANT -lpthread

clean:
	rm -rf *.o server client

