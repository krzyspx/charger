/*******************************************************************
 nowe dzielniki, button CI/CU

  V1 - vLED OK -
  v9 wys duzy skalowane wartosci
  V10 pwmU główne
  v15 - programy prostownika od 1 do ..... 0 i niezdefiniowane - test po uruchomieniu prostownika
  v20 - wyswietlacz duzy 1 - pomiary wewnetrzne
  v21 wysietlacz duży 2 - message
  V29 - Terminal

  V34 -

********************************************************************/

#include "klaw.h"
#include "transmit.h"
#include "dallas.h"
#include "pompa.h"
#include "prostownik.h"

#include <OneWire.h>
#include <DallasTemperature.h>

#include <Timers.h> //  my favorite timer 
Timers <6> akcja; //

#define led_green A3  // przyłączenia do fizycznych portów NANO
#define led_red 13
#define resetesp  12
#define czasresetesp 60


//zmienne globalne

int sendnotifi = 0; //wyślij noti
int sendemail = 0;  // wyślij email


//>>>>>>>>>>>>>>>>>>> my NANO Programs

int ESPonline = czasresetesp; //licznik watchdog dla esp

void Program_vPin0(String strvPin) {
  digitalWrite(led_green, intfromstr(strvPin));  //miganie led green przez ESP
  ESPonline = czasresetesp;
  if (flagasetup1 == 0) {
    flagasetup1 = 1;
    BlynkvirtualWrite_order(1, "a");
  }
}

void Program_vPin10(String strvPin) {
  pwmU = intfromstr(strvPin);
  analogWrite(3, pwmU);
}
void Program_vPin11(String strvPin) {
  setUmax = intfromstr(strvPin);
}
void Program_vPin12(String strvPin) {
  programU = intfromstr(strvPin);
}
void Program_vPin13(String strvPin) {
  //  plot = intfromstr(strvPin);
}
void Program_vPin14(String strvPin) {
  robc = intfromstr(strvPin);
}
void Program_vPin15(String strvPin) {
  programprost = intfromstr(strvPin);
}
void Program_vPin16(String strvPin) {
  //  timerpompa(strvPin);
}
void Program_vPin29(String strvPin) {}

void Program_vPin34(String strvPin) {
  if (intfromstr(strvPin)) termesp = 1; else termesp = 0;
}

int intfromstr(String str) {
  String    strdata3 = str.substring(str.indexOf(':') + 1);  // if nr vPin typ int
  return strdata3.toInt();
}

//ssssssssssssssssssssssssssssssssssssssssssss
void setup()
{
  pinMode(led_red, OUTPUT); //test
  pinMode(led_green, OUTPUT); //test
  digitalWrite(led_green, HIGH);
  pinMode(wyRobc, OUTPUT); //robc

  analogWrite(sterU, 0);
  analogWrite(reg5, 0);

  Serial.begin(115200);
  delay(100);
  Serial.println(F(__FILE__));  //BLYNK .3.10 Arduino IDE.cc 1.6.12

  /*
  setupstart();
  delay(500);
  setupstart1();
  delay(500);
  setupstart2();
  delay(500);
  setupstart3();
  delay(500);
  setupstart4();
  delay(500);
  */
  akcja.attach(0, 1000, timer1sek); //  5 sek
  akcja.attach(1, 15000, programy_prost); // pętla dłuższa 10 s licznik grzania
  akcja.attach(2, 20000, notemail); //by nie wysyłać za częto emaili i noti
  akcja.attach(3, 5000, timerled); //

  setdallas(); //setup for ds18b20

  flagasetup1 = 0; //flaga do jednokrotnego wysłania aktualnych stanów vPinów z APP
  setprostownik();
}

void loop()
{
  akcja.process(); // timer
  recivestrfromserial(); //dekodowanie danych z serial
}

void notemail() {// wysyłanie emaili i noti
  if (sendnotifi) {
    sendnotifi = 0;
    BlynkvirtualWrite_order(2, "noti od BLYNKa");
  }
  if (sendemail) {
    sendemail = 0;
    BlynkvirtualWrite_order(3, "Email od BLYNKa");
  }
}

void timer1sek() { //różne rzeczy wykonywane cyklicznie
  watchdogESP();  //watchdog for ESP
  sensorT();    //dallas
  blinkLedred(); // blink led on NANO

  analogread();
  analogprint();
  usrednianie();
  onrobc();
}


void timerled() {// osobny timer dla leda green

  blinkvLed2(); //blink vLed from NANO
}

void watchdogESP () { //watchdog dla esp
  ESPonline--;
  //   Serial.println("reset esp za " + String(ESPonline));
  if (ESPonline == 0) { //reset esp
    digitalWrite(resetesp, LOW);
    Serial.println(F("Reset ESP"));
  }
  if (ESPonline == -1) {
    Serial.println(F("END Reset ESP"));
    ESPonline = czasresetesp;
  }
}

bool flagaled = 0; //blink NANO led NANO
void blinkLedred() {
  flagaled = !flagaled;
  digitalWrite(led_red, flagaled);
}
bool flagaled2 = 0;
void blinkvLed2() {
  flagaled2 = !flagaled2;
  if (flagaled2)  {
    BlynkvirtualWrite_led(1, 255); //blink NANO in APP
  }
  else   {
    BlynkvirtualWrite_led(1, 0);
  }
}


