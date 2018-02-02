#include <Arduino.h>
void BlynkvirtualWrite_col(byte vPin4, String col);
void BlynkvirtualWrite_On(byte vPin5, String ON) ;
void BlynkvirtualWrite_Off(byte vPin6, String OFF);


#define BLYNK_GREEN     "#23C48E"
#define BLYNK_BLUE      "#04C0F8"
#define BLYNK_YELLOW    "#ED9D00"
#define BLYNK_RED       "#D3435C"
#define BLYNK_DARK_BLUE "#5F7CD8"

#define _RED        "#FF0000"
#define _GREEN      "#00FF00"
#define _MAGENTA    "#FF00FF"
#define _YELLOW     "#FFFF00"
#define _BLACK      "#000000"
#define _WHITE      "#FFFFFF"
#define _WBLUE      "#00FFFF"
#define _PINK       "#FFB3B3"
#define _OLIVE      "#808000"
#define _BRAU       "#FF8000"
#define _karmin     "#FF0080"
#define _SEE        "#00FF80"
#define _fiolet     "#8000FF"
#define _BLUE       "#0080FF"


void setupstart() { //początkowe ustawienia widgetow
  BlynkvirtualWrite_col(30, _GREEN);
  BlynkvirtualWrite_col(1, BLYNK_GREEN);
  BlynkvirtualWrite_col(2, _RED);
  BlynkvirtualWrite_col(10, _BLUE);
}
 void setupstart1()
{ 
  BlynkvirtualWrite_col(11, _BLUE);
  BlynkvirtualWrite_col(12, _MAGENTA);
  BlynkvirtualWrite_col(15, _YELLOW);
    BlynkvirtualWrite_col(18, _BLUE);
      BlynkvirtualWrite_col(19, _RED);
}
void setupstart2()
{
  BlynkvirtualWrite_Off(10, "PILOT");
  BlynkvirtualWrite_On(10, "PILOT");
  BlynkvirtualWrite_On(12, "k1");
  BlynkvirtualWrite_Off(12, "k1");
}
void setupstart3()
{
  BlynkvirtualWrite_On(15, "POMPA");
  BlynkvirtualWrite_Off(15, "POMPA");
    BlynkvirtualWrite_Off(18, "DOWN");
      BlynkvirtualWrite_Off(19, "UP");
}
void setupstart4()
{

}

