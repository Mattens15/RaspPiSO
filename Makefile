all: server.c client.c
	gcc -lpthread -o picam server.c 
	gcc -lpthread -o picamclient client.c

client: client.c 
	gcc -lpthread -o picamclient client.c

server: server.c
	gcc -lpthread -o picam server.c


.PHONY: clean
clean:
	rm -f *.o $(objects) picam picamclient
