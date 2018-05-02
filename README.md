# Picam
```
          "                        
 mmmm   mmm     mmm    mmm   mmmmm 
 #" "#    #    #"  "  "   #  # # # 
 #   #    #    #      m"""#  # # # 
 ##m#"  mm#mm  "#mm"  "mm"#  # # # 
 # 
```
Picam allows you to remotely move a webcam and take pictures with it at a specified angle (0 to 180) and picture resolution .It uses [WiringPI](www.wiringpi.com) to control the servo motor via Raspberry Pi's GPIO pins.

## Setup 
### From The Raspberry pi 
Download the Picam folder : 
- from the terminal by running
	```git clone https://www.github.com/Mattens15/RaspPiSO```

Move to the working directory in the terminal and compile the files:
 ```cd /path/to/directory && make```
	

Run the program:

```./picam```
  
# Progetto Sistemi Operativi 2017/2018

## Obiettivo del progetto / Project Goal

Si propone l'implementazione di un sistema di videosorveglianza controllabile da remoto utilizzando un Raspberry pi, un modulo camera ed un motore servo.  

The group proposes the implementation of a video surveillance system based on a Raspberry pi, a camera module and a servo motor.


## Collaboratori/Collaborators

_Raoul Nuccetelli_ [@rsoul](https://www.github.com/rsoul)   
_Matteo Attenni_ [@Mattens15](https://www.github.com/Mattens15)   
_Mihai Alxandru Zapodeanu_ [@djzapo](https://www.github.com/djzapo)   

___
### To do
- Implementare controllo servo
- Implementare controllo webcam
- implementare connessione al Raspberry Pi tramite TCP/IP (sia client-side che server-side)
- implementare gestione connessioni
