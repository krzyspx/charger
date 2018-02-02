#include <Arduino.h>

#define sterU 3 //wyj PWM sterowanie U
#define wyRobc 4 //wyj na mofsfet Robc 0/1
#define reg5   5 // wyj PWM port D5

#define Unull 20 // napiecie braku akku 
#define Umin 105 // minimalne napięcie akku 
#define Unom 126 //  napięcie nominalne akku 
#define Umax 148 // max napięcie akku bez korekcji temperatury

void BlynkvirtualWrite_str (byte vPin1, String str1);
void BlynkvirtualWrite_int (byte vPin2, int z);
void regU();
void regI();

int a0 = 0; //pomiary z wejść analog
int a1 = 0;
int a2 = 0;
int pwmU = 0; // pwm główne
int setUmax = 126; //ustawiane napiecie ładowania
int robc = 0; // syerowanie R obciążenia 0 - wył
int u10 = 0; //przeskalowane napięcie z wejścia u10 x 10 teraz Uwe
int u0 = 0; // przeskalowane napięcie x 10 z wejścia u0
int ip = 0; // przeskalowany prąd prostownika x10
int czaslad = 0;
int czasprzerwa = 0;
int liczczas = 0;

int programprost = 0; //rodzaj programu sterownika wybierane z menu

void setpwmU(int pwm) // ustawianie pwm główne
{
  pwmU = pwm;
  analogWrite(sterU, pwmU);
  BlynkvirtualWrite_int(10, pwmU);
}
void setobc1(int pwm) //tymczasowa procedura pwm dla obciążenia
{
  if (pwm > 0) {
    robc = 1;
  } else {
    robc = 0;
  }
}

void setprostownik() //procedury początkowe prostownika w pętli setup
{
  setpwmU(0); //pwm off
}

void analogread()
{
  a0 = analogRead(A0);
  a1 = analogRead(A1);
  a2 = analogRead(A2);
}

void analogprint() //skalowanie i wydruk napięć
{
  u10 = (a0 / 20); //kalibracja
  int ac = (u10 / 10);
  int ad = (u10 % 10);
  //Serial.println("a0 "+ String(a0)+ "  a1 "+ String(a1)+ "  a2 "+ String(a2));
  String str = "" + String(ac) + "." + String(ad) + " "; // oszczedzam 3 kB pamieci
  //BlynkvirtualWrite_str(7, str);
  //   Serial.println(str);

  long a1temp = a1 * 10;
  a1temp = a1temp * 100;
  u0 = a1temp / 489; //kalibracja
  ac = (u0 / 100);
  ad = (u0 % 100);
  str = str + "" + String(ac) + "." + String(ad) + "V "; // oszczedzam 3 kB pamieci
  //BlynkvirtualWrite_str(8, str);
  //     Serial.println(str);

  ip = abs(a2 - 515) / 2;
  ac = (ip / 10);
  if (a2 > 515) {
    ac = -ac;
  }
  ad = (ip % 10);
  str = str + "" + String(ac) + "." + String(ad) + "A " + String(pwmU) + "."+  String(setUmax); // oszczedzam 3 kB pamieci
  //    String str = ("s=" + String(programprost) + " I=" + String(pwmU) + " wy=" + String(u0sred) + " U=" + String(setUmax));
  BlynkvirtualWrite_str(9, str);
  //    Serial.println(str);
  //  str = "a0="+ (String(a0) + "   a1=" + String(a1) + "   a2=" + String(a2));
  //   BlynkvirtualWrite_str(20, str);

}

int u10sred = 0;
int u0sred = 0;
int ipsred = 0;
int u10sredch = 0;
int u0sredch = 0;
int ipsredch = 0;
byte liczsred = 0;
int programU = 0; //flaga typu programu ładowania 0 CI 1- CU


void usrednianie()
{
  liczsred++;
  if (u0 < 1700) {
    u0sredch = u0sredch + u0; //omijanie skoków napięcia
  } else {
    u0sredch = u0sredch + u0sred;
  }
  ipsredch = ipsredch + ip;
  if (liczsred == 10)
  {
    u0sred = u0sredch / liczsred;
    ipsred = ipsredch / liczsred;
    u0sredch = 0;
    ipsredch = 0;
    liczsred = 0;
    String str = ("s=" + String(programprost) + " I=" + String(pwmU) + " wy=" + String(u0sred) + " U=" + String(setUmax));
    //   Serial.println(str);
    BlynkvirtualWrite_str(20, str);
  }
}

void regI()
{
  if ((u0sred/10) > setUmax) {
    pwmU = pwmU - 1;
    if (pwmU < 0) {
      pwmU = 0;
    }
    setpwmU(pwmU);
    u10sred = 10*(setUmax);
    liczsred = 0;
  }
}

void regU()
{
  if ((u0sred/10) > setUmax) {
    pwmU = pwmU - 2;
    if (pwmU < 0) {
      pwmU = 0;
    }
  }
  if ((u0sred/10) < (setUmax - 2)) {
    pwmU = pwmU + 1;
    if (pwmU > 255) {
      pwmU = 255;
    }
  }
  setpwmU(pwmU);
  u10sred = 10*(setUmax);
  liczsred = 0;
}

void onrobc()
{
  if (robc == 0) {
    digitalWrite(wyRobc, LOW);
    analogWrite(reg5, 0);
  } else {
    digitalWrite(wyRobc, HIGH);
    analogWrite(reg5, 255);
  }

}
void message_txt(String str)
{
  BlynkvirtualWrite_str(21, str);
}


void message(int mes)
{
  switch (mes)
  {
    case 1: {
        String str = ("s=" + String(programprost) + " l=" + String(czaslad) + "   p=" + String(czasprzerwa) + "   x" + String(liczczas));
        //   Serial.println(str);
        BlynkvirtualWrite_str(20, str);
        String i = "Wynik";
        message_txt(i);

      } break;
    default: { // 0 i inne wartości - test
      }
  }
}


byte flaga_test = 0;
void test()
{
  if (pwmU == 0) {
    message(1);
  } else   {
    setpwmU(0);
    String i = "Czekam na test";
    message_txt(i);
  }
}
byte faza_odsiarczania= 0; //0-start
void odsiarczanie()
{
  if ((u0sred/10) > setUmax) {
    setpwmU(0);
    setobc1(255);
    String i = "Odsiarczanie rozładowanie";
    message_txt(i);
  }
    if ((u0sred/10) < (Unom+2)) {
    setobc1(0);
    setpwmU(1);
        String i = "Odsiarczanie ładowanie";
    message_txt(i);
  }
}

void odsiarczanie2()
{
  liczczas++;
  if ((u0sred/10) > setUmax) {
    setpwmU(0);
    setobc1(0);
    String i = "Odsiarczanie przerwa";
    message_txt(i);
    if (liczczas > 10){czaslad = liczczas;}
    liczczas = 0;
  }
    if ((u0sred/10) < (Unom+8)) {
    setobc1(0);
    setpwmU(1);
        String i = "Odsiarczanie ładowanie";
    message_txt(i);
    if (liczczas > 10){czasprzerwa = liczczas;}
    liczczas = 0;
  }
        String str = ("l=" + String(czaslad/4) + " p=" + String(czasprzerwa/4) + " x" + String(liczczas/4));
        //   Serial.println(str);
        BlynkvirtualWrite_str(20, str);
}
void pomiarRwew()
{
          String i = "pomiar Rwew";
    message_txt(i);
}
void programy_prost()
{
  switch (programprost)
  {
    case 1: {
        test();
      } break;
    case 2: {
        regU();
      } break;
    case 3: {
        regI();
      } break;//odsiarczanie
    case 4: {
        odsiarczanie();
      } break;//odsiarczanie
          case 5: {
        odsiarczanie2();
      } break;//odsiarczanie2
      case 6: {
        pomiarRwew();
      } break;//odsiarczanie2
    default: { // 0 i inne wartości - test
        test();
      }
  }
}

