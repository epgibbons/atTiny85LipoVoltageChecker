
// For ATtiny85 Turn on LED and Analog Read
//RESET | pin 1 pin 8| 5V

// 3/A3 | pin 2 pin 7| 2/A1

// 4/A2 | pin 3 pin 6| 1/PWM

// GND | pin 4 pin 5| 0/PWM


#include "SoftwareSerial.h"

const int Rx = 3; // this is physical pin 2

const int Tx = 4; // this is physical pin 3


const int LED = 1; // this is physical pin 6 for the LED


const int ANTENNA = 1; // this is physical pin 7, connect wire as antenna


SoftwareSerial mySerial(Rx, Tx);
float val;

void setup() {
  analogReference(INTERNAL1V1);
  // put your setup code here, to run once:
  pinMode(Rx, INPUT);
  pinMode(Tx, OUTPUT);
  pinMode(LED, OUTPUT); // tell Arduino LED is an output
  mySerial.begin(9600); // send serial data at 9600 bits/sec
}

void loop() {
  
  val = analogRead(ANTENNA); // read the ANTENNA
  if( val <=446){
    tone(LED, 1000,500);      
  }
  mySerial.println(val); // send the value to Serial Monitor, ^Cmd-M
  
  
  delay(500);
  
}
