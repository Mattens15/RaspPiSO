#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

pthread_t receiving_thread;
char server_reply[20000];
char message[1024];

typedef struct receiving_args_s{
  int descr;
} receiving_args_t;

int sock=0;

void kill_handler(int sign) {
  puts("Ricevuto segnale di chiusura, chiudo tutto!");
  if(sock != 0) close(sock);
  exit(1);
}

void* recevingPhoto(void* arg) {
    struct sockaddr_in server;

    int photo_sock = socket(AF_INET , SOCK_STREAM , 0);
    if (photo_sock == -1){
        printf("errore creazione socket");
    }
    puts("Socket creato");

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons( 3000 );

    if (connect(photo_sock , (struct sockaddr *)&server , sizeof(server)) < 0){
        perror("Non trovo un server a cui collegarmi!");
        pthread_exit(NULL);
    }
    puts("Connesso correttamente");

    int ret;
    printf("%ld",sizeof(server_reply));
    while(1){
        ret = recv(photo_sock , server_reply , sizeof(server_reply) , 0);
        if(ret <= 0){
            puts("errore collegamento");
            close(photo_sock);
            exit(1);
        }
        int server_reply_size = ret;
        printf("Dimensione del file: %d\n",server_reply_size);
        if(server_reply_size > 5000){
            // scrittura su file
            FILE* foto;
            foto = fopen("test2.jpg","wb");
            fwrite(server_reply,1,server_reply_size,foto);
            fclose(foto);
            puts("ho memorizzato il file");
        }
    }
}


int main(int argc , char *argv[]){
    struct sockaddr_in server;

    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1){
        printf("errore creazione socket");
    }
    puts("Socket creato");

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons( 3000 );

    signal(SIGINT,kill_handler);

    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0){
        perror("non trovo server a cui collegarmi");
        return 1;
    }

    puts("Connesso correttamente");


    receiving_args_t* receive_thread_args=malloc(sizeof(receiving_args_t));
    receive_thread_args->descr=sock;

    int ret=pthread_create(&receiving_thread,NULL,recevingPhoto,(void*)receive_thread_args);
    if(ret!=0){
        perror("Errore durante la creazione del thread di cattura della foto");
        return 1;
    }

    // interazione a domande col server per la modifica di parametri di risoluzione/movimento del servo
    while(1){
        while ((ret = recv(sock, message, 1024, 0)) < 0 ) {
            if (errno == EINTR) continue;
            if(ret == -1) {
                fprintf(stderr,"Errno on recv: %d, Error message: %s\n",errno,strerror(errno));
                return -1;
            }
        }
        message[ret] = '\0';

        puts("il server dice :");
        puts(message);

        printf("inserire comando da inviare : ");
        scanf("%s" , message);

        //message contiene il messaggio che manda il client!
        if( send(sock , message , strlen(message) , 0) < 0){
            puts("invio fallito");
            return 1;
        }
    }

    close(sock);
    return 0;
}