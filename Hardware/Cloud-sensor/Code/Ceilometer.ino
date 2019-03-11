// IDE ADRUINO V1.8.7
// BOARD ARDUINO MEGA 2560 (no enough memory with UNO board)
//______________________________ SERVO
#include <Servo.h>
#define UPDATE_TIME 10
#define MAX_POS 180
#define MIN_POS 0
int servoPin = 9;
int pulse = 1900;
char s="1.5";
Servo HS64WP;
int pos=0; 
int ServoPwr_pin = 8;
int ServoPwr_led = 22;
//______________________________ THERMOCOUPLE
#include <Adafruit_MAX31855.h>
Adafruit_MAX31855  *thermocouples[1];
float Tc_HSU_start, Tc_HSU_end;
volatile float Tc_Heater;
float Tc_MAX31855;
//______________________________ IR SENSORs
#include <Wire.h>
#include <Adafruit_MLX90614.h>
float TA90H,TO90H,TA10H,TO10H,TA90R,TO90R,RO90;
Adafruit_MLX90614 mlx_fov10_Head = Adafruit_MLX90614(0x1B); // Head FOV90
Adafruit_MLX90614 mlx_fov90_Head = Adafruit_MLX90614(0x5A); // Head FOV10
Adafruit_MLX90614 mlx_fov90_Ref = Adafruit_MLX90614(0x3A); // Ref FOV90
//______________________________ SD-CARD
#include <SPI.h>
#include <SD.h>
const int chipSelect = 53;
//______________________________ REAL TIME CLOCK DS1307
#define DS1307_I2C_ADDRESS 0x68
volatile int top, top60, top10, topD,Heat_cnt;
byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
int SQW_RTC_led = 2;
//______________________________ RF433 MHZ
//#include <RH_ASK.h> => can't be used simutaneously with another SPI component. => should be externalyzed
// try to use RCSwitch or VirtualWire
//______________________________ HEATER 12V-4W
int HeaterPwr_pin = 7;
int HeaterPwr_led = 23;
//______________________________ GLOBAL VARIABLEs
boolean HeaterPwr_cmd, Heater_break, Idle_mode; 
float voltage, MinTamb, Alt_ref_raw, Alt_head_raw, Alt_head_corr, Alt_dyn_raw, Alt_dyn_corr, KDG90, KDG10;
String File_record = "Celio_A1";
volatile int Idle_cnt, Break_cnt, Clean_cnt;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
volatile float TPL_HH = 55;         // (C) Thermal Protection Level High-High <> risk for PLA, absolute threshold to stop completely the main program.
volatile float TPL_H = 45;          // (C) Thermal Protection Level High <> permission to launch Start_up sequence in cas of several Power restart
volatile float PwrSupply_ref = 7.45;// (V) Power Supply Volatege Reference <> shoudl be control due to risk of deviation +/-
volatile float PwrSupply_db = 0.40; // (V) Dead band power supply for accessories <> below 7V the servo don't work correctly.
volatile int   HS64_offset = -12;   // (step) aligne the deflector with the MLX axle. 
volatile float TMNICE = 2.0;        // (C) thermal threshold to define the risk of ice and activate the Heater. Hysteresis is handle at +3C on pick-Up
volatile float KHTO = 120;          // (second) Heat TimeOut running => permit a safety break during heating. A n Breaker time is associated at the half
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {

pinMode(HeaterPwr_pin, OUTPUT);
pinMode(HeaterPwr_led, OUTPUT);
pinMode(ServoPwr_pin, OUTPUT);
pinMode(ServoPwr_led, OUTPUT);
pinMode(SQW_RTC_led,OUTPUT);
pinMode(3,INPUT_PULLUP);

thermocouples[0] = new Adafruit_MAX31855(4, 5, 6); 
attachInterrupt (digitalPinToInterrupt(3), Flash_RTC , RISING );

Serial.begin(9600);

mlx_fov10_Head.begin();  
mlx_fov90_Head.begin(); 
mlx_fov90_Ref.begin(); 

Wire.begin();                
Wire.beginTransmission(DS1307_I2C_ADDRESS); Wire.write(0x07); Wire.write(0x10);
Wire.endTransmission();

if (!SD.begin(chipSelect)) 
  {Serial.println("Card failed, or not present");return;}
else
  {Serial.println("card initialized.");
  File dataFile = SD.open(File_record, FILE_WRITE); 
    if (dataFile) 
    {              
      dataFile.print("Date")            ;dataFile.print(" ; ");             
      dataFile.print("Hour")            ;dataFile.print(" ; ");  
      dataFile.print("Tamb_FOV90_Head") ;dataFile.print(" ; ");   
      dataFile.print("Tamb_FOV10_Head") ;dataFile.print(" ; "); 
      dataFile.print("Tamb_FOV90_Ref")  ;dataFile.print(" ; ");
      dataFile.print("Tobj_FOV90_Head") ;dataFile.print(" ; ");
      dataFile.print("Tobj_FOV10_Head") ;dataFile.print(" ; ");
      dataFile.print("Tobj_FOV90_Ref")  ;dataFile.print(" ; ");   
      dataFile.print("Tc_Heater")       ;dataFile.print(" ; ");
      dataFile.print("Tc_MAX31855")     ;dataFile.print(" ; ");
      dataFile.print("Voltage")         ;dataFile.print(" ; ");  
      dataFile.print("Rate_FOV90")      ;dataFile.print(" ; ");  
      dataFile.print("Rate_FOV10")      ;dataFile.print(" ; ");  
      dataFile.print("Altitude_raw")    ;dataFile.print(" ; ");   
      dataFile.print("Altitude_corr")   ;dataFile.print(" ; "); 
      dataFile.print("Idle_mode")       ;dataFile.print(" ; ");
      dataFile.print("HeaterPwr_cmd")   ;dataFile.print(" ; ");      
      dataFile.print("Heater_break")    ;dataFile.print(" ; ");
      dataFile.print("Heat_count")      ;dataFile.print(" ; ");
      dataFile.println("File name")     ;dataFile.close(); 
    }}

StartUp();

HS64WP.attach(servoPin); 
for (pos = MIN_POS; pos <= MAX_POS+HS64_offset; pos += 1) { HS64WP.write(pos); } 
delay(UPDATE_TIME);Serial.println("Configuration Corrected Measurment");delay(1000);
top60 = 0;

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {

Tc_Heater = thermocouples[0]->readCelsius();
Thermal_Prot();

Tc_MAX31855 = thermocouples[0]->readInternal();
TA90H = mlx_fov90_Head.readAmbientTempC();
TA10H = mlx_fov10_Head.readAmbientTempC();
TA90R = mlx_fov90_Ref.readAmbientTempC();
voltage = analogRead(A0) * (5.0 / 1023.0) * (1680.0/680.0); // (R1 = 1 kOhm , R2 = 680 Ohms)
TO90H = mlx_fov90_Head.readObjectTempC();
TO10H = mlx_fov10_Head.readObjectTempC();
TO90R = mlx_fov90_Ref.readObjectTempC();

KDG90 = TO90H / TO90R;
KDG10 = TO10H / TO90R;

MinTamb = min(TA90H,TA10H); // Mini Tambe sensor and not outdoor
Alt_ref_raw = max(0,(( TA90R - TO90R) / 0.6) * 100);
Alt_head_raw = max(0,(( TA90H - TO90H) / 0.6) * 100);
Alt_head_corr = Alt_head_raw * KDG90;
Alt_dyn_raw = max(0,(( TA10H - TO10H) / 0.6) * 100);
Alt_dyn_corr = Alt_dyn_raw * KDG10;

//[1] HEATING MANAGMENT___________________________________
if (MinTamb <= TMNICE && HeaterPwr_cmd == false && Heater_break == false) {HeaterPwr_cmd = true; } 
else if (MinTamb >= TMNICE + 3 or Heat_cnt >= KHTO) {HeaterPwr_cmd = false; Heat_cnt = 0; Heater_break = true;}

//[2] CLEANING MANAGMENT__________________________________

if ( ((TA90H - TO90H) <= 2.0) && ((TA10H - TO10H) <= 2.0) && ((TA90R - TO90R) <= 2.0) && (Idle_mode == false) )
  {Monitor(); CleanUp(); Clean_cnt = Clean_cnt + 1;}
else
  {Clean_cnt = 0;}

if (Clean_cnt >= 3) 
  {Serial.println("IDLE");for (pos = MAX_POS + HS64_offset; pos <= MIN_POS; pos -= 1) { HS64WP.write(pos); delay(UPDATE_TIME);} 
  Idle_mode = true;
  Alt_ref_raw = 0;
  Alt_head_raw = 0; 
  Alt_head_corr = 0; 
  Alt_dyn_raw = 0;    
  Alt_dyn_corr = 0;   
  }


//[3] RECORDING___________________________________________
if (top60 >= 10) { Record(); Monitor(); top60 = 0;}


//[4] ACTUATORS CONTROL __________________________________
if (HeaterPwr_cmd == true) {digitalWrite(HeaterPwr_pin, HIGH); digitalWrite(HeaterPwr_led, HIGH); } else {digitalWrite(HeaterPwr_pin, LOW); digitalWrite(HeaterPwr_led, LOW); }

}


//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

void CleanUp()
{

Serial.println("Clean-Up");
for (pos = MIN_POS; pos <= MAX_POS+HS64_offset; pos += 1) { HS64WP.write(pos); delay(UPDATE_TIME);} 
delay(2000);
  
}

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

byte bcdToDec(byte val) 
{ return ( (val/16*10) + (val%16) );}

void getDateDs1307(byte *second,byte *minute,byte *hour,byte *dayOfWeek,byte *dayOfMonth,byte *month,byte *year) //---
{
  Wire.beginTransmission(DS1307_I2C_ADDRESS);
  Wire.write(0);
  Wire.endTransmission();
  Wire.requestFrom(DS1307_I2C_ADDRESS, 7);

  *second     = (bcdToDec(Wire.read() & 0x7f));
  *minute     = (bcdToDec(Wire.read()));
  *hour       = (bcdToDec(Wire.read() & 0x3f));  // Need to change this if 12 hour am/pm
  *dayOfWeek  = bcdToDec(Wire.read());
  *dayOfMonth = bcdToDec(Wire.read());
  *month      = bcdToDec(Wire.read());
  *year       = bcdToDec(Wire.read());
}

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

void Flash_RTC ()
{
  
  top = top + 1;
  top60 = top60 + 1;

  digitalWrite(SQW_RTC_led,HIGH);
  delay(500); 
  digitalWrite(SQW_RTC_led, LOW); 

if (HeaterPwr_cmd == true) {Heat_cnt = Heat_cnt + 1;}
  
  switch(Heat_cnt) 
  {
    case 30:  CleanUp(); break;
    case 60:  CleanUp(); break;
    case 90:  CleanUp(); break;
    case 120: CleanUp(); break;    
  }

if (Heater_break == true) {Break_cnt = Break_cnt + 1;}
if (Break_cnt >= KHTO / 2) {Heater_break = false; Break_cnt = 0;}

if (Idle_mode == true) {Idle_cnt = Idle_cnt + 1;}
if (Idle_cnt >= 600) {Idle_mode = false; Idle_cnt = 0;Clean_cnt = 0;} // cleanUp reset required to permit a new cycle of Cleaning and avoid looping 
}

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

void Record() {

  getDateDs1307(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
   
  File dataFile = SD.open(File_record, FILE_WRITE); 
     
    if (dataFile) 
    {              
      dataFile.print(dayOfMonth);dataFile.print("/");dataFile.print(month);dataFile.print("/");dataFile.print(year);dataFile.print(" ; ");             
      dataFile.print(hour);dataFile.print(":");dataFile.print(minute);dataFile.print(":");dataFile.print(second);dataFile.print(" ; ");
      dataFile.print(TA90H,2)         ;dataFile.print(" ; ");   
      dataFile.print(TA10H,2)         ;dataFile.print(" ; "); 
      dataFile.print(TA90R,2)         ;dataFile.print(" ; ");
      dataFile.print(TO90H,2)         ;dataFile.print(" ; ");
      dataFile.print(TO10H,2)         ;dataFile.print(" ; ");
      dataFile.print(TO90R,2)         ;dataFile.print(" ; ");   
      dataFile.print(Tc_Heater,1)     ;dataFile.print(" ; ");
      dataFile.print(Tc_MAX31855,1)   ;dataFile.print(" ; ");
      dataFile.print(voltage,2)       ;dataFile.print(" ; ");  
      dataFile.print(KDG90,3)         ;dataFile.print(" ; ");  
      dataFile.print(KDG10,3)         ;dataFile.print(" ; ");  
      dataFile.print(Alt_ref_raw,1)   ;dataFile.print(" ; ");   
      dataFile.print(Alt_head_raw,1)  ;dataFile.print(" ; "); 
      dataFile.print(Alt_head_corr,1) ;dataFile.print(" ; "); 
      dataFile.print(Alt_dyn_raw,1)   ;dataFile.print(" ; "); 
      dataFile.print(Alt_dyn_corr,1)  ;dataFile.print(" ; "); 
      dataFile.print(Idle_mode)       ;dataFile.print(" ; ");
      dataFile.print(HeaterPwr_cmd)   ;dataFile.print(" ; ");      
      dataFile.print(Heater_break)    ;dataFile.print(" ; ");
      dataFile.print(Heat_cnt)        ;dataFile.print(" ; ");
      dataFile.println(File_record )  ;dataFile.close(); 
    }
  
}

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


void Monitor() {

Serial.print(TA90H);Serial.print("C : ");
Serial.print(TA10H);Serial.print("C : ");
Serial.print(TA90R);Serial.print("C : >> ");
Serial.print(Tc_Heater);Serial.print("C << : ");
Serial.print(Tc_MAX31855);Serial.print("C : ");
Serial.print(top);Serial.print("- : ");
Serial.print(HeaterPwr_cmd);Serial.print(" : ");
Serial.print(voltage);Serial.print("V : ");
Serial.print(TO90H);Serial.print("C : ");
Serial.print(TO10H);Serial.print("C : ");
Serial.print(TO90R);Serial.print("C : ");
Serial.print(Alt_ref_raw);Serial.print("m : ");
Serial.print(Alt_head_raw);Serial.print("m : ");
Serial.print(Alt_head_corr);Serial.print("m : ");
Serial.print(Alt_dyn_raw);Serial.print("m : ");
Serial.print(Alt_dyn_corr);Serial.print("m : ");
Serial.print(KDG90);Serial.print("- : ");
Serial.print(KDG10);Serial.print("- : ");
Serial.print(top60);Serial.println("- : ");
  
}

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

int laps, SUHT_STOP;
float deltat, volt_max, volt_min;

void StartUp() {

Tc_Heater = thermocouples[0]->readCelsius();

if (Tc_Heater <= TPL_H) {

digitalWrite(HeaterPwr_pin, LOW);digitalWrite(HeaterPwr_led, LOW);
digitalWrite(ServoPwr_pin, LOW);digitalWrite(HeaterPwr_led, LOW);

Serial.println(">>>>>>>>>START-UP<<<<<<<<<<");

Serial.println("(1) HEATER TEST ______________");

Serial.print("Temp Tc Heater:");
Tc_HSU_start = thermocouples[0]->readCelsius();
volt_max = Tc_HSU_start;
volt_min = Tc_HSU_start;
Serial.print(Tc_HSU_start);Serial.println(" C");
delay(1000);
laps = 0;
Serial.println("-->HEATER ON");
  do {digitalWrite(HeaterPwr_pin, HIGH);digitalWrite(HeaterPwr_led, HIGH);
      laps = laps + 1 ;
      deltat=thermocouples[0]->readCelsius()-Tc_HSU_start;
      voltage = analogRead(A0) * (5.0 / 1023.0) * (1680.0/680.0);
      if (voltage > volt_max) {volt_max = voltage;} else {volt_max = volt_max;}
      if (voltage > volt_min) {volt_min = voltage;} else {volt_min = volt_min;}
      Serial.print(laps);Serial.print(" : ");
      Serial.print(voltage);Serial.print(" : ");
      Serial.print(deltat);Serial.println(" : ");
      if (volt_max - volt_min > (2 * PwrSupply_db)) {Serial.println("TRIP BOARD - POWER SUPPLY ISSUE"); delay(250); exit(0);}
      if (voltage < (PwrSupply_ref-PwrSupply_db)) {Serial.println("TRIP BOARD - POWER SUPPLY TOO LOW"); delay(250); exit(0);}
      if (laps == 40 && deltat <= 5.0) {Serial.println("TRIP BOARD - NO HEATING"); delay(250); exit(0);}
      if (thermocouples[0]->readCelsius() > TPL_HH) {Serial.println("TRIP BOARD - Too HOT"); delay(250); exit(0);}
      if (laps >=40 || deltat >= 5.0) {SUHT_STOP = 1;} else {SUHT_STOP = 0;}
      delay(1000); } while ( SUHT_STOP == 0); 

digitalWrite(HeaterPwr_pin, LOW); digitalWrite(HeaterPwr_led, LOW);Serial.println("-->HEATER OFF");} else {Serial.println("TRIP BOARD - Too HOT"); delay(250); exit(0);}

Serial.println("(2) SERVO TEST ______________");
HS64WP.attach(servoPin); 
digitalWrite(ServoPwr_pin, HIGH); digitalWrite(ServoPwr_led, HIGH); Serial.println("SERVO ON");
delay(1000);


for (pos = MIN_POS; pos <= MAX_POS+HS64_offset; pos += 1) { HS64WP.write(pos); delay(UPDATE_TIME);
      if (voltage > volt_max) {volt_max = voltage;} else {volt_max = volt_max;}
      if (voltage > volt_min) {volt_min = voltage;} else {volt_min = volt_min;}} 
      Serial.println("Configuration Corrected Measurment");delay(2000);
for (pos = MAX_POS+HS64_offset; pos >= 85+HS64_offset; pos -= 1) { HS64WP.write(pos); delay(UPDATE_TIME);
      if (voltage > volt_max) {volt_max = voltage;} else {volt_max = volt_max;}
      if (voltage > volt_min) {volt_min = voltage;} else {volt_min = volt_min;}} 
      Serial.println("Configuration Head Measurment only");delay(2000);
for (pos = 90+HS64_offset; pos >= MIN_POS; pos -= 1) { HS64WP.write(pos); delay(UPDATE_TIME);
      if (voltage > volt_max) {volt_max = voltage;} else {volt_max = volt_max;}
      if (voltage > volt_min) {volt_min = voltage;} else {volt_min = volt_min;}} 
      Serial.println("Safe location - Measurment OFF");delay(2000);


Serial.println("END START-UP ______________");

}

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

void Thermal_Prot() {if (Tc_Heater > TPL_HH) {Serial.println("TRIP BOARD - OVERHEAT");delay(250);exit(0);}}
