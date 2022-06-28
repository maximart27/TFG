#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

//Crear el objeto lcd  dirección  0x3F y 16 columnas x 2 filas
LiquidCrystal_I2C lcd(0x27,16,2);  //

void setup() {
  // Inicializar el LCD
  lcd.init();
  Serial.begin(115200);
  Serial.println("esto es el setup");
  Serial.println("Linea nueva despues");
  
  //Encender la luz de fondo.
  lcd.backlight();
  
  // Escribimos el Mensaje en el LCD en una posición  central.
  lcd.setCursor(0,0);
  lcd.print("Maxi lcd");
  lcd.setCursor(0, 1);
  lcd.print("esp-32");
  delay(1000);
}

void loop() {
  //desplazamos una posición a la izquierda
  lcd.clear();delay (500);
  lcd.setCursor(0,0);
  lcd.print("Maxi lcd");
  Serial.println("entro en el loop");
  lcd.setCursor(1,1);
  lcd.print("Sensor Infr");
  delay (3000);
}
