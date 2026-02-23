#include <Servo.h>
#include <math.h>
#include <Adafruit_NeoPixel.h>
#include "constants.h"
#include "coordinates.h"


void turn(float degrees, int turningDirection = 1);
bool move(float distance, bool bounceAtObstacle = true);
bool spiral(double initialRadiusMM, double finalRadiusMM, double revolutions = 1.0, bool clockwise = true, bool stopAtSwitch = true, bool stopAtSensor = false, bool togglePenAtSensor = false);
void drawCoordinates(float scale = 1.0);

//Adafruit_NeoPixel pixels(3, NEOPIXEL, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels(NUM_LEDS, NEOPIXEL, NEO_GRB + NEO_KHZ800);


Servo myservo;           //                                                   servo to lift / lower the pen
uint8_t stepCount1 = 0;  //                                                   global sep counter motor 1
uint8_t stepCount2 = 0;  //                                                   global sep counter motor 2

long delayValue = 2000;  //                                                   current delay between steps
int direction = 1;       //                                                   current moving direction
bool drawing = true;     //                                                    pen position (true = pen is down / false = pen is up)

//=========================================================================== SETUP
void setup() {
  randomSeed(analogRead(A7) + analogRead(A6));
  pixels.setBrightness(32);
  pixels.begin();
  pixels.clear();

  Serial.begin(9600);
  Serial.println("Hi there!");

  pinMode(STEP_1A, OUTPUT);
  pinMode(STEP_1B, OUTPUT);
  pinMode(STEP_1C, OUTPUT);
  pinMode(STEP_1D, OUTPUT);

  pinMode(STEP_2A, OUTPUT);
  pinMode(STEP_2B, OUTPUT);
  pinMode(STEP_2C, OUTPUT);
  pinMode(STEP_2D, OUTPUT);

  pinMode(SERVO, OUTPUT);
  pinMode(SPEAKER, OUTPUT);

  pinMode(SWITCH_F, INPUT_PULLUP);
  pinMode(SWITCH_B, INPUT_PULLUP);

  triTone();
  penUp();
}

//=========================================================================== LOOP
void loop() {
/*
#if 0
  pixels.setPixelColor(0, pixels.Color(255,0,0));
  pixels.setPixelColor(1, pixels.Color(0,255,0));
  pixels.show();

  while(1);
return;
#endif
 */ 
 
  plotText(" base48 ", 15);
 
//Serial.print(digitalRead(SWITCH_F) ? "[F]" : " f ");
//Serial.println(digitalRead(SWITCH_B) ? "[B]" : " b ");
//return;



  // draw a fish:
  // penUp();move(30);drawCoordinates(1.0);penUp();move(20);



  // draw a 10cm square:
 /*penDown();
    move(100);
    turn(90);
    move(100);
    turn(90);
    move(100);
    turn(90);
    move(100);
    turn(90);
  penUp();
  move(50);
*/
}
