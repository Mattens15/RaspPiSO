#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>

char webpage[]=
"HTTP/1.1 200 Ok\r\n"
"Content-Type: text/html; charset=UTF-8\r\n\r\n"
"<!DOCTYPE html>\r\n"
"<html lang='it'><head><title>Raspberry Pi Surveillance</title><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1'><link rel='stylesheet' href='https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0-alpha.6/css/bootstrap.min.css' integrity='sha384-rwoIResjU2yc3z8GV/NPeZWAv56rSmLldC3R/AZzGRnGxQQKnKkoFVhFQhNUwEyJ' crossorigin='anonymous'><link rel='stylesheet' href='https://v4-alpha.getbootstrap.com/examples/sticky-footer/sticky-footer.css'><script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script><script src='https://cdnjs.cloudflare.com/ajax/libs/tether/1.4.0/js/tether.min.js' integrity='sha384-DztdAPBWPRXSA/3eYEEUWrWCy7G5KFbe8fFjk5JAIxUYHKkDx6Qin1DkWx51bBrb' crossorigin='anonymous'></script><script src='https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0-alpha.6/js/bootstrap.min.js' integrity='sha384-vBWWzlZJ8ea9aCX4pEW3rVHjgjt7zpkNpZk+02D9phzyeVkE+jo0ieGizqPLForn' crossorigin='anonymous'></script></head>\r\n"
"<body><div class='container-fluid' style='padding-left:5%; padding-right:5%;'><!-- TITLE --><div class='row' style='padding-top: 2%'><div class='col-12 col-md-12 col-sm-12 col-xl-12'><center><h1>RASPBERRY PI VIDEO SURVEILLANCE</h1></center></div></div><!-- SEZIONE LIVE --><div class='row' style='height:60vh;'><div class='col-12 col-md-12 col-sm-12 col-xl-12'><center><img id='iframeid' src='stream/test.jpg' style='width:100%; height:100%;'></center></div></div><!-- SEZIONE COMANDI --><div class='row' style='padding-top: 1%;'><div class='col-12 col-sm-12 col-md-12' style='position:sticky;'><center><form name='control-form' method='post'><div class='form-group'><input id='comandosx' type='submit' class='btn btn-inline' style='margin: 1%;' name='comandi' value='SX (45°)'><input id='comandodx' type='submit' class='btn btn-inline' name='comandi' value='DX (45°)'></div><div class='form-group'><label><h3>Position: </h3></label><input id='coordinate' style='margin-left:1%;margin-right:1%;' placeholder='0-359°' type='number' name='position' class='form-control-inline' min='0' max='359'><input id='absoluteposition' type='submit'  name='sendposition' value='INVIA' class='btn btn-inline'></div><div class='form-group'><label><h3>Resolution: </h3></label><input id='resolutionw' placeholder='w' style=margin-left:1%;' type='number' name='position' class='form-control-inline' min='240' max='2560'><input id='resolutionh' placeholder='h' style='margin-left:1%;margin-right:1%;' type='number' name='position' class='form-control-inline' min='240' max='1920'><input id='scatta' type='submit' class='btn btn-inline btn-danger' name='comandi' value='SCATTA'></div></form></center></div></div></div><footer class='footer' style='background-color: lightgrey; padding-bottom: 1%;position:absolute; bottom: 0; width: 100%;'><div class='container' style='background-color: lightgrey'><center><span class='text-danger'>Coded by: Matteo Attenni & Raoul Nuccetelli & Mihai Zapodeanu</span></center></div></footer></body>"
"</html>";

char imageheader[] = "HTTP/1.1 200 Ok\r\n""Content-Type: image/jpg\r\n\r\n";

int main(){
    struct sockaddr_in server_addr,client_addr;
    socklen_t sin_len=sizeof(client_addr);
    int fd_server,fd_client;
    char buf[1024];
    int fdimg;
    int on=1;
    fd_server=socket(AF_INET,SOCK_STREAM,0);
    if(fd_server<0){
        perror("Problema nella crezione della socket");
        exit(1);
    }
    setsockopt(fd_server, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);
    
    if(bind(fd_server, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1){
        perror("Problema nel binding");
        close(fd_server);
        exit(1);
    }

    if(listen(fd_server,10)==-1){
        perror("Problema nel listening");
        close(fd_server);
        exit(1);
    }
    printf("Connessione avvenuta con successo! ");
    while(1){
        fd_client=accept(fd_server,(struct sockaddr*)&client_addr,&sin_len);
        if(fd_client==-1){
            perror("Connessione fallita...\n");
            continue;
        }
        if(!fork()){
            close(fd_server);
            memset(buf,0,2048);
            read(fd_client,buf,2047);
            printf("%s\n",buf);
            if(!strncmp(buf,"GET /test.jpg",13)){
                printf("Invio immagine in corso\n");
                write(fd_client,imageheader,sizeof(imageheader)-1);
                fdimg=open("test.jpg",O_RDONLY);
                int sent=sendfile(fd_client,fdimg,NULL,10000);
                printf("Inviata %d:",sent);
                close(fdimg);
            }
            else write(fd_client,webpage,sizeof(webpage)-1);
            close(fd_client);
            printf("Chiusura della connessione...\n");
            exit(0);
        }
        close(fd_client);
    }
    return 0;
}
