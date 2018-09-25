# Picam

Picam allows you to remotely move a webcam and take pictures with it at a specified angle (0 to 360) and picture resolution. It uses [bcm2835.h](http://www.airspayce.com/mikem/bcm2835) to control the servo motor via Raspberry Pi's GPIO pins.

## Setup 
#### Setting up the servo motor
Connect the servo motor to your Raspberry Pi as follows:
1. Pin 2 with (+)
2. Pin 6 with (-)
3. Pin 12 with signal

![alt text](https://image.ibb.co/kr8CU9/schema.jpg "Pin Schematics")


### From The Raspberry Pi (Server side)
Download the Picam folder : 

```git clone https://www.github.com/Mattens15/RaspPiSO```

Move to the working directory in the terminal :
 
```cd /path/to/directory```

Install the necessary library files using the bash script provided in the repository (**run only if on Raspberry pi**):

```sh bcm.sh```

Compile the files by running:
1. If you wish to compile all the project: ```make```
2. If you with to compile only the server:```make server```
3. If you wish to compile only the client:```make client```

Run the server:

```./picam```

_Optionally you can choose to controll the server from within the Pi if you have compiled the whole project._
Open another Terminal window and make sure you are in the same directory as the executable

Run the client:

```./picamclient```
 

### From Client side
You can choose to controll the server from a different computer connected to the same network as the Raspberry Pi.

Download the Picam folder :

```git clone https://www.github.com/Mattens15/RaspPiSO```

Move to the working directory in the terminal :

 ```cd /path/to/directory```

Compile the files:

```make client```

Run the program:

```./picamclient```

  
# Progetto Sistemi Operativi 2017/2018

### Obiettivo del progetto / Project Goal

Si propone l'implementazione di un sistema di videosorveglianza controllabile da remoto utilizzando un Raspberry pi, un modulo camera ed un motore servo.  

The group proposes the implementation of a video surveillance system based on a Raspberry pi, a camera module and a servo motor.


### Collaboratori/Collaborators

_Raoul Nuccetelli_ [@rsoul](https://www.github.com/rsoul)   
_Matteo Attenni_ [@Mattens15](https://www.github.com/Mattens15) 