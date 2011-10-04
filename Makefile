TARGETS=client server
all: ${TARGETS}

server: server.o server_tcp.o server_udp.o fileIO.o shared.o bitvector.o cache.o
	g++ -o server server.o server_tcp.o server_udp.o shared.o fileIO.o bitvector.o cache.o  -D_REETRANT -lpthread

client: client.o client_tcp.o client_udp.o shared.o bitvector.o fileIO.o
	g++ -o client client.o client_tcp.o client_udp.o shared.o bitvector.o fileIO.o  -D_REETRANT -lpthread

server.o: server.cc
	g++ -c -g server.cc  -D_REETRANT -lpthread

client.o: client.cc
	g++ -c -g client.cc  -D_REETRANT -lpthread

server_tcp.o: server_tcp.cc
	g++ -c -g server_tcp.cc  -D_REETRANT -lpthread

client_tcp.o: client_tcp.cc
	g++ -c -g client_tcp.cc  -D_REETRANT -lpthread

shared.o: shared.cc
	g++ -c -g shared.cc  -D_REETRANT -lpthread

server_udp.o: server_udp.cc
	g++ -c -g server_udp.cc  -D_REETRANT -lpthread

client_udp.o: client_udp.cc
	g++ -c -g client_udp.cc  -D_REETRANT -lpthread

fileIO.o: fileIO.cc
	g++ -c -g fileIO.cc  -D_REETRANT -lpthread

cache.o: cache.cc
	g++ -c -g cache.cc  -D_REETRANT -lpthread

bitvector.o: bitvector.cc
	g++ -c -g bitvector.cc  -D_REETRANT -lpthread

clean:
	rm -rf *.o server client

