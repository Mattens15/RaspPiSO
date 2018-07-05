#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <err.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include "camera.c"

char imageheader[]= "HTTP/1.1 200 Ok\r\n""Content-Type: image/jpg\r\n\r\n";
char response[] ="HTTP/1.1 200 Ok\r\n"
"Content-Type: text/html; charset=UTF-8\r\n\r\n"
"<!DOCTYPE html>\r\n"
"<html lang='it'><head><title>Raspberry Pi Surveillance</title><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1'><link rel='stylesheet' href='https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0-alpha.6/css/bootstrap.min.css' integrity='sha384-rwoIResjU2yc3z8GV/NPeZWAv56rSmLldC3R/AZzGRnGxQQKnKkoFVhFQhNUwEyJ' crossorigin='anonymous'><link rel='stylesheet' href='https://v4-alpha.getbootstrap.com/examples/sticky-footer/sticky-footer.css'><script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script><script src='https://cdnjs.cloudflare.com/ajax/libs/tether/1.4.0/js/tether.min.js' integrity='sha384-DztdAPBWPRXSA/3eYEEUWrWCy7G5KFbe8fFjk5JAIxUYHKkDx6Qin1DkWx51bBrb' crossorigin='anonymous'></script><script src='https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0-alpha.6/js/bootstrap.min.js' integrity='sha384-vBWWzlZJ8ea9aCX4pEW3rVHjgjt7zpkNpZk+02D9phzyeVkE+jo0ieGizqPLForn' crossorigin='anonymous'></script></head>\r\n"
"<body><div class='container-fluid' style='padding-left:5%; padding-right:5%;height:100%'><!-- TITLE --><div class='row' style='padding-top: 2%'><div class='col-12 col-md-12 col-sm-12 col-xl-12'><center><h1>RASPBERRY PI VIDEO SURVEILLANCE</h1></center></div></div><!-- SEZIONE LIVE --><div class='row' style='height:30%;'><div class='col-12 col-md-12 col-sm-12 col-xl-12'><center><img id='iframeid' class='img-responsive' src='test.jpg' style='width:70%;height:50%;'></center></div></div><!-- SEZIONE COMANDI --><div class='row' style='padding-top: 1%;'><div class='col-12 col-sm-12 col-md-12' style='position:sticky;'><center><form name='control-form' method='post'><div class='form-group'><input id='comandosx' type='submit' class='btn btn-inline' style='margin: 1%;' name='comandi' value='SX (45°)'><input id='comandodx' type='submit' class='btn btn-inline' name='comandi' value='DX (45°)'></div><div class='form-group'><label><h3>Position: </h3></label><input id='coordinate' style='margin-left:1%;margin-right:1%;' placeholder='0-359°' type='number' name='position' class='form-control-inline' min='0' max='359'><input id='absoluteposition' type='submit'  name='sendposition' value='INVIA' class='btn btn-inline'></div><div class='form-group'><label><h3>Resolution: </h3></label><input id='resolutionw' placeholder='w' style=margin-left:1%;' type='number' name='position' class='form-control-inline' min='240' max='2560'><input id='resolutionh' placeholder='h' style='margin-left:1%;margin-right:1%;' type='number' name='position' class='form-control-inline' min='240' max='1920'><input id='scatta' type='submit' class='btn btn-inline btn-danger' name='comandi' value='SCATTA'></div></form></center></div></div></div><footer class='footer' style='background-color: lightgrey; padding-bottom: 1%;position:absolute; bottom: 0; width: 100%;'><div class='container' style='background-color: lightgrey'><center><span class='text-danger'>Coded by: Matteo Attenni & Raoul Nuccetelli & Mihai Zapodeanu</span></center></div></footer></body>"
"</html>";
 
int main()
{
  int one = 1, client_fd;
  int w=2592;
  int h=1944;
  
  int default_size=w*h;
  char buf[default_size];
  int fdimg;

  struct sockaddr_in svr_addr, cli_addr;
  socklen_t sin_len = sizeof(cli_addr);
 
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0)
    err(1, "Non riesco ad aprie il socket");
 
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int));
 
  int port = 8080;
  svr_addr.sin_family = AF_INET;
  svr_addr.sin_addr.s_addr = INADDR_ANY;
  svr_addr.sin_port = htons(port);
 
  if (bind(sock, (struct sockaddr *) &svr_addr, sizeof(svr_addr)) == -1) {
    close(sock);
    err(1, "Errore binding");
  }
 
  listen(sock, 5);
  printf("Server in attesa di connessioni\n");
 if(!fork())
   startLoopCapturingTimer(10,w,h);

   
  while (1) {
   
    
     
    client_fd = accept(sock, (struct sockaddr *) &cli_addr, &sin_len);
    
 
    if (client_fd == -1) {
      perror("Errore accettazione connessione");
      continue;
    }
    
    memset(buf,0,default_size);
    read(client_fd,buf,default_size-1);
    if(!strncmp(buf,"GET /test.jpg",13)){
                printf("Invio immagine in corso\n");
                write(client_fd,imageheader,sizeof(imageheader)-1);
                fdimg=open("test.jpg",O_RDONLY);
                int sent=sendfile(client_fd,fdimg,NULL,default_size);
                printf("Inviata %d:",sent);
                close(fdimg);
            }
    else
    {
    write(client_fd, response, sizeof(response) - 1); /*-1:'\0'*/
    }
    close(client_fd);
    
    
  }
}