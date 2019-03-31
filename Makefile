CC=gcc
DBFLAGS=-g

make: server.c 
	$(CC) $(CFLAGS)  -pthread server.c



clean: 
	rm *.o
