#include "FastLED.h"

// ColourClock
// Reverse rainbow fade showing colors mapped over an interval of time. Useful as a non-nunmeric timer.
// Tested on Adafruit Circuit Playground Express with a strand of NeoPixels.
// by aTegart, May 2019
//


#define DATA_PIN    6
#define SLIDE       7
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    20       // Change this to reflect the number of LEDs you have
#define BRIGHTNESS  80       // Set brightness here

// SET YOUR DEFAULT START AND END TIMES HERE (IN 24 HOUR FORMAT)
#define START_HOUR  9
#define START_MIN   0
#define END_HOUR    10
#define END_MIN     0

void color (unsigned char red, unsigned char green, unsigned char blue);
unsigned long timeToMilli(int hour, int minute);

//Global variables
long unsigned beginTime; //computer manual time minus millis at that point

long unsigned defStart; //default mode start time
long unsigned defEnd; //default mode end time
int mode; //current mode, may make enum
unsigned long modeStart; // current mode start time
unsigned long modeEnd; // current mode end time
bool switchWasOff; //switch was previously off
bool buttonWasOff; //button was off at last time checked
unsigned long timeSincePress; //time since the button was last pressed

CRGB leds[NUM_LEDS];
CRGB defCurrentColor;

CRGB defBaseColor[] = {
CRGB(0,0,0),    // nothing
CRGB(255,0,0),  // red
CRGB(255,0,255),// purple
CRGB(0,0,255),  // blue
CRGB(0,255,255),// turquioise
CRGB(0,255,0),  // green
CRGB(255,255,0),// yellow
CRGB(255,0,0)}; // red
//CRGB(0,0,0)}; // nothing - not needed

int defDirection[] = {1,1,-1,1,-1,1,-1,-1};

CRGB::HTMLColorCode defChanging[] = {
  CRGB::Red,
  CRGB::Blue,
  CRGB::Red,
  CRGB::Green,
  CRGB::Blue,
  CRGB::Red,
  CRGB::Green,
  CRGB::Red
};


// An array generated by the lovely people at Adafruit, where each index, 0-255,
// representing the theoretically desired proportion of colour,
// holds an integer value of what the value actually needs to be to achieve the expected result.
// See https://learn.adafruit.com/led-tricks-gamma-correction/the-quick-fix for more info.

const uint8_t PROGMEM gamma8[] = {
  // this array is stored in program storage space,
  // rather than dynamic memory, because it is relatively large
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
  2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
  5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
  10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
  17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
  25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
  37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
  51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
  69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
  90, 92, 93, 95, 96, 98, 99, 101, 102, 104, 105, 107, 109, 110, 112, 114,
  115, 117, 119, 120, 122, 124, 126, 127, 129, 131, 133, 135, 137, 138, 140, 142,
  144, 146, 148, 150, 152, 154, 156, 158, 160, 162, 164, 167, 169, 171, 173, 175,
  177, 180, 182, 184, 186, 189, 191, 193, 196, 198, 200, 203, 205, 208, 210, 213,
  215, 218, 220, 223, 225, 228, 231, 233, 236, 239, 241, 244, 247, 249, 252, 255
};

void setup() {
  Serial.begin(9600);
  while(!Serial){
    //wait for usb to connect
  }

  Serial.println("Please tell me the time (in 24h format with a space, like hh mm):");
  
  while(Serial.available() <= 0){
    //wait for user input
   }
  
  int incomingHour = Serial.parseInt();
  int incomingMin = Serial.parseInt();
  
  Serial.print("I received: ");
  Serial.print(incomingHour);
  Serial.print(" hours and ");
  Serial.print(incomingMin);
  Serial.println(" minutes");

  Serial.print(millis());
  Serial.println(" milliseconds have passed");   
  
  
  beginTime = timeToMilli(incomingHour, incomingMin) - /*milli now*/ millis(); //calculates time the board was powered on, in milliseconds since midnight
  defStart = timeToMilli(START_HOUR,START_MIN);
  defEnd = timeToMilli(END_HOUR,END_MIN);
  defCurrentColor = CRGB(0,0,0);
  
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS)
    .setCorrection(TypicalLEDStrip)
    .setDither(BRIGHTNESS < 255);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

  pinMode(SLIDE, INPUT_PULLUP); //onboard slide switch

  Serial.println("waiting...");
  delay(1000);
  color(0,0,0);
  Serial.println("lights off");
}


void loop()
{
  if(digitalRead(SLIDE) == LOW && switchWasOff) return;
  else if(digitalRead(SLIDE) == LOW && !switchWasOff)
  {
    //tidy up and turn things off
    Serial.println("turned off");
    switchWasOff = true;
    color(0,0,0);
    return;
  }
  else if(digitalRead(SLIDE) == HIGH && switchWasOff)
  {
    Serial.println("I am on");
    switchWasOff = false;
  }

  //TODO: MODE CHECKS

  updateLights();
}

void updateLights(){
  
  //calculate new values
  CRGB newColor = defaultModeUpdate();
  
  //if they are different than old values, update lights
  if (defCurrentColor != newColor)
  {
    Serial.println("send new color");
    color(newColor.red,newColor.green, newColor.blue);
    defCurrentColor = newColor;
  }
}

CRGB defaultModeUpdate()
{
  unsigned long timeNow = (beginTime + millis()) % timeToMilli(24,0);
  if (timeNow <= defStart || defEnd <= timeNow) 
  {
    return CRGB(0,0,0); //if outside range
  }
  //Serial.println("in time range");
  unsigned long slot = map(timeNow,defStart,defEnd,0,8);
  //Serial.println(slot);
  unsigned long slotLow = map(slot,0,8,defStart,defEnd);
  unsigned long slotHigh = map(slot+1,0,8,defStart,defEnd) - 1;
  unsigned long delta = map(timeNow,slotLow,slotHigh,0,255);

  //TODO: TIDY WITH ARRAY AND/OR ENUM
  int addRed = 0;
  int addGreen = 0;
  int addBlue = 0;
  
  if (defChanging[slot] == CRGB::Red) addRed = delta*defDirection[slot];
  else if (defChanging[slot] == CRGB::Green) addGreen = delta*defDirection[slot];
  else if (defChanging[slot] == CRGB::Blue) addBlue = delta*defDirection[slot];
  
  return CRGB(defBaseColor[slot].red + addRed,
              defBaseColor[slot].green + addGreen,
              defBaseColor[slot].blue + addBlue);
}


void color (unsigned char red, unsigned char green, unsigned char blue)
{
  CRGB color = CRGB ( pgm_read_byte(&gamma8[red]),  pgm_read_byte(&gamma8[green]),  pgm_read_byte(&gamma8[blue]));
  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = color;
  }
  FastLED.show();
}

unsigned long timeToMilli(int hour, int minute)
{
  return (unsigned long)hour*3600000 + (unsigned long)minute*60000;
}

