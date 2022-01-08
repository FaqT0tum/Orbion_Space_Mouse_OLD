//Authors: Mattia Russo aka FaqT0tum
//         Cosik
//January 2022
//FREE FOR ANY NON COMMERCIAL PROJECT

#include "icon.h"
#include <EEPROM.h>
#include <Mouse.h>
#include <Keyboard.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h> // Comment if you use SSD1306
//#include <Adafruit_SSD1306.h>                // Uncomment if your oled is SSD1306
#define OLED_RESET 4
Adafruit_SH1106 display(OLED_RESET); // Comment if you use SSD1306
//Adafruit_SSD1306 display(OLED_RESET);        // Uncomment if your oled is SSD1306

/////////////// Encoder ////////////////////

uint8_t DT = 7;     // Pin  Detect                           <------------
uint8_t CLK = 6;    // Pin  Clockwise                        <------------
uint8_t encBut = 5; //Pin encoder Button                    <------------

uint8_t encBef = LOW;
uint8_t scr = LOW;
uint8_t encBefClick = 0;

/////////////// JoyStick ///////////////////

uint8_t horzPin = A1; // Pin Analog output of X        <------------
uint8_t vertPin = A2; // Pin Analog output of Y        <------------
uint8_t joyButt = 15; // Pin Joystick Button           <------------

int YValue, XValue;
int range;     // output range of X or Y movement
int threshold; // resting threshold
int center;    // resting position value

unsigned long tim, h, tim1, h1, tim2, h2;

/////////////// Mouse Axis ///////////////////////

uint16_t smooth;

/////////////// Rear Button  ///////////////

uint8_t butFun = 14; // Pin rear button
uint8_t butFunBef = 0;
char arButt[36] = {'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'z', 'x', 'c', 'v', 'b', 'n', 'm', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0'};
char cb;

/////////////// MENU ////////////////////////

uint8_t reset = LOW;
uint8_t sel = 0;
uint8_t exi = LOW;
uint8_t first = LOW;
uint8_t item = 0;
uint8_t on = 1;
uint8_t epr = 0;

void setup()
{

  //////////////////////////////////////////////////////////////////////////// PIN //////////////////////////////////////

  pinMode(horzPin, INPUT_PULLUP);
  pinMode(vertPin, INPUT_PULLUP);
  pinMode(encBut, INPUT_PULLUP);
  pinMode(butFun, INPUT_PULLUP);
  pinMode(DT, INPUT);
  pinMode(CLK, INPUT);
  pinMode(joyButt, INPUT_PULLUP);

  range = EEPROM.read(0);
  smooth = EEPROM.read(1);
  threshold = range / 4; // resting threshold
  center = range / 2;    // resting position value
  scr = digitalRead(DT);

  //////////////////////////////////////////////////////////////////////////// INITIALIZE ///////////////////////////////

  Mouse.begin();
  Keyboard.begin();

  display.begin(SH1106_SWITCHCAPVCC, 0x3C); // Comment if you use SSD1306
  //display.begin(SSD1306_SWITCHCAPVCC, 0x3C);          // Uncomment if your oled is SSD1306

  display.setRotation(2);
  display.clearDisplay();
}

void loop()
{
  //selSense(&sens);
  YValue = readAxis(vertPin);
  XValue = readAxis(horzPin) * -1;

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
    if (EEPROM.read(20) <= 35)
    {
      Keyboard.press(arButt[EEPROM.read(20)]);
    }
    if (EEPROM.read(20) >= 36)
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
      Mouse.release(MOUSE_RIGHT);
      Keyboard.releaseAll();
    }
  }

  //////////////////////////////////////////////////////////////////////////// CENTRAL BUTTON ///////////////////////////////
  //  Serial.println(encBefClick);
  if ((digitalRead(encBut) == 0) && (encBefClick == 0))
  {
    encBefClick = 1;
    if (EEPROM.read(30) <= 35)
    {
      Keyboard.press(arButt[EEPROM.read(30)]);
    }
    if (EEPROM.read(30) >= 36)
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
      Mouse.release(MOUSE_RIGHT);
      Keyboard.releaseAll();
    }
  }

  //////////////////////////////////////////////////////////////////////////// ORBIT & PAN //////////////////////////////

  tim = millis() - h;
  if (tim > smooth)
  {
    h = millis();
    if ((YValue != 0) || (XValue != 0))
    {
      selModes();
      Mouse.move(XValue, YValue, 0);
    }

    if ((YValue == 0) && (XValue == 0))
    {
      if (((digitalRead(encBut) == 1) && (digitalRead(butFun) == 1)) || (EEPROM.read(10) == 4))
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

  if (first == LOW)
  {
    start(&first);
  }
  if (digitalRead(joyButt) == LOW)
  {
    delay(300);
    do
    {
      item = 6; //////////////////// item main menu
      rotaryMenu(&sel, &item);
      Keyboard.releaseAll();
      Mouse.release(MOUSE_MIDDLE);
      Mouse.release(MOUSE_LEFT);
      Mouse.release(MOUSE_RIGHT);
      menu(&sel, &exi, &first);
    } while (exi == LOW);
    delay(300);
  }
  exi = LOW;
  sel = 0;
}

void start(uint8_t *f)
{
  uint8_t i = 0;
  if (on == 1)
  {
    do
    {
      display.clearDisplay();
      display.drawBitmap(0, 0, faq, 12 * i, 6 * i, WHITE);
      display.display();
      i++;
    } while (i != 20);
    on = 0;
  }
  display.clearDisplay();
  display.drawBitmap(0, 0, faq, 128, 64, WHITE);
  display.display();
  *f = 1;
}

///////////////////////////////////////////////////////////////////////////  MENU /////////////////////////////////////

void rotaryMenu(uint8_t *s, uint8_t *i)
{
  if ((digitalRead(DT) == HIGH) && (digitalRead(CLK) == LOW) && (reset == LOW))
  {
    if (*s <= (*i - 2))
    {
      *s = *s + 1;
    }
    reset = HIGH;
  }
  if ((digitalRead(DT) == LOW) && (digitalRead(CLK) == HIGH) && (reset == LOW))
  {
    if ((*s <= (*i - 1)) && (*s > 0)) //set number of item
    {
      *s = *s - 1;
    }
    else
    {
      *s = 0;
    }
    reset = HIGH;
  }
  if (digitalRead(DT) && digitalRead(CLK))
  {
    reset = LOW;
  }
}

void menu(uint8_t *s, uint8_t *e, uint8_t *f)
{
  switch (*s)
  {
  case 0:
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(2);
    display.setCursor(11, 25);
    display.println("Joy Sense");
    display.display();
    if (digitalRead(butFun) == LOW)
    {
      joySens();
    }
    break;

  case 1:
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(2);
    display.setCursor(11, 25);
    display.println("Joy Delay");
    display.display();
    if (digitalRead(butFun) == LOW)
    {
      joyDelay();
    }
    break;

  case 2:
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(2);
    display.setCursor(16, 25);
    display.println("Joy Mode");
    display.display();
    if (digitalRead(butFun) == LOW)
    {
      joyMode();
    }
    break;

  case 3:
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(2);
    display.setCursor(10, 25);
    display.println("Knob Push");
    display.display();
    if (digitalRead(butFun) == LOW)
    {
      epr = 30;
      buttMode(&epr);
    }
    break;

  case 4:
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(2);
    display.setCursor(10, 25);
    display.println("Rear Push");
    display.display();
    if (digitalRead(butFun) == LOW)
    {
      epr = 20;
      buttMode(&epr);
    }
    break;

  case 5:
    display.clearDisplay();
    display.drawBitmap(0, 0, ext, 128, 64, WHITE);
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.setCursor(49, 54);
    display.println("v0.26");
    display.display();
    if (digitalRead(butFun) == LOW)
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
  uint8_t t = EEPROM.read(10);
  uint8_t ex = LOW;
  delay(500);
  do
  {
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(2);
    display.setCursor(16, 10);
    display.println("Joy Mode");
    if (t == 0)
    {
      display.setCursor(33, 35);
      display.print("Mouse");
    }
    if (t == 1)
    {
      display.setCursor(17, 35);
      display.print("Autodesk");
    }
    if (t == 2)
    {
      display.setCursor(10, 35);
      display.print("Solid W/E");
    }
    if (t == 3)
    {
      display.setCursor(23, 35);
      display.print("Blender");
    }
    if (t == 4)
    {
      display.setCursor(33, 35);
      display.print("Adobe");
    }
    if (t == 5)
    {
      display.setCursor(23, 35);
      display.print("FreeCad");
    }
    if (t == 6)
    {
      display.setCursor(33, 35);
      display.print("Rhino");
    }
    if (t == 7)
    {
      display.setCursor(28, 35);
      display.print("ZBrush");
    }
    if (t == 8)
    {
      display.setCursor(17, 35);
      display.print("Onshape");
    }
    display.display();
    item = 9; //number of modes
    rotaryMenu(&t, &item);
    if (digitalRead(butFun) == LOW)
    {
      ex = HIGH;
      EEPROM.update(10, t);
    }
  } while (ex == LOW);
  delay(500);
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
    //Onshape
    Mouse.press(MOUSE_RIGHT);
    break;
  }
}

/////////////////////////////////////////////////////////////////////////// JOY SENSE /////////////////////////////////

void joySens()
{
  uint8_t ex2 = LOW;
  uint8_t t = EEPROM.read(0);
  delay(500);
  do
  {
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(2);
    display.setCursor(10, 10);
    display.println("Joy Sense");
    display.setCursor(56, 35);
    display.print(t + 1);
    display.display();
    item = 255;
    rotaryMenu(&t, &item);
    range = t;
    threshold = range / 4; // resting threshold
    center = range / 2;    // resting position value
    if (digitalRead(butFun) == LOW)
    {
      ex2 = HIGH;
      EEPROM.update(0, t);
    }
  } while (ex2 == LOW);
  delay(500);
  ex2 = LOW;
}

void joyDelay()
{
  uint8_t ex2 = LOW;
  uint8_t t = EEPROM.read(1);
  delay(500);
  do
  {
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(2);
    display.setCursor(10, 10);
    display.println("Joy Delay");
    display.setCursor(56, 35);
    display.print(t + 1);
    display.display();
    item = 100;
    rotaryMenu(&t, &item);
    smooth = t;
    if (digitalRead(butFun) == LOW)
    {
      ex2 = HIGH;
      EEPROM.update(1, t);
    }
  } while (ex2 == LOW);
  delay(500);
  ex2 = LOW;
}

/////////////////////////////////////////////////////////////////////////// BUTTON MODE /////////////////////////////////

void buttMode(uint8_t *e)
{
  uint8_t ex = LOW;
  uint8_t t = EEPROM.read(*e);
  delay(500);
  do
  {
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(2);
    display.setCursor(10, 10);
    display.println("SELECT FN");
    if (t <= 35)
    {
      display.setCursor(58, 35);
      display.print(arButt[t]);
    }
    if (t == 36)
    {
      display.setCursor(36, 35);
      display.print("Space");
    }
    if (t == 37)
    {
      display.setCursor(45, 35);
      display.print("CTRL");
    }
    if (t == 38)
    {
      display.setCursor(37, 35);
      display.print("Shift");
    }
    if (t == 39)
    {
      display.setCursor(48, 35);
      display.print("Alt");
    }
    if (t == 40)
    {
      display.setCursor(48, 35);
      display.print("Tab");
    }
    if (t == 41)
    {
      display.setCursor(49, 35);
      display.print("Esc");
    }
    if (t == 42)
    {
      display.setCursor(49, 35);
      display.print("Del");
    }
    if (t == 43)
    {
      display.setCursor(31, 35);
      display.print("Scroll");
    }
    if (t == 44)
    {
      display.setCursor(10, 35);
      display.print("Solid W/E");
    }
    if (t == 45)
    {
      display.setCursor(10, 35);
      display.print("RightM");
    }
    display.display();
    item = 46;
    rotaryMenu(&t, &item);
    if (digitalRead(butFun) == LOW)
    {
      ex = HIGH;
      EEPROM.update(*e, t);
    }
  } while (ex == LOW);
  delay(500);
}

void selButt(uint8_t *e)
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
    Keyboard.press(KEY_LEFT_GUI);
    Mouse.press(MOUSE_RIGHT);
    break;
  }
}

int readAxis(int thisAxis)
{
  // read the analog input:
  uint16_t reading = analogRead(thisAxis);

  // map the reading from the analog input range to the output range:
  reading = map(reading, 20, 1023, 0, range);

  // if the output reading is outside from the rest position threshold, use it:
  int distance = reading - center;

  if (abs(distance) < threshold)
  {
    distance = 0;
  }

  // return the distance for this axis:
  return distance;
}
