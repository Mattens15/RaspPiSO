#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>
#include "camera.h"
#include "servo.h"
#include <errno.h>


#define DEFAULT_WIDTH 2592;
#define DEFAULT_HEIGHT 1954;
#define SHOOTRATE 15; 

pthread_t shooting_loop;
pthread_t servo_thread;
char nome_file[100];




typedef struct shooting_args_s{
  int rate;
  int w;
  int h;
} shooting_args_t;

typedef struct servo_args_s{
  float degrees;
} servo_args_t;


//crating library for control camera from c or c++
void scatta(int w,int h )// this function capture the image and save it in currentdate.jpg
{   char w_s[10];
    char h_s[10];
    char date[255];
    time_t rawtime;
    time(&rawtime);
    sprintf(date,ctime(&rawtime));
    char* p=date;
    for(; *p; ++p){
        if(*p==" ")
            *p="";
    }
    sprintf(w_s," -w %d ",w);
    sprintf(h_s,"-h %d ",h);
    char command[50];
    strcpy(command,strcat("raspistill -o",strcat(*p,".jpg")));
    strcat(command,w_s);
    strcat(command,h_s);
    printf("%s\n",command);
    system(command);
}
void*sendFotoToclient(int* desc )
{
   int connfd=(int)*desc;
   printf("Connessione accettata e id:%d\n",connfd);
   printf("Connesso con il client: %s:%d \n",inet_ntoa(c_addr.sin_addr),ntohs(c_addr.sin_port));
   write(connfd,nome_file,256);
   FILE *fb=fopen(nome_file,"rb");
   if(fp==NULL)
   {
     printf("Errore apertura file");
     return 1;
   }
   while(1)
   {
     /*prima leggiamo trace di 256 byte*/
     unsigned char buff[10240]={0};
     int nread=fread(buff,1,10240,fp);
     printf("Byte letti: %d \n",nread)
     //se e stato un successo,inviamo i dati
     if(nread>0)
        {
          printf("Sto inviando la foto....\n");
          write(desc,buff,nread);

        }
        if(nread <10240)
        {
          if(feof(fp))
          {
            printf("fine del file\n");
            printf("Trasferimento del file completato con %d \n",desc);

          }
          if(ferror(fp))
             printf("Errore di lettura\n");
          break;    
        }
     /*
     unsigned char buff[10240]={0};   
     printf("Sto chiudendo la conessione con id: %d\n",connfd);
     close(connfd);
     shutdown(connfd,SHUT_WR)*/
     sleep(170000);
     

   }
}


void* startLoopCapturingTimer(void* arg) {
   
   shooting_args_t* args=(shooting_args_t*) arg;
   int sec=args->rate;
   int w=args->w;
   int h=args->h;
   while(1)
   {    
    printf("Sto catturando...\n");
   	scatta(w,h);
    printf("Ho finito di catturare\n");
    printf("Sleeping...\n");
   	sleep(sec);
   }
}

int main(int argc,char* argv) {
  
  
  int ret;

  int socket_desc,client_sock,c,read_size;
  struct sockaddr_in server,client;
  char client_msg[1024];
  char server_msg[10240];

  socket_desc=socket(AF_INET,SOCK_STREAM,0);
  if(socket_desc==-1){
    printf("Errore di connessione");
    exit(1);
  }
  puts("Socket creata con successo");

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(3000);

  if(bind(socket_desc,(struct sockaddr*)&server,sizeof(server))<0){
    perror("Errore di connessione");
    return 1;
  }

  puts("Connessione su porta 3000 effettuata correttamente");

  listen(socket_desc,3);

  puts("In attesa del client...");
  c=sizeof(struct sockaddr_in);

  client_sock=accept(socket_desc,(struct sockaddr*)&client,(socklen_t*)&c);
  if(client_sock<0){
    perror("Errore in fase di connessione col client");
    return 1;
  }
  puts("Client connesso con successo");

  // inizio ad inviare una foto ogni 15 secondi al client alla risluzione di default di 5mpx
  shooting_args_t* thread_shot_args=malloc(sizeof(shooting_args_t));
  thread_shot_args->rate=SHOOTRATE;
  thread_shot_args->w=DEFAULT_WIDTH;
  thread_shot_args->h=DEFAULT_HEIGHT;
  ret=pthread_create(&shooting_loop,NULL,startLoopCapturingTimer,(void*)thread_shot_args);
  if(ret!=0){
    perror("Errore durante la creazione del thread di cattura della foto");
    return 1;
  }
  

  while(1){

    // CAMBIO RISOLUZIONE CAMERA
    int sended=0;
    int size_mesg=strlen("Ciao, sono il server! Ogni 15 secondi ti mando una foto a risoluzione di 5mpx. Vuoi modificare la risoluzione? (S/N)\n");
    char msg[size_mesg];
    strcpy(msg,"Ciao, sono il server! Ogni 15 secondi ti mando una foto a risoluzione di 5mpx. Vuoi modificare la risoluzione? (S/N)\n",size_mesg)
    int ret=0;
    while(sended<size_mesg)
    {
     ret=write(client_sock,"Ciao, sono il server! Ogni 15 secondi ti mando una foto a risoluzione di 5mpx. Vuoi modificare la risoluzione? (S/N)\n",strlen("Ciao, sono il server! Ogni 15 secondi ti mando una foto a risoluzione di 5mpx. Vuoi modificare la risoluzione?\n"));
       if(ret<0)
       {
         perror("Errore scrittura sul socket")
         break;
       }
       if(ret==0 && errno==EINTR)
       {continue;}
       sended+=ret;

    }  
    
    read_size=recv(client_sock,client_msg,1024,0);
    if(read_size<0){
      perror("Errore in ricezione dal client");
      return 1;
    }
    if(*client_msg=="S"){
      int width;
      int height;
      pthread_cancel(shooting_loop);
      write(client_sock,"Indica larghezza(0-2592)",sizeof("Indica larghezza(0-2592)"));
      read_size=recv(client_sock,client_msg,1024,0);
      if(read_size<0){
        perror("Errore in ricezione dal client");
        return 1;
      }
      width=*client_msg;
      write(client_sock,"Indica altezza(0-1954)",sizeof("Indica altezza(0-1954)"));
      read_size=recv(client_sock,client_msg,1024,0);
      if(read_size<0){
        perror("Errore in ricezione dal client");
        return 1;
      }
      height=*client_msg;
      
      shooting_args_t* thread_shot_args=malloc(sizeof(shooting_args_t));
      thread_shot_args->rate=SHOOTRATE;
      thread_shot_args->w=width;
      thread_shot_args->h=height;
      ret=pthread_create(&shooting_loop,NULL,startLoopCapturingTimer,(void*)thread_shot_args);
      if(ret!=0){
        perror("Errore durante la creazione del thread di cattura della foto");
        return 1;
      }
      pthread_t thread_invio;
      ret=pthread_create(&thread_invio,NULL,&sendFotoToclient,&client_sock);
      if(ret!=0)
      {
        perror("Errore durante la creazione del thread di invio foto");
        return 1;
      }
    }
    
    // MOVIMENTO SERVO
    write(client_sock,"Vuoi spostare il servo per orientare la fotocamera? (S/N)\n",strlen("Vuoi spostare il servo per orientare la fotocamera? (S/N)\n"));
    read_size=recv(client_sock,client_msg,1024,0);
    if(read_size<0){
      perror("Errore in ricezione dal client");
      return 1;
    }
    if(*client_msg=="S"){
      write(client_sock,"Indica di quanti gradi vuoi spostarti (0-359)\n",strlen("Indica di quanti gradi vuoi spostarti (0-359)\n"));
      read_size=recv(client_sock,client_msg,1024,0);
      if(read_size<0){
        perror("Errore in ricezione dal client");
        return 1;
      }
      float dg=atof(*client_msg);
      servo_args_t* servo_control_args=malloc(sizeof(servo_args_t));
      servo_control_args->degrees=dg;
      ret=pthread_create(&servo_thread,NULL,moveCamera,(void*)servo_control_args);
      if(ret!=0){
        perror("Errore durante la creazione del thread per il controllo del servo");
        return 1;
      }
      ret=pthread_detach(servo_thread);
      if(ret!=0){
        perror("Errore in fase di detach del thread servo");
        return 1;
      }
      ret=pthread_detach(thread_invio);
      if(ret!=0)
      {
        perror("Errore in fase detach del thread invio foto");
      }
    }

    write(client_sock,"Vuoi terminare la connessione con il server? (S/N)\n",strlen("Vuoi terminare la connessione con il server? (S/N)\n"));
    read_size=recv(client_sock,client_msg,1024,0);
    if(read_size<0){
      perror("Errore in ricezione dal client");
      return 1;
    }
    if(*client_msg=="S") break;
  }

  puts("Connessione con il client terminata!");
  return 0;
    
}