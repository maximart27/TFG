#include <esp_task_wdt.h>
#include <IRTemp.h>
#include <TN901.h>


/*
 * File:    readTemperature.ino
 * Author:  Andy Gelme (@geekscape) & Angus Gratton (angus at freetronics. com)
 * License: GPLv3
 *
 * For more information see www.freetronics.com/irtemp
 *
 */

//#include "IRTemp.h"

static const byte PIN_ACQUIRE = 27;//16;//27;//8;
static const byte PIN_CLOCK   = 26;//27;//26;//13;   
static const byte PIN_DATA    = 25;//14;//25;//7; // Choose any pins you like for these

static const TempUnit SCALE=CELSIUS;  // Options are CELSIUS, FAHRENHEIT

IRTemp irTemp(PIN_ACQUIRE, PIN_CLOCK, PIN_DATA);

void setup(void) {
  Serial.begin(9600);
  Serial.println("IRTemp example");
  //Serial.println("~~~~~~~~~~~~~~");
}

void loop(void) {
  float irTemperature = irTemp.getIRTemperature(SCALE);
  irTemperature = irTemperature-1+0,64 ;
  //Serial.print(" temperature: ");
  //Serial.print(irTemperature);
  
  //Serial.println(" C");
  printTemperature("IR", irTemperature );

 
  
 float ambientTemperature = irTemp.getAmbientTemperature(SCALE);
  
  //Serial.print(" temperature ambiente: ");
 // Serial.print(ambientTemperature);
  //Serial.println(" C");
  printTemperature("Ambient", ambientTemperature);

 float eps=0.28; 
 float Toc = pow( ( ( (pow(irTemperature,4)-pow(ambientTemperature,4))/eps) + pow(ambientTemperature,4) ), 0.25);
 Serial.print("t_Objeto_ral: ");
 Serial.println(Toc);

  delay(1000);
}

void printTemperature(
  char  *type,
  float  temperature) {

  Serial.print(type);
  Serial.print(" temperature: ");

  if (isnan(temperature)) {
    Serial.println("Failed");
  }
  else {
    Serial.print(temperature);
    Serial.println(SCALE == FAHRENHEIT  ?  " F"  :  " C");
  }
}
