//Author Mattia Russo aka FaqT0tum
//June 2021
//FREE FOR ANY NON COMMERCIAL PROJECT

#include "icon.h"
#include <EEPROM.h>
#include <Mouse.h>
#include "Keyboard.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>                   // Comment if you use SSD1306
//#include <Adafruit_SSD1306.h>                // Uncomment if your oled is SSD1306
#define OLED_RESET 4
Adafruit_SH1106 display(OLED_RESET);           // Comment if you use SSD1306
//Adafruit_SSD1306 display(OLED_RESET);        // Uncomment if your oled is SSD1306
    
String vers = "0.26.3";

/////////////// MACRO KEYBOARD /////////////

char shiftKey = KEY_LEFT_SHIFT;
const int L = 4;      //rot      L für Linie               <------------
const int C = 8;      //gelb     C für Kreis               <------------  
const int R = 9;      //grün     R für Rechteck            <------------
const int E = 21;     //schwarz  E für Extrusion           <------------
const int Q = 10;     //weiß     Q für Drücken/Ziehen      <------------
const int SHIFT = 16; //blau Shift für Taste Schift        <------------   
/////////////// Encoder ////////////////////

int DT = 7;       // Pin  Detect                           <------------
int CLK = 6;      // Pin  Clockwise                        <------------
int encBut = 5;    //Pin encoder Button                    <------------

int encBef = LOW;  
int scr = LOW;            
int encBefClick = 0;     

/////////////// JoyStick ///////////////////

int horzPin = A1;         // Pin Analog output of X        <------------
int vertPin = A2;        // Pin Analog output of Y        <------------
int joyButt = 15;         // Pin Joystick Button           <------------

int moved = 0;        
int YZero, XZero;     
int YValue, XValue;   
int sens = 0;         
int arSens[5] = {125,100,85,60,35};
int used = 0;         
int lastused = 0;     
int offsetJoyX = 15;    // set this value if the joystick moves by itself
int offsetJoyY = 15;    // set this value if the joystick moves by itself

unsigned long tim, h, tim1, h1, tim2, h2;

/////////////// Rear Button  ///////////////

int butFun = 14;   // Pin rear button                      <------------
int butFunBef = 0;  
char arButt [36] = {'q','w','e','r','t','y','u','i','o','p','a','s','d','f','g','h','j','k','l','z','x','c','v','b','n','m','1','2','3','4','5','6','7','8','9','0'};
char cb;

/////////////// Mouse Axis ///////////////////////

int invertMouseX = -1;
int invertMouseY = 1;
int inv = 0;
int smooth = 0;

/////////////// MENU ////////////////////////

int reset = LOW;
int sel = 0;
int exi = LOW;
int first = LOW;
int item = 0;
int on = 1;
int epr = 0;

void setup() 
  {

//////////////// MACRO KEYBOARD //////////////

    pinMode(L, INPUT_PULLUP);
    pinMode(C, INPUT_PULLUP);
    pinMode(R, INPUT_PULLUP);
    pinMode(E, INPUT_PULLUP);
    pinMode(Q, INPUT_PULLUP);
    pinMode(SHIFT, INPUT_PULLUP);							 
    
//////////////////////////////////////////////////////////////////////////// PIN //////////////////////////////////////
    
    pinMode(horzPin, INPUT_PULLUP);        
    pinMode(vertPin, INPUT_PULLUP);
    pinMode(encBut, INPUT_PULLUP);         
    pinMode(butFun, INPUT_PULLUP);        
    pinMode (DT, INPUT);
    pinMode (CLK, INPUT);
    pinMode (joyButt, INPUT_PULLUP);
    
    YZero = analogRead(vertPin);  
    XZero = analogRead(horzPin);  
    scr = digitalRead(DT);
    
//////////////////////////////////////////////////////////////////////////// INITIALIZE ///////////////////////////////
    
    Mouse.begin();      
    Keyboard.begin();   
    
    display.begin(SH1106_SWITCHCAPVCC, 0x3C);             // Comment if you use SSD1306
    //display.begin(SSD1306_SWITCHCAPVCC, 0x3C);          // Uncomment if your oled is SSD1306
    
    display.setRotation(2);
    display.clearDisplay();
  }

void loop() 
  {

/////////////// MACRO KEYBOARD ///////////////

// read the pushbuttons:
  int SHIFT_State = digitalRead(SHIFT);
  int L_State = digitalRead(L);
  int C_State = digitalRead(C);
  int R_State = digitalRead(R);
  int E_State = digitalRead(E);
  int Q_State = digitalRead(Q);  
   
 // _________Shift & 1_________ //
    
     if (SHIFT_State == LOW){
        Keyboard.release(KEY_LEFT_SHIFT);
        Keyboard.press(shiftKey); //press Shift
        Keyboard.press('1');      //press 1
        Keyboard.releaseAll();
        delay(500);
        }
 
 // _____________L____________ //
 
  else if(L_State == LOW){
    Keyboard.press('l'); //press l
    Keyboard.releaseAll();
    delay(500);
    }
   
 // _____________C____________ //
 
  else if(C_State == LOW){
    Keyboard.press('c'); //press c
    Keyboard.releaseAll();
    delay(500);
    }
// _____________R____________ //
 
  else if(R_State == LOW){
    Keyboard.press('r'); //press r
    Keyboard.releaseAll();
    delay(500);
    }
// _____________E____________ //
 
  else if(E_State == LOW){
    Keyboard.press('e'); //press e
    Keyboard.releaseAll();
    delay(500);
    }
// _____________Q____________ //
 
  else if(Q_State == LOW){
    Keyboard.press('q'); //press q
    Keyboard.releaseAll();
    delay(500);
    }
   else {
  Keyboard.releaseAll();
  }
////////////////////////////////////////////////////////////////////////////
  //selSense(&sens);
  sens = arSens[EEPROM.read(0)];
  
  YValue = analogRead(vertPin) - YZero;  
  XValue = analogRead(horzPin) - XZero;

//////////////////////////////////////////////////////////////////////////// SCROLL ///////////////////////////////////

  scr = digitalRead(DT);
  if ((encBef == LOW) && (scr == HIGH)) 
    {
    if (digitalRead(CLK) == LOW) 
      {
        if (EEPROM.read(10) == 4)
            {
              Keyboard.press(KEY_LEFT_ALT);
            }
        Mouse.move(0, 0, -1);
      } 
      else 
        {
          if (EEPROM.read(10) == 4)
            {
              Keyboard.press(KEY_LEFT_ALT);
            }
          Mouse.move(0, 0, 1);
        }
    }
  encBef = scr;
    
//////////////////////////////////////////////////////////////////////////// REAR BUTTON ///////////////////////////////

  if ((digitalRead(butFun) == 0) && (butFunBef == 0)) 
    { 
      butFunBef = 1;
      if (EEPROM.read(20) <=35) 
        {
          Keyboard.press(arButt [EEPROM.read(20)]);
        }
      if (EEPROM.read(20) >=36) 
        {
          epr = 20;
          selButt(&epr);
        }
    }
  else if (digitalRead(butFun) && (butFunBef))
    {
      butFunBef = 0;
      tim2 = millis() - h2;
      if (tim2 > 50)
        {
          h2 = millis();
          Mouse.release(MOUSE_MIDDLE);
          Keyboard.releaseAll();
        }
    }

//////////////////////////////////////////////////////////////////////////// CENTRAL BUTTON ///////////////////////////////
  
  if ((digitalRead(encBut) == 0) && (encBefClick == 0)) 
    { 
      encBefClick = 1;
      if (EEPROM.read(30) <=35) 
        {
          Keyboard.press(arButt [EEPROM.read(30)]);
        }
      if (EEPROM.read(30) >=36) 
        {
          epr = 30;
          selButt(&epr);
        }
    }
  else if (digitalRead(encBut) && (encBefClick))
    {
      encBefClick = 0;
      tim2 = millis() - h2;
      if (tim2 > 50)
        {
          h2 = millis();
          Mouse.release(MOUSE_MIDDLE);
          Keyboard.releaseAll();
        }
    }

//////////////////////////////////////////////////////////////////////////// ORBIT & PAN //////////////////////////////
  if(EEPROM.read(10) == 0)
    {
      smooth = 0;
    }
  else
    {
      smooth = 30;
    }
  
  tim = millis() - h;
  if (tim > smooth)
    {
      h = millis();
      if ((YValue > offsetJoyY)||(YValue < (-offsetJoyY)))
        { 
          selModes();
          Mouse.move(0, (invertMouseY * (YValue / sens)), 0); 
          moved=1;
        }
        
      if ((XValue > offsetJoyX)||(XValue < (-offsetJoyX)))
        {  
          selModes();
          Mouse.move((invertMouseX * (XValue / sens)), 0, 0);
          moved=1;
        }
      
      if ( (YValue <= offsetJoyY)&&(YValue >= (-offsetJoyY))  &&  (XValue <= offsetJoyX)&&(XValue >= (-offsetJoyX)))
        {  
          if ( (digitalRead(encBut) == 1) && (digitalRead(butFun) == 1) || (EEPROM.read(10) == 4) )
            {
              Keyboard.releaseAll();
              tim1 = millis() - h1;
              if (tim1 > 120)
                {
                  h1 = millis();
                  Mouse.release(MOUSE_MIDDLE);
                  Mouse.release(MOUSE_LEFT);
                  Mouse.release(MOUSE_RIGHT);
                }
            }
          
        }
     }

     
//////////////////////////////////////////////////////////////////////////// MAIN /////////////////////////////////////

    
  if(first == LOW)
    {
      start(&first);
    } 
  if((digitalRead(joyButt) == LOW) && (digitalRead(butFun)== LOW))
    {
      delay(500);
      Keyboard.releaseAll();
      Mouse.release(MOUSE_MIDDLE);
      Mouse.release(MOUSE_LEFT);
      Mouse.release(MOUSE_RIGHT);
      do
        {
          item = 5; //////////////////// item main menu
          rotaryMenu(&sel, &item);
          menu(&sel, &exi, &first);
        }
      while (exi==LOW);
    }
  exi = LOW;
  sel = 0;
  }

void start(int* f) 
  {
    int i = 0;
    if(on == 1)
      {
        do
          {
            display.clearDisplay();
            display.drawBitmap(0, 0, faq, 12*i, 6*i, WHITE);
            display.display();
            i++;
          }
        while(i != 20);  
        on = 0;
      }
    display.clearDisplay();
    display.drawBitmap(0, 0, faq, 128, 64, WHITE);
    display.display();
    *f=1;
  }

///////////////////////////////////////////////////////////////////////////  MENU /////////////////////////////////////

void rotaryMenu(int* s, int* i) 
  {
    if((digitalRead(DT) == HIGH) && (digitalRead(CLK) == LOW) && (reset == LOW))
      {
        if(*s<=(*i-2))
          {
            *s = *s +1;
          }
        reset = HIGH;
      }
    if((digitalRead(DT) == LOW) && (digitalRead(CLK) == HIGH) && (reset == LOW))
      {
        if((*s<=(*i-1)) && (*s>0)) //set number of item
          {
            *s = *s -1;
          }
        else
          {
            *s=0;  
          }
        reset = HIGH;
      }
    if(digitalRead(DT) && digitalRead(CLK))
      {
        reset = LOW;
      }
  }

void menu(int* s, int* e, int* f)
  {
    switch (*s)
      {
        case 0:
          display.clearDisplay();
          display.setTextColor(WHITE);
          display.setTextSize(2);
          display.setCursor(11,25);
          display.println("Joy Sense");
          display.display();
          if(digitalRead(butFun)==LOW)
            {
              joySens();
            }
        break;
  
        case 1:
          display.clearDisplay();
          display.setTextColor(WHITE);
          display.setTextSize(2);
          display.setCursor(16,25);
          display.println("Joy Mode");
          display.display();
          if(digitalRead(butFun)==LOW)
            {
              joyMode();
            }
        break;
  
        case 2:
          display.clearDisplay();
          display.setTextColor(WHITE);
          display.setTextSize(2);
          display.setCursor(10,25);
          display.println("Knob Push");
          display.display();
          if(digitalRead(butFun)==LOW)
            {
              epr = 30;
              buttMode(&epr);
            }
        break;
  
        case 3:
          display.clearDisplay();
          display.setTextColor(WHITE);
          display.setTextSize(2);
          display.setCursor(10,25);
          display.println("Rear Push");
          display.display();
          if(digitalRead(butFun)==LOW)
            {
              epr = 20;
              buttMode(&epr);
            }
        break;
  
        case 4:
          display.clearDisplay();
          display.drawBitmap(0, 0, ext, 128, 64, WHITE);
          display.setTextColor(WHITE);
          display.setTextSize(1);
          display.setCursor(47,54);
          display.println(vers);
          display.display();
          if(digitalRead(butFun)==LOW)
            {
              *e = HIGH;
            }
        break;
      }
    *f = LOW;
  }


/////////////////////////////////////////////////////////////////////////// JOY MODES /////////////////////////////////

void joyMode()
  {
    int t = EEPROM.read(10);
    int ex = LOW;
    delay(300);
    do 
      {
        display.clearDisplay();
        display.setTextColor(WHITE);
        display.setTextSize(2);
        display.setCursor(16,10);
        display.println("Joy Mode");
        if(t == 0)
          { 
            display.setCursor(33,35);
            display.print("Mouse");
          }
        if(t == 1)
          {
            display.setCursor(17,35);
            display.print("Autodesk");
          }
        if(t == 2)
          {
            display.setCursor(10,35);
            display.print("Solid W/E");
          }
        if(t == 3)
          {
            display.setCursor(23,35);
            display.print("Blender");
          }
        if(t == 4)
          {
            display.setCursor(33,35);
            display.print("Adobe");
          }
        if(t == 5)
          {
            display.setCursor(23,35);
            display.print("FreeCad");
          }
        if(t == 6)
          {
            display.setCursor(33,35);
            display.print("Rhino");
          }
        if(t == 7)
          {
            display.setCursor(28,35);
            display.print("ZBrush");
          }
        if(t == 8)
          {
            display.setCursor(41,35);
            display.print("ANSA");
          }
        if(t == 9)
          {
            display.setCursor(23,35);
            display.print("Onshape");
          }
        display.display();
        item = 10;                     //number of modes
        rotaryMenu(&t, &item); 
        Serial.println(t);                   
        if(digitalRead(butFun)==LOW)
          {
            ex = HIGH;
            EEPROM.update(10, t);
          }
      }
    while (ex == LOW);
    delay(300);
  }

void selModes()
  {
  switch (EEPROM.read(10))
    {
      case 0:
        //mouse mode
      break;

      case 1:
        //Autodesk
        Keyboard.press(KEY_LEFT_SHIFT);
        Mouse.press(MOUSE_MIDDLE);
      break;

      case 2:
        //Solid Work/Edge
        Mouse.press(MOUSE_MIDDLE);
      break;

      case 3:
        //Blender
        Keyboard.press(KEY_LEFT_SHIFT);
        Keyboard.press(KEY_LEFT_ALT);
        Mouse.press(MOUSE_MIDDLE);
      break;

      case 4:
        //Adobe
        Keyboard.press(32);
        Mouse.press(MOUSE_LEFT);
      break;

      case 5:
        //Freecad
        Mouse.press(MOUSE_MIDDLE);
        Mouse.press(MOUSE_RIGHT);
      break;

      case 6:
        //Rhino
        Mouse.press(MOUSE_RIGHT);
      break;

      case 7:
        //ZBrush
        Mouse.press(MOUSE_LEFT);
      break;

      case 8:
        //ANSA
        Keyboard.press(KEY_LEFT_CTRL);
        Mouse.press(MOUSE_LEFT);
      break;

      case 9:
        //Onshape
        Mouse.press(MOUSE_RIGHT);
        break;
    }  
  }

/////////////////////////////////////////////////////////////////////////// JOY SENSE /////////////////////////////////

void joySens()
  {
  int ex2 = LOW; 
  int t = EEPROM.read(0);
  delay(300);
  do 
    {
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(2);
    display.setCursor(10,10);
    display.println("Joy Sense");
    display.setCursor(56,35);
    display.print(t+1);
    display.display();
    item = 5;
    rotaryMenu(&t, &item);
    if(digitalRead(butFun)==LOW)
      {
      ex2=HIGH;
      EEPROM.update(0, t);
      }
    }
  while (ex2==LOW);
  delay(300);
  ex2=LOW;
  }

/////////////////////////////////////////////////////////////////////////// BUTTON MODE /////////////////////////////////

void buttMode(int *e)
  {
    int ex = LOW;
    int t = EEPROM.read(*e);
    delay(500);
    do 
      {
        display.clearDisplay();
        display.setTextColor(WHITE);
        display.setTextSize(2);
        display.setCursor(10,10);
        display.println("SELECT FN");
        if (t <= 35)
          {
            display.setCursor(58,35);
            display.print(arButt[t]);
          }
        if (t == 36)
          {
            display.setCursor(36,35);
            display.print("Space");
          }
        if (t == 37)
          {
            display.setCursor(45,35);
            display.print("CTRL");
          }
        if (t == 38)
          {
            display.setCursor(37,35);
            display.print("Shift");
          }
        if (t == 39)
          {
            display.setCursor(48,35);
            display.print("Alt");
          }
        if (t == 40)
          {
            display.setCursor(48,35);
            display.print("Tab");
          }
        if (t == 41)
          {
            display.setCursor(49,35);
            display.print("Esc");
          }
        if (t == 42)
          {
            display.setCursor(49,35);
            display.print("Del");
          }
        if (t == 43)
          {
            display.setCursor(31,35);
            display.print("Scroll");
          }
        if (t == 44)
          {
            display.setCursor(10,35);
            display.print("Solid W/E");
          }
        if (t == 45)
          {
            display.setCursor(41,35);
            display.print("ANSA");
          }
        display.display();
        item = 46;
        rotaryMenu(&t, &item);
        if(digitalRead(butFun)==LOW)
          {
            ex = HIGH;
            EEPROM.update(*e, t);
          }
      }
    while (ex == LOW);
    delay(300);
  }

void selButt(int *e)
  {
  switch (EEPROM.read(*e))
      {
        case 36:
          Keyboard.press(32);          
        break;
  
        case 37:
          Keyboard.press(KEY_LEFT_CTRL);
        break;
  
        case 38:
          Keyboard.press(KEY_LEFT_SHIFT);
        break;
  
        case 39:
          Keyboard.press(KEY_LEFT_ALT);
        break;
  
        case 40:
          Keyboard.press(KEY_TAB);
        break;
  
        case 41:
          Keyboard.press(KEY_ESC);
        break;
  
        case 42:
          Keyboard.press(KEY_DELETE);
        break;

        case 43:
          Mouse.press(MOUSE_MIDDLE);
        break;

        case 44:
          Keyboard.press(KEY_LEFT_GUI);
          Mouse.press(MOUSE_MIDDLE);
        break;
          
        case 45:
          Keyboard.press(KEY_LEFT_CTRL);
          Mouse.press(MOUSE_MIDDLE);
        break;
      } 
  }
