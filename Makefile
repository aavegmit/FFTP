# Macros

CC = g++
CFLAGS = -g -Wall -D_REENTRANT
OBJ = 
LIBS = -lcrypto -lpthread
INC = 
#LIBS = -L/home.scf-22/csci551b/openssl/lib -lcrypto -lnsl -lsocket -lresolv
#INC = -I/home/scf-22/csci551b/openssl/include

# Explicit rule
all: server client

server: $(OBJ)
	$(CC) $(CFLAGS) -o server $(OBJ) $(INC) $(LIBS) 

client: $(OBJ)
	$(CC) $(CFLAGS) -o client $(OBJ) $(INC) $(LIBS) 

install:
#	cp sv_node bnode1/
#	cp sv_node bnode2/
#	cp sv_node bnode3/
#	cp sv_node nonbnode1/
#	cp sv_node nonbnode2/	

clean:
	rm -rf *.o sv_node 

server.o: server.cc
	$(CC) $(CFLAGS) -c server.cc $(INC)
client.o: client.cc
	$(CC) $(CFLAGS) -c client.cc $(INC)

