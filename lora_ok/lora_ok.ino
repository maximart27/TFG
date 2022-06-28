#include <rn2xx3.h>
//#include <SoftwareSerial.h>

//SoftwareSerial mySerial(10, 11); // RX, TX

//create an instance of the rn2xx3 library,
//giving the software serial as port to use
rn2xx3 myLora(Serial2);

// the setup routine runs once when you press reset:
void setup()
{
  //output LED pin
  //pinMode(13, OUTPUT);
  //led_on();

  // Open serial communications and wait for port to open:
 // Serial.begin(57600); //serial port to computer
  Serial2.begin(9600,SERIAL_8N1,3,1); //serial port to radio RX , TX 
  Serial2.println("Startup"); 

  initialize_radio();

  //transmit a startup message
  myLora.tx("TTN Mapper on TTN Enschede node");

  //led_off();
  delay(2000);
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
    Serial2.println("Communication with RN2xx3 unsuccessful. Power cycle the board.");
    Serial2.println(hweui);
    delay(10000);
    hweui = myLora.hweui();
  }

  //print out the HWEUI so that we can register it via ttnctl
  Serial2.println("When using OTAA, register this DevEUI: ");
  Serial2.println(myLora.hweui());
  Serial2.println("RN2xx3 firmware version:");
  Serial2.println(myLora.sysver());

  //configure your keys and join the network
  Serial2.println("Trying to join TTN");
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
  const char *devEUI = "0004A3080025A396";
  const char *appEui = "0000000000000000";
  const char *appKey = "700F5CE7F4BF095824F4B454F34E7F85";

  join_result = myLora.initOTAA(appEui, appKey, devEUI);


  while(!join_result)
  {
    Serial2.println("Unable to join. Are your keys correct, and do you have TTN coverage?");
    delay(60000); //delay a minute before retry
    join_result = myLora.init();
  }
  Serial2.println("Successfully joined TTN");

} 

// the loop routine runs over and over again forever:
void loop()
{
  
    //led_on();
delay(20000);
    Serial2.println("TXing");
    Serial2.println(myLora.tx("datos lora")); //one byte, blocking function

    //led_off();
    
}

/*void led_on()
{
  digitalWrite(13, 1);
}

void led_off()
{
  digitalWrite(13, 0);
}*/
