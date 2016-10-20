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

const int BELOW_TRIGGER_OUTPUT = 1; 
const int BELOW_TRIGGER_OUTPUT_2 = 4; 
const int CALIBRATION_INPUT = 0; 
const int VOLTAGE_ADC = 1;
const int RESPONSE_OUTPUT = 3;


#define EEPROM_VALID 124

int trigger=440;
byte val=0;
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
  pinMode(BELOW_TRIGGER_OUTPUT_2, OUTPUT); 
  pinMode(CALIBRATION_INPUT, INPUT); 
  pinMode(RESPONSE_OUTPUT, OUTPUT); 
  digitalWrite( BELOW_TRIGGER_OUTPUT, LOW);
  digitalWrite( BELOW_TRIGGER_OUTPUT_2, LOW);  
  val = EEPROM.read(0);
  if( EEPROM.read(1) == EEPROM_VALID){
    trigger=EEPROM.read(2)*4;
    showResponse(2);
  }
  showResponse(1);
  
}

void loop() {
  
  int voltage = analogRead(VOLTAGE_ADC);
  int voltageTriggered = voltage <= trigger;
  digitalWrite( BELOW_TRIGGER_OUTPUT, voltageTriggered ? HIGH: LOW);
  digitalWrite( BELOW_TRIGGER_OUTPUT_2, voltageTriggered ? HIGH: LOW);

  if ( digitalRead( CALIBRATION_INPUT) == HIGH ){
    trigger = voltage-1;

    EEPROM.write(0,val++);
    if( val >=255)
      val=0;
    EEPROM.write(1, EEPROM_VALID);
    EEPROM.write(2, trigger/4);
    delay(500);
    //indicate that calibration is done by flashing response LED
    showResponse(4);
  }
  delay(1000);
  
}
