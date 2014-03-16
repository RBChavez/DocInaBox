/*-------------------------------------------------------Library-------------------------------------------------------------------*/
#include<MeetAndroid.h>  // Communicate with Android smart phone via Blutooth
#include <stdio.h>
#include <Wire.h>   
#include <LiquidCrystal_I2C.h>  // Connect with LCD
#include <DHT22.h>  // DHT22 tempurature and humidity
#include <SoftwareSerial.h>  // software serial for colorPal
/*---------------------------------------------------------------------------------------------------------------------------------*/
// DHT22
#define DHT22_PIN 3  // Assign digital pin for DHT22
DHT22 myDHT22(DHT22_PIN);  // Setup a DHT22 instance
// Heart Rate
const int HR_RX = 4; // Assign digital pin for Heart rate
const int HR_RX_Gen = 8;   // Assign digital pin for Heart Rate generate signal
int countLow = 0;  // Count low beat(0)
// ColorPal
const int sio = 2;            // ColorPAL connected to digital pin 
const int unused = 255;         // Non-existant pin # for SoftwareSerial
const int sioBaud = 4800;        // bound rate for colorPal
int red;  // Received RGB values from ColorPAL
int grn;
int blu;
int ph;
SoftwareSerial serin(sio, unused);  // Set up two software serials on the same pin.
SoftwareSerial serout(unused, sio);
// LCD
LiquidCrystal_I2C lcd(0x3F,20,4); // set the LCD address to 0x3F for a 20 chars and 4 line display
// Android
MeetAndroid meetAndroid;  
/*----------------------------------------------------------set up------------------------------------------------------------------*/
void setup(void)
{
//Serial port communicate
  Serial.begin(115200);   // start serial port
//Heart Rate
  pinMode (HR_RX, INPUT); // Set mode input for digital pin of Heart Rate
  pinMode (HR_RX_Gen, OUTPUT); // Set mode output for digital pin of Heart rate generate signal
//ColorPal  
  reset();                  // Send reset to ColorPal
  serout.begin(sioBaud);
  pinMode(sio, OUTPUT);
  serout.print("= (00 $ m) !"); // Loop print values, see ColorPAL documentation
  serout.end();              // Discontinue serial port for transmitting
  serin.begin(sioBaud);            // Set up serial port for receiving
  pinMode(sio, INPUT);
//LCD 
  lcd.init();  // Initial LCD
  lcd.backlight();
  lcd.setCursor(0, 1);
  lcd.print("BMP "); //(5 ,1)
  lcd.setCursor(11, 1);
  lcd.print("DEG "); //(15, 1)
  lcd.setCursor(0, 2);
  lcd.print("SpO2 ");
  lcd.setCursor(11, 2);
  lcd.print("PH  ");
  lcd.setCursor(0, 3);
  lcd.print("mmHg ");
  //lcd.setCursor(9, 3);
  //lcd.print("DIA ");
}
/*---------------------------------------------------------loop----------------------------------------------------------------------*/
void loop(void)
{ delay(30);
      char Val[100]; // print on LCD
//DHT22
  DHT22_ERROR_t errorCode; 
  errorCode = myDHT22.readData();

      int temp;
      temp = ((myDHT22.getTemperatureCInt()/10)*1.8) + 32;
      int Humid1 = myDHT22.getHumidityInt()/10;
      int Humid2 = myDHT22.getHumidityInt()%10;
      sprintf(Val, "ROOM %iF HUM %i.%01i%%RH",
                   temp,
                   Humid1, Humid2);

      lcd.setCursor(0, 0);
      lcd.print(Val);
      lcd.setCursor(0, 2);
      lcd.print("SpO2 ");

//ColorPal
     readData();
     sprintf(Val,"%2d", ph);
     lcd.setCursor(16, 2);
     lcd.print(Val);
//LM34 tempurature
      int Tempurature = analogRead(A2)-77;    //The sensor may be used in temperatures ranging from +32 to +212 °F. The sensor is accurate to 1.0 °F at +77 °F. 
      sprintf(Val, "%3dF", Tempurature);
      lcd.setCursor(15, 1);
      lcd.print(Val);
//Heart Rate    
      // Generate signal for heart rate
      if(countLow == 6)
      {
        countLow = 1;
        digitalWrite(HR_RX_Gen, HIGH);
      }
      else
      {
        countLow++;
        digitalWrite(HR_RX_Gen, LOW);
      }
      
      int HeartRate   = digitalRead(HR_RX); 
      sprintf(Val,"%3d |", HeartRate);
      lcd.setCursor(5, 1);
      lcd.print(Val);
//Spo2      
      int Spo2        = analogRead(A3);
      sprintf(Val, "%3d |", Spo2);
      lcd.setCursor(5, 2);
      lcd.print(Val);
//MEAS     
      int MEAS        = analogRead(A1);
      /* 70 = 190 BMP
         1 = 190/70 =  2.714
      */
      float ValBMP = MEAS*2.174;
      MEAS = ValBMP;
//Pressure
      int Pressure    =  analogRead(A0);
      /*  MRT meter between 0-300 mmHg, MPX5010AP pressure sensor meter between 0-1024(5 v.) mb
          1 mmb = 0.750061683 mmhg
          1024 = 768.0630387739075 mmhg
      */
      float ValPressure = Pressure*0.750061683;  
      Pressure = ValPressure - 26;  // Threadhold -26
      sprintf(Val,"%3d", Pressure);
      lcd.setCursor(5, 3);
      lcd.print(Val);
//add string
      /* 1 = Room Tempuratur
         2 = Room Hudity
         3 = Heart Rate
         4 = Spo2
         5 = SYS
         6 = DIA
         7 = DEG tempurature
         8 = PH
         9 = MEAS
         10 = Pressure*/
      char buf[200];
      sprintf(buf,"1:%i,2:%i.%01i,3:%i,4:%i,7:%i,8:%i,9:%i,10:%i",
                   temp,
                   myDHT22.getHumidityInt()/10, myDHT22.getHumidityInt()%10,
                   HeartRate,
                   Spo2,
                   Tempurature,
                   ph,
                   MEAS,
                   Pressure);  
                   
     meetAndroid.send(buf);      //sent string to Android
    // delay(200);   
}
//ColorPal reset
void reset() {
  //delay(200);
  pinMode(sio, OUTPUT);
  digitalWrite(sio, LOW);
  pinMode(sio, INPUT);
  while (digitalRead(sio) != HIGH);
  pinMode(sio, OUTPUT);
  digitalWrite(sio, LOW);
  //delay(80);
  pinMode(sio, INPUT);
  //delay(200);
}
//ColorPal read data
void readData() {
  char buffer[32];
 
  if (serin.available() > 0) {
    // Wait for a $ character, then read three 3 digit hex numbers
    buffer[0] = serin.read();
    if (buffer[0] == '$') {
      for(int i = 0; i < 9; i++) {
        while (serin.available() == 0);     // Wait for next input character
        buffer[i] = serin.read();
        if (buffer[i] == '$')               // Return early if $ character encountered
          return;
      }
      parseAndPrint(buffer);
      //delay(10);
    }
  }
}
// Parse the hex data into integers
void parseAndPrint(char * data) {
  sscanf (data, "%3x%3x%3x", &red, &grn, &blu);
  //ph 1,8,9,10, and 11 cannot be determined becasue sample could not be obtain. 
   if(red>250 && red<260 && grn>80 && grn<90 && blu>100 && blu<120)
   {
    ph = 2;
   }
   else if(red>240 && red<260 && grn>90 && grn<100 && blu>120 && blu<130)
   {
     ph = 3;
   }
   else if(red>250 && red<270 && grn>100 && grn<120 && blu>100 && blu<110)
   {
     ph = 4;
   }
   else if(red>140 && red<160 && grn>100 && grn<110 && blu>120 && blu<135)
   {
     ph = 5;
   }
   else if(red>90 && red<100 && grn>90 && grn<110 && blu>120 && blu<135)
   {
     ph = 6;     
   }
   else if(red>40 && red<60 && grn>55 && grn<70 && blu>60 && blu<80)
   {
     ph = 7;
   }
   else
   {
     ph = 0;
   }
}
