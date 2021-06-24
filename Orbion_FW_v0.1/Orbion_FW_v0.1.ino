//Author Mattia Russo aka FaqT0tum
//June 2021
//FREE FOR ANY NON COMMERCIAL PROJECT

#include "icon.h"
#include <EEPROM.h>
#include <Mouse.h>
#include <Keyboard.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>
#define OLED_RESET 4
Adafruit_SH1106 display(OLED_RESET);

/////////////// Encoder ////////////////////

int DT = 7;       // Pin  Detect
int CLK = 6;      // Pin  Clockwise
int encBut = 5;    //Pin Button 

int encBef = LOW;  
int scr = LOW;            
int encBefClick = 0;     

/////////////// JoyStick ///////////////////

int horzPin = A2;     // Analog output of X
int vertPin = A1 ;     // Analog output of Y

int moved = 0;        
int YZero, XZero;     
int YValue, XValue;   
int sens = 0;         
int arSens[5] = {60,35,23,10,5};
int used = 0;         
int lastused = 0;     
int offsetJoyX = 12;    // set this value if the joystick moves by itself
int offsetJoyY = 12;    // set this value if the joystick moves by itself

unsigned long tim, h;

/////////////// Rear Button  ///////////////

int butFun = 14;   // rear button
int butFunBef = 0;  
char arButt [36] = {'q','w','e','r','t','y','u','i','o','p','a','s','d','f','g','h','j','k','l','z','x','c','v','b','n','m','1','2','3','4','5','6','7','8','9','0'};
char cb;

/////////////// Mouse Axis ///////////////////////

int invertMouseX = -1;
int invertMouseY = 1;
int inv = 0;

/////////////// MENU ////////////////////////

int reset = LOW;
int sel = 0;
int exi = LOW;
int push = 15;
int first = LOW;
int item = 0;
int on = 1;

void setup() 
  {
    
//////////////////////////////////////////////////////////////////////////// PIN //////////////////////////////////////
    
    pinMode(horzPin, INPUT_PULLUP);        
    pinMode(vertPin, INPUT_PULLUP);
    pinMode(encBut, INPUT_PULLUP);         
    pinMode(butFun, INPUT_PULLUP);        
    pinMode (DT, INPUT);
    pinMode (CLK, INPUT);
    pinMode (push, INPUT_PULLUP);
    
    YZero = analogRead(vertPin);  
    XZero = analogRead(horzPin);  
    scr = digitalRead(DT);
    
//////////////////////////////////////////////////////////////////////////// INITIALIZE ///////////////////////////////
    
    Mouse.begin();      
    Keyboard.begin();   
    
    display.begin(SH1106_SWITCHCAPVCC, 0x3C);
    display.setRotation(2);
    display.clearDisplay();
  }

void loop() 
  {
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
        Mouse.move(0, 0, -1);
      } 
      else 
        {
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
          selButt();
        }
    }
  else if (digitalRead(butFun) && (butFunBef))
    {
      butFunBef = 0;
      Mouse.release(MOUSE_MIDDLE);
      Keyboard.releaseAll();
      delay(50);
    }

//////////////////////////////////////////////////////////////////////////// CENTRAL BUTTON ///////////////////////////////
  Serial.println(encBefClick);
  if ((digitalRead(encBut) == 0) && (encBefClick == 0)) 
    { 
      encBefClick = 1;
      if (EEPROM.read(30) <=35) 
        {
          Keyboard.press(arButt [EEPROM.read(30)]);
        }
      if (EEPROM.read(30) >=36) 
        {
          selcenButt();
        }
    }
  else if (digitalRead(encBut) && (encBefClick))
    {
      encBefClick = 0;
      Mouse.release(MOUSE_MIDDLE);
      Keyboard.releaseAll();
      delay(50);
    }

//////////////////////////////////////////////////////////////////////////// ORBIT & PAN //////////////////////////////
  tim = millis() - h;
  if (tim >= 30)
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
          Keyboard.releaseAll();
          if ((Mouse.isPressed(MOUSE_MIDDLE)==1) && (digitalRead(encBut) == 1) && (digitalRead(butFun) == 1))
            {
              Mouse.release(MOUSE_MIDDLE);
              delay(50);
            }
        }
     }

     
//////////////////////////////////////////////////////////////////////////// MAIN /////////////////////////////////////

    
  if(first == LOW)
    {
      start(&first);
    } 
  if(digitalRead(push) == LOW)
    {
      delay(300);
      do
        {
          item = 5; //////////////////// item main menu
          rotaryMenu(&sel, &item);
          menu(&sel, &exi, &first);
        }
      while (exi==LOW);
      delay(300);
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
            //delay(5);
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
    if(digitalRead(DT) && !digitalRead(CLK) && !reset)
      {
        if(*s<=(*i-2))
          {
            *s = *s +1;
          }
        reset = HIGH;
      }
    if(!digitalRead(DT) && digitalRead(CLK) && !reset)
      {
        if((*s<=(*i-1)) && (*s>=0)) //set number of item
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
          if(digitalRead(push)==LOW)
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
          if(digitalRead(push)==LOW)
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
          if(digitalRead(push)==LOW)
            {
              cenButt();
            }
        break;
  
        case 3:
          display.clearDisplay();
          display.setTextColor(WHITE);
          display.setTextSize(2);
          display.setCursor(10,25);
          display.println("Rear Push");
          display.display();
          if(digitalRead(push)==LOW)
            {
              buttMode();
            }
        break;
  
        case 4:
          display.clearDisplay();
          display.drawBitmap(0, 0, ext, 128, 64, WHITE);
          display.display();
          if(digitalRead(push)==LOW)
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
    delay(500);
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
        display.display();
        rotaryJoy(&t);                     
        if(digitalRead(push)==LOW)
          {
            ex = HIGH;
            EEPROM.update(10, t);
          }
      }
    while (ex == LOW);
    delay(500);
  }

void rotaryJoy(int* s)
  {
    if(digitalRead(DT) && !digitalRead(CLK) && !reset)
      {
        reset=HIGH;
        if(*s<=2)
          {
          *s = *s +1;
          }
      }  
    if(!digitalRead(DT) && digitalRead(CLK) && !reset)
      {
        reset=HIGH;
        if((*s<=3) && (*s>1))
            {
            *s = *s -1;
            }
          else
            {
            *s = 0;  
            }
      }
    if(digitalRead(DT) && digitalRead(CLK))
      {
        reset=LOW;
      } 
    delay(10);
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
    }  
  }

/////////////////////////////////////////////////////////////////////////// JOY SENSE /////////////////////////////////

void joySens()
  {
  int ex2 = LOW; 
  int t = EEPROM.read(0);
  delay(500);
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
    rotaryJoySense(&t);
    if(digitalRead(push)==LOW)
      {
      ex2=HIGH;
      EEPROM.update(0, t);
      }
    }
  while (ex2==LOW);
  delay(500);
  ex2=LOW;
  }

void rotaryJoySense(int* s)
  {
    if(digitalRead(DT) && !digitalRead(CLK) && !reset)
      {
        reset=HIGH;
        if(*s<=3)
          {
          *s = *s +1;
          }
      }  
    if(!digitalRead(DT) && digitalRead(CLK) && !reset)
      {
        reset=HIGH;
        if((*s<=4) && (*s>1))
            {
            *s = *s -1;
            }
          else
            {
            *s = 0;  
            }
      }
    if(digitalRead(DT) && digitalRead(CLK))
      {
        reset=LOW;
      } 
    delay(10);
  }



/////////////////////////////////////////////////////////////////////////// REAR BUTTON MODE /////////////////////////////////

void buttMode()
  {
    int t = EEPROM.read(20);
    int ex = LOW;
    delay(500);
    do 
      {
        display.clearDisplay();
        display.setTextColor(WHITE);
        display.setTextSize(2);
        display.setCursor(16,10);
        display.println("REAR BUTT");
        display.setCursor(20,35);
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
        display.display();
        rotaryButt(&t);
        if(digitalRead(push)==LOW)
          {
            ex = HIGH;
            EEPROM.update(20, t);
          }
      }
    while (ex == LOW);
    delay(500);
  }

void selButt()
  {
  switch (EEPROM.read(20))
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
      } 
  }

/////////////////////////////////////////////////////////////////////////// KNOB BUTTON MODE /////////////////////////////////

void cenButt()
  {
    int t = EEPROM.read(30);
    int ex = LOW;
    delay(500);
    do 
      {
        display.clearDisplay();
        display.setTextColor(WHITE);
        display.setTextSize(2);
        display.setCursor(12,10);
        display.println("KNOB BUTT");
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
        display.display();
        rotaryButt(&t);
        if(digitalRead(push)==LOW)
          {
            ex = HIGH;
            EEPROM.update(30, t);
          }
      }
    while (ex == LOW);
    delay(500);
  }

void rotaryButt(int* s)
  {
    if(digitalRead(DT) && !digitalRead(CLK) && !reset)
      {
        reset=HIGH;
        if(*s<=43)
          {
          *s = *s +1;
          }
      }  
    if(!digitalRead(DT) && digitalRead(CLK) && !reset)
      {
        reset=HIGH;
        if((*s<=44) && (*s>1))
            {
            *s = *s -1;
            }
          else
            {
            *s = 0;  
            }
      }
    if(digitalRead(DT) && digitalRead(CLK))
      {
        reset=LOW;
      } 
    delay(10);
  }

void selcenButt()
  {
  switch (EEPROM.read(30))
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
      } 
  }
