all: server.c camera.c servo.c
		gcc -pthread -o picam server.c camera.c servo.c

client: client.c 
	gcc -pthread -o picamclient client.c

server: server.c
	gcc -pthread -o server server.c


.PHONY: clean
clean:
    rm -f *.o $(objects)
