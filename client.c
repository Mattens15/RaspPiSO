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


void ChiudiSocket(int sock)
{
  int ret=close(sock)
  if(ret<0)
     perror ("Errore chiusura socket");
  return;
}
void CreaSocket(char* Destinazione,int Porta)
{
    struct sockaddr_in temp;
    struct hostent *h;
    int sock;
    int errore;
    //Tipo di indirizzo
    temp.sin_family=AF_INET;
    temp.sin_port(htons(Porta));
    h=gethostbyname(Destinazione);
    if(h==0)
    {
        printf("Gethostbyname fallito\n");
        exit(1);
    }
    bcopy(h->h_addr,&temp.sin_addr,h->h_length);
    //creazione socket
    sock=socket(AF_INET,SOCK_STREAM,0);
    //connessione del socket in caso di errore gestiamo con l'oportuno errore
   errore=connect(sock,(struct sockaddr*) &temp,sizeof(temp));
   if(errore<0)
     {perror("Errore connessione socket!! riprova");
       exit(1);
     }
   return sock;
}

int main(int argc,char* argv)
{
  int socket_server;




}