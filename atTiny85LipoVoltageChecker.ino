/*
 * atTiny85LipoVoltageChecker
 * 
 * An atTiny85 sketch to report when a lipo 3.7v battery needs recharging.
 * 
 * Vcc is the provided by the battery to be checked.
 * 
 * An external resistor network provides a voltage point of about 0.5v which is about midway 
 * through the internal reference voltage, nominally 1.1v, of the atTiny85. This voltage is
 * wired to ADC1 ( physical pin 7).
 * 
 * The loop function: 
 *  1. reads the voltage at ADC1 using analogRead. analogRefence set to INTERNAL1V1 in setup function.
 *  
 *  2. digital output 1 (physical pin 6) set ON using digitalWrite if voltage is below a certain value, 
 *  or set OFF if not. This value is known as the TRIGGER. port 1 is set to OUTPUT using pinMode in setup function,
 *  
 *  3. reads digital input 0 (physical pin 5)  using digitalRead. port 0 is set to INPUT using pinMode 
 *  in setup function,
 *  
 *  4.if input 0 is HIGH then the current value of voltage read in step1 is taken as the new value of  
 *  TRIGGER used in step2 of subsequent executions of the loop function. 
 *  This step epresents a calibration routine which removes lack of precision due
 *  to variation in the resistor network and internal reference voltage. To remove the need to recalibrate 
 *  after every RESET the new TRIGGER is written into the EPROM of the chip. In the setup function the 
 *  EPROM value is read and if set then its value is used tp initialise TRIGGER, otherwise TRIGGER is set to a 
 *  nominal value of 450.
 *  
 *  Note that if I do not write to EPROM then the trigger is reset to nominal value in code below on RESET.
 *  
 */
#include <EEPROM.h>

//#include "SoftwareSerial.h"
//
//const int Rx = 3; // this is physical pin 2
//
//const int Tx = 4; // this is physical pin 3


const int BELOW_TRIGGER_OUTPUT = 1; 
const int CALIBRATION_INPUT = 0; 
const int VOLTAGE_ADC = 1;
const int RESPONSE_OUTPUT = 3;

#define EEPROM_VALID 1

//SoftwareSerial mySerial(Rx, Tx);
int trigger=440;

/*
 * Always end in high to indicate that the system is working - the supply voltage is ok.
 */
void showResponse( int times, int delayMS=200){
  for( int i=0; i< times; i++){
    digitalWrite( RESPONSE_OUTPUT,LOW);
    delay(delayMS);    
    digitalWrite( RESPONSE_OUTPUT,HIGH);
    delay(delayMS);
  }
}

void setup() {
  analogReference(INTERNAL1V1);
  pinMode(BELOW_TRIGGER_OUTPUT, OUTPUT); 
  pinMode(CALIBRATION_INPUT, INPUT); 
  pinMode(RESPONSE_OUTPUT, OUTPUT); 
//  pinMode(Rx, INPUT);
//  pinMode(Tx, OUTPUT);
//  mySerial.begin(9600); // send serial data at 9600 bits/sec
  int eepromCheckSet;
  EEPROM.get(0, eepromCheckSet);
  if( eepromCheckSet == EEPROM_VALID){
    //assume value is written to address 2
    EEPROM.get(sizeof(eepromCheckSet), trigger);
    showResponse(2);
  }
  showResponse(1);
  
}

void loop() {
  
  int voltage = analogRead(VOLTAGE_ADC);
  digitalWrite( BELOW_TRIGGER_OUTPUT, voltage <=trigger ? HIGH: LOW);
  if ( digitalRead( CALIBRATION_INPUT) == HIGH ){
    trigger = voltage;
    int eepromCheckSet=EEPROM_VALID;
    EEPROM.put(0, eepromCheckSet);
    EEPROM.put(sizeof(eepromCheckSet), trigger);
    //indicate that calibration is done by flashing LED
    showResponse(2);
  }
  delay(1000);
  
}
