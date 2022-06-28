#include <ArduinoJson.h>

#include <rn2xx3.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <IRTemp.h>

/*
 * File:    readTemperature.ino
 * Author:  Andy Gelme (@geekscape) & Angus Gratton (angus at freetronics. com)
 * License: GPLv3
 *
 * For more information see www.freetronics.com/irtemp
 *
 */
//creamos objeto de la lcd 

//Crear el objeto lcd  dirección  0x27 y 16 columnas x 2 filas
LiquidCrystal_I2C lcd(0x27,16,2);  //

// definimos los pines para el sensor 
static const byte PIN_ACQUIRE = 27;//8;
static const byte PIN_CLOCK   = 26;//13;   
static const byte PIN_DATA    = 25;//7; // Choose any pins you like for these

static const TempUnit SCALE=CELSIUS;  // Options are CELSIUS, FAHRENHEIT

IRTemp irTemp(PIN_ACQUIRE, PIN_CLOCK, PIN_DATA);
//se crea objeto para la libreria de lora 
rn2xx3 myLora(Serial2);

void setup(void) {
  //output LED pin
  //pinMode(2, OUTPUT);
  //led_on();
  Serial2.begin(9600, SERIAL_8N1,3,1); //serial port to radio TX RX  
  //Serial.begin(57600);
  //Serial.println("Startup");
  //Serial.println("~~~~~~~~~~~~~~");
 
  // Inicializar el LCD
  lcd.init();
  lcd.clear();delay (500);
  //Encender la luz de fondo.
  lcd.backlight();
  
  // Escribimos el Mensaje en el LCD en una posición  central.
  lcd.setCursor(0,0);
  lcd.print("Medidor de Temp");
  lcd.setCursor(0, 1);
  lcd.print("esp-32");
  // Lora
   Serial2.println(" empieza el radio : ");
   initialize_radio();
   Serial2.println(" finaliza el radio");
   myLora.tx("TTN Mapper on TTN Enschede node");
   //led_off();
  delay(2000); // 1 s de espera 
  lcd.clear();
}

void initialize_radio()
{
  //reset rn2483
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);
  delay(500);
  digitalWrite(2, HIGH);

  delay(100); //wait for the RN2xx3's startup message
  Serial2.flush();

  //Autobaud the rn2483 module to 9600. The default would otherwise be 57600.
  myLora.autobaud();

  //check communication with radio
  String hweui = myLora.hweui();
  while(hweui.length() != 16)
  {
    //Serial.println("Communication with RN2xx3 unsuccessful. Power cycle the board.");
    //Serial.println(hweui);
    delay(10000);
    hweui = myLora.hweui();
  }

  //print out the HWEUI so that we can register it via ttnctl
  //Serial.println("When using OTAA, register this DevEUI: ");
  //Serial.println(myLora.hweui());
  //Serial.println("RN2xx3 firmware version:");
  //Serial.println(myLora.sysver());

  //configure your keys and join the network
  //Serial.println("Trying to join TTN");
  bool join_result = false;


  /*
   * ABP: initABP(String addr, String AppSKey, String NwkSKey);
   * Paste the example code from the TTN console here:
   */
  //const char *devAddr = "02017201";
  //const char *nwkSKey = "AE17E567AECC8787F749A62F5541D522";
  //const char *appSKey = "8D7FFEF938589D95AAD928C2E2E7E48F";

  //join_result = myLora.initABP(devAddr, appSKey, nwkSKey);

  /*
   * OTAA: initOTAA(String AppEUI, String AppKey);
   * If you are using OTAA, paste the example code from the TTN console here:
   */
   
  //Server AEInnova
  /* 
    const char *appDev = "0004A3080025A396"; // aplicación AEInnova
    const char *appEui = "0000000000000000";
    const char *appKey = "700F5CE7F4BF095824F4B454F34E7F85"; // aplicación AEInnova
  */
  // AppTagoio
  const char *appDev = "70B3D57ED0051E81"; //preuba maxi
  const char *appEui = "0000000000000000";
  const char *appKey = "700F5CE7F4BF095824F4B454F34E7F85";//prueba maxi
  
  Serial2.println("empieza el join");
  join_result = myLora.initOTAA(appEui, appKey,appDev);


  while(!join_result)
  {
    Serial2.println("Unable to join. Are your keys correct, and do you have TTN coverage?");
    delay(60000); //delay a minute before retry
    join_result = myLora.init();
  }
  
  Serial2.println("Successfully joined TTN");

}


void loop(void) {
  StaticJsonDocument<64> doc;
  Serial2.println(" empieza el loop ");
  //lectura de temperatura 
  float irTemperature = irTemp.getIRTemperature(SCALE);
  irTemperature=readtemp(irTemperature);
  //corrección con fluke 
  //irTemperature = irTemperature+0,64 ;
  printTemperature("IR", irTemperature);
  //int aux = irTemperature *100;
  //float aux2 = aux/100;
  float ambientTemperature = irTemp.getAmbientTemperature(SCALE);
  ambientTemperature=readtemp_amb(ambientTemperature);
  printTemperature("Ambient", ambientTemperature);
  //if(irTemperature==nan){
  // corrección de la temperatura aluminio:
    float eps=0.22; // para 125 -> 0.215 || 175 -> 0.089
    float Toc = pow( ( ( (pow(irTemperature,4)-pow(ambientTemperature,4))/eps) + pow(ambientTemperature,4) ), 0.25);
    Serial2.print("Temp_AL: ");
    Serial2.println(Toc);
  // }
  //float irTemperature=readtemp(irTemperature);
  lcd.clear();delay (500);
  lcd.setCursor(0, 0);
  lcd.print("T_Obj:");//+(float)irTemperature)+" C" );
  lcd.setCursor(6, 0);
  lcd.print(irTemperature);
  lcd.setCursor(12,0);
  lcd.print("C");

  lcd.setCursor(0, 1);
  lcd.print("T_Amb:");//+(float)irTemperature)+" C" );
  lcd.setCursor(6, 1);
  lcd.print(ambientTemperature);
  lcd.setCursor(12,1);
  lcd.print("C");
  //Serial2.println("temp : "+irTemperature )
  // Lora 
  //led_on();
  
  delay(20000); // envia tx cada 2min
  //Serial.println("TXing temperatura del objeto");
  doc["temperature"] =Toc;
  //doc["temperature"] =35.666;
  String json_string;
  serializeJson(doc,json_string);
  Serial2.println(myLora.tx(json_string)); //one byte, blocking function

  //led_off();  
  delay(2000);
}

void printTemperature(
  char  *type,
  float  temperature) {

  Serial2.print(type);
  Serial2.print(" temperature: ");

  if (isnan(temperature)) {
    Serial2.println("Failed");
    
  }
  else {
    Serial2.print(temperature);
    Serial2.println(SCALE == FAHRENHEIT  ?  " F"  :  " C");
  }
}

float readtemp (float &temperature){
    bool cond=false;
    while(cond!=true){
      if (isnan(temperature)) {
        Serial2.println("Failed");
        temperature = irTemp.getIRTemperature(SCALE);
        Serial2.print("temperature: ");
        Serial2.println(temperature);
      }else{
        float temp_anterior;
        if(temperature<0){
            temperature = irTemp.getIRTemperature(SCALE);
            Serial2.print("temperature: ");
            Serial2.println(temperature);
          } else {
            temp_anterior=temperature;
             for(int i=0;i<10;i++){
              temperature = irTemp.getIRTemperature(SCALE);
              if(temperature>temp_anterior){
                  temp_anterior =temperature;
                }
                Serial2.print("temp_pick: ");
                Serial2.println(temp_anterior);
              //delay(1000);
              }
              temperature=temp_anterior;
              cond=true;
            }
        }  
       delay(100);
    }
    return temperature;
  }

float readtemp_amb(float &temperature){
    bool cond=false;
    while(cond!=true){
      if (isnan(temperature)) {
        Serial2.println("Failed");
        temperature = irTemp.getAmbientTemperature(SCALE);
        Serial2.print("temperature_amb: ");
        Serial2.println(temperature);
      }else{
        if(temperature<0){
            temperature = irTemp.getAmbientTemperature(SCALE);
            Serial2.print("temperature_amb: ");
            Serial2.println(temperature);
          } else {
              cond=true;
            }
        }  
       delay(100);
    }
    return temperature;
  }
/*
void led_on()
{
  digitalWrite(2, 1);
}

void led_off()
{
  digitalWrite(2, 0);
}*/
