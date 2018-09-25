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
#include <signal.h>
//#include <bcm2835.h>

#define DEFAULT_WIDTH 3280;
#define DEFAULT_HEIGHT 2464;
#define SHOOTRATE 15;
#define SENDINGRATE 20;
#define PORT 3000

// servo Raoul
#define PI 180.0
#define PIN RPI_GPIO_P1_12
#define CHANNEL 0
#define RANGE 1024

// thread per la cattura della foto
pthread_t shooting_loop;
//thread per l'invio della foto
pthread_t sending_photo;
// foto da mandare dal momento che non abbiamo una camera 
char nome_file[100] = "test.jpg";
int flag = 1;

int photo_sock=0;
int socket_desc=0;

//argomenti necessari per il loop di cattura: frequenza di scatto, larghezza, altezza della foto
typedef struct shooting_args_s
{
  int shooting_rate;
  int w;
  int h;
} shooting_args_t;

//argomenti necessari per l'invio della foto: frequenza d'invio e descriptor della socket
typedef struct sending_args_s
{
  int sending_rate;
  int descr;
} sending_args_t;

// FUNZIONE DI SCATTO CON RASPISTILL
/* Prendo come parametri larghezza ed altezza della foto da passare come parametri a raspistill;
   Lancio il comando di scatto e salvo la foto in un file di nome currentdate.jpg con currentdate = data odierna
*/
void scatta(int w, int h) {

  // imposto i parametri larghezza, altezza e currentdate ( mi servirà per nominare il file della foto)
  char w_s[10];
  char h_s[10];
  char date[255];
  time_t rawtime;
  time(&rawtime);
  sprintf(date, "%s",ctime(&rawtime));
  char *p = date;
  for (; *p; ++p) {
    if (*p == ' ')
      *p = '-';
  }

  // scrivo i parametri larghezza e altezza negli array dedicati
  sprintf(w_s, " -w %d ", w);
  sprintf(h_s, "-h %d ", h);
  char command[50];

  // popolo il comando raspistill -> "rapistill -o currentdate.jpg larghezza altezza"
  strcpy(command, strcat("raspistill -o", strcat(p, ".jpg")));
  strcat(command, w_s);
  strcat(command, h_s);
  printf("%s\n", command);

  // lancio il comando raspistill e catturo la foto
  //system(command);
}

// thread di cattura automatica delle foto settata a 'rate' secondi
void *startLoopCapturingTimer(void *arg) {

  // popolamento degli argomenti rate, larghezza e altezza per il ciclo di cattura
  shooting_args_t *args = (shooting_args_t *)arg;
  int sec = args->shooting_rate;
  int w = args->w;
  int h = args->h;
  flag = 1;
  // ciclo while di cattura in cui w,h vengono passati come parametri alla funziona scatta che lancia il comando raspistill
  while (flag) {
    printf("Sto catturando...\n");
    //scatta(w,h);
    printf("Ho finito di catturare\n");
    printf("Sleeping...\n");
    sleep(sec);
  }
  pthread_exit(NULL);
}

void *sendPhotoToClient(void *arg) {

  // popolamento degli argomenti rate, socket descriptot per il ciclo d'invio
  sending_args_t *args = (sending_args_t *)arg;
  int rate = args->sending_rate;
  int socket_desc = args->descr;

  struct sockaddr_in client;
  int c = sizeof(struct sockaddr_in);

  photo_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c);
  if (photo_sock < 0) {
    perror("Errore in fase di connessione col client 2");
    return NULL;
  }
  puts("Client connesso con successo");

  // check status connessione
  printf("Connessione accettata e id:%d\n", photo_sock);
  //write(desc, nome_file, 256);

  // apertura del file contenente il jpg.
  while (1) {
    FILE *fp = fopen(nome_file, "rb");
    if (fp == NULL) {
      perror("Errore apertura file");
      return NULL;
    }
    // prima leggiamo tracce di 256 byte
    unsigned char buff[20000] = {0};
    int nread = fread(buff, 1, 20000, fp);
    printf("Byte letti: %d \n", nread);

    //se la lettura è andata a buon fine, inviamo i dati
    int ret;
    if (nread > 0) {
      printf("Sto inviando la foto....\n");
	    while ( (ret = send(photo_sock, buff, nread, 0)) < 0) {
          if (errno == EINTR) continue;
          if(ret == -1) {
              fprintf(stderr,"Errno on send: %d, Error message: %s\n",errno,strerror(errno));
              return NULL;
          }
      }
    }

    if (nread < 20000) {
      if (feof(fp)) {
        printf("fine del file\n");
        printf("Bytes letti: %d\n",nread);
        printf("Bytes inviati: %d\n",ret);
        printf("Trasferimento del file completato con %d \n", photo_sock);
      }
      if (ferror(fp)) {
        printf("Errore di lettura\n");
        break;
      }
    }
    /*
    unsigned char buff[10240]={0};   
    printf("Sto chiudendo la conessione con id: %d\n",connfd);
    close(connfd);
    shutdown(connfd,SHUT_WR)*/
    sleep(rate);
  }
}

/*
// SERVO RAOUL
float calculateAngle(float angle,float currentPos){
	if (currentPos+angle>2*PI){
		return currentPos+angle-2*PI;
	}
	else if(currentPos+angle<0){
		return currentPos+angle+2*PI;
	}
	else{
		return angle;
	}
}

void moveCamera(float angle,float currentPos){
	//clockwise
	if (angle>0){
		bcm2835_pwm_set_data(CHANNEL,110);
		bcm2835_delayMicroseconds(800000.0*(angle/PI));
		bcm2835_pwm_set_data(CHANNEL,0);
		currentPos-=angle; //update position
	}
	//counterclockwise
	else if(angle<0){
		bcm2835_pwm_set_data(CHANNEL,30);
		bcm2835_delayMicroseconds(200000.0*(-angle/PI));
		bcm2835_pwm_set_data(CHANNEL,0);
		currentPos+=angle; //update position
	}
 }

void initGPIO(){
	if(bcm2835_init()){
		bcm2835_gpio_fsel(PIN, BCM2835_GPIO_FSEL_ALT5); 	
		bcm2835_pwm_set_clock(375);
    	bcm2835_pwm_set_mode(CHANNEL, 1, 1);
    	bcm2835_pwm_set_range(CHANNEL, RANGE);
		printf("Servo up and running!\n");}
	else{
		printf("Unable to initialize GPIO... Shutting down.\n");
		exit(EXIT_FAILURE);
	}
}

void closeGPIO(){
	if(bcm2835_close()){
		printf("All GPIO connections closed!\n");}
	else{
		printf("Unable to close GPIO \n");
		exit(EXIT_FAILURE);
	}
}
*/

// intercetta segnali di SIGINT in caso fermassi brutalmente il server
void kill_handler(int sign) {
  puts("Ricevuto segnale di chiusura, chiudo tutto!");
  if(photo_sock != 0) close(photo_sock);
  if(socket_desc != 0) close(socket_desc);
  exit(1);
}

int main(int argc, char *argv) {
  int ret;
  int client_sock, c, read_size;
  struct sockaddr_in server, client;
  // con questo buffer si interagisce con il client
  char client_msg[1024];
  // con questo buffer invio le foto al client
  char server_msg[10240];

  signal(SIGINT,kill_handler);
  signal(SIGPIPE,kill_handler);
  // creo la socket e vedo se la creazione va a buon fine
  socket_desc = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_desc == -1) {
    perror("Errore di connessione");
    return 1;
  }
  puts("Socket creata con successo");

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY; // vogliamo accettare connessioni da qualsiasi interfaccia
  server.sin_port = htons(PORT);       // comunicazione impostata su porta 3000

  // bind address socket
  if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0) {
    perror("Errore di connessione");
    return 1;
  }
  puts("Connessione su porta 3000 effettuata correttamente");

  // mi metto in ascolto del client
  listen(socket_desc, 3);
  puts("In attesa del client...");
  c = sizeof(struct sockaddr_in);

  client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c);
  if (client_sock < 0)
  {
    perror("Errore in fase di connessione col client 1");
    return 1;
  }
  puts("Client connesso con successo");

  // inizio a scattare una foto ogni 15 secondi alla risoluzione di default di 8mpx
  shooting_args_t *thread_shot_args = malloc(sizeof(shooting_args_t));
  thread_shot_args->shooting_rate = SHOOTRATE;
  thread_shot_args->w = DEFAULT_WIDTH;
  thread_shot_args->h = DEFAULT_HEIGHT;
  ret = pthread_create(&shooting_loop, NULL, startLoopCapturingTimer, (void *)thread_shot_args);
  if (ret != 0) {
    perror("Errore durante la creazione del thread di cattura della foto");
    return 1;
  }

  // inizio a mandare le foto al client ogni 20 secondi
  sending_args_t *sending_photo_args = malloc(sizeof(sending_args_t));
  sending_photo_args->sending_rate = SENDINGRATE;
  sending_photo_args->descr = socket_desc;
  ret = pthread_create(&sending_photo, NULL, sendPhotoToClient, (void *)sending_photo_args);
  if (ret != 0) {
    perror("Errore durante la creazione del thread di cattura della foto");
    return 1;
  }

  // variabili locali utili per il restart dei thread con arg aggiornati
  int width;
  int height;
  float deg;
  float current_position = 0;

  while (1) {
    // CAMBIO RISOLUZIONE CAMERA
    char msg_camera[] = "Ciao, sono il server! Ogni 15 secondi ti mando una foto a risoluzione di 8mpx. Vuoi modificare la risoluzione? (S/N)\n";
    write(client_sock, msg_camera, strlen(msg_camera));
    read_size = recv(client_sock, client_msg, 1024, 0);
    if (read_size < 0) {
      perror("Errore in ricezione dal client");
      return 1;
    }
    if (!strncmp(client_msg,"S",1)) {
      // fermo il loop di cattura per potergli dare i nuovi parametri di larghezza e altezza della foto
      flag = 0;
      printf("flag a 0");
      pthread_join(shooting_loop,NULL);
      char msg_width[] = "Indica larghezza(0-2592)";
      char msg_height[] = "Indica altezza(0-1954)";
      write(client_sock, msg_width, strlen(msg_width));
      read_size = recv(client_sock, client_msg, 1024, 0);
      if (read_size < 0) {
        perror("Errore in ricezione dal client");
        return 1;
      }
      width = *client_msg;
      write(client_sock, msg_height, strlen(msg_height));
      read_size = recv(client_sock, client_msg, 1024, 0);
      if (read_size < 0) {
        perror("Errore in ricezione dal client");
        return 1;
      }
      height = *client_msg;

      shooting_args_t *thread_shot_args = malloc(sizeof(shooting_args_t));
      thread_shot_args->shooting_rate = SHOOTRATE;
      thread_shot_args->w = width;
      thread_shot_args->h = height;
      ret = pthread_create(&shooting_loop, NULL, startLoopCapturingTimer, (void *)thread_shot_args);
      if (ret != 0) {
        perror("Errore durante la creazione del thread di cattura della foto");
        return 1;
      }
    }

    // MOVIMENTO SERVO
    char msg_servo[] = "Vuoi spostare il servo per orientare la fotocamera? (S/N)\n";
    write(client_sock, msg_servo, strlen(msg_servo));
    read_size = recv(client_sock, client_msg, 1024, 0);
    if (read_size < 0) {
      perror("Errore in ricezione dal client");
      return 1;
    }
    if (!strncmp(client_msg,"S",1)) {
      ret=pthread_cancel(shooting_loop);
      puts("quauudiwiqi");
      char msg_degree[] = "Indica di quanti gradi vuoi spostarti (0-359)\n";
      write(client_sock, msg_degree, strlen(msg_degree));
      read_size = recv(client_sock, client_msg, 1024, 0);
      if (read_size < 0) {
        perror("Errore in ricezione dal client");
        return 1;
      }
      deg = atof(client_msg);
      //moveCamera(deg,current_position);
      current_position += deg;
      if (current_position > 360)
        current_position = current_position - 360;
      /*ret=pthread_detach(servo_thread);
      if(ret!=0){
        perror("Errore in fase di detach del thread servo");
        return 1;
      }
      */
      shooting_args_t *thread_shot_args = malloc(sizeof(shooting_args_t));
      thread_shot_args->shooting_rate = SHOOTRATE;
      thread_shot_args->w = width;
      thread_shot_args->h = height;
      ret = pthread_create(&shooting_loop, NULL, startLoopCapturingTimer, (void *)thread_shot_args);
      if (ret != 0) {
        perror("Errore durante la creazione del thread di cattura della foto");
        return 1;
      }
    }
    char msg_closing[] = "Vuoi terminare la connessione con il server? (S/N)\n";
    write(client_sock, msg_closing, strlen(msg_closing));
    read_size = recv(client_sock, client_msg, 1024, 0);
    if (read_size < 0) {
      perror("Errore in ricezione dal client");
      return 1;
    }
    if (!strncmp(client_msg,"S",1)) break;
  }

  puts("Connessione con il client terminata!");
  close(socket_desc);
  return 0;
}