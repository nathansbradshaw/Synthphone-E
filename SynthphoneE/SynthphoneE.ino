#include "Adafruit_Keypad.h"
#include "Tone.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
void PlayTone(int, int);
void StopTone();

const byte ROWS = 4; // rows
const byte COLS = 3; // columns

const int analogInPin = A1; // Analog input pin that the potentiometer is attached to

// int sensorValue = 0; // value read from the pot
#define encoderPinA 2
#define encoderPinB 3
//#define encoderBtn 4
int encoderPos = 0;
int valRotary = 0;
int lastValRotary = 0;
int maxRotations = 12;

#define AUDIOPIN 13

// define the symbols on the buttons of the keypads
#define R1 4  // 1
#define R2 5  // 2
#define R3 11 // 7
#define R4 10 // 6
#define C1 7  // 3
#define C2 8  // 4
#define C3 12 // 8

#define F 697
#define G 770
#define GS 852
#define AS 941
#define D 1209
#define E 1336
#define FS 1477

//
int notes[12] = {261.6256, 277.1826, 293.6648, 311.1270, 329.6276, 349.2282, 369.9944, 391.9954, 415.3047, 440.0000, 466.1638, 493.8833};
char *noteNames[12] = {"C ", "C#", "D ", "D#", "E ", "F ", "F#", "G ", "G#", "A ", "A#", "B "};
int octave = 4;

const bool MODE_STEPS[7] = {1, 1, 0, 1, 1, 1, 0};
const char *MODE_NAMES[7] = {"Ionian", "Dorian", "Phrygian", "Lydian", "Mixolydian", "Aeolion", "Locrian"};
enum instrumentMode
{
  PLAY,
  KEY,
  MODE,
  OCTAVE
};
instrumentMode mode = PLAY;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SSD1306_NO_SPLAS true
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#pragma region Logo

// '8008INC', 128x32px
const unsigned char epd_bitmap_8008INC[] PROGMEM = {
    // 'synthophoneTM, 128x32px
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0x9f, 0xff, 0xfe, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf1, 0xff,
    0xf0, 0xff, 0xff, 0xff, 0xcf, 0x9f, 0xff, 0xfe, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfb, 0x5f,
    0xe7, 0x7f, 0xff, 0xff, 0xcf, 0x9f, 0xff, 0xfe, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x3a, 0xaf,
    0xcf, 0xff, 0xff, 0xff, 0xcf, 0x9f, 0xff, 0xfe, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xfa, 0xaf,
    0xcf, 0xe7, 0xce, 0x43, 0x03, 0x90, 0xf2, 0x3e, 0x43, 0xe0, 0xf9, 0x0f, 0x87, 0xf3, 0xff, 0xff,
    0xc7, 0xf3, 0xce, 0x31, 0xcf, 0x8c, 0x71, 0x9e, 0x31, 0xce, 0x78, 0xc7, 0x33, 0xe7, 0xf8, 0x0f,
    0xe3, 0xf3, 0xde, 0x79, 0xcf, 0x9e, 0x73, 0xce, 0x79, 0x9f, 0x39, 0xe6, 0x79, 0xf7, 0xff, 0xff,
    0xf0, 0xf3, 0x9e, 0x79, 0xcf, 0x9e, 0x73, 0xce, 0x79, 0x9f, 0x39, 0xe6, 0x79, 0xf1, 0xff, 0xff,
    0xfc, 0x79, 0x9e, 0x79, 0xcf, 0x9e, 0x73, 0xce, 0x79, 0x9f, 0x39, 0xe6, 0x01, 0x18, 0x3f, 0xff,
    0xff, 0x39, 0xbe, 0x79, 0xcf, 0x9e, 0x73, 0xce, 0x79, 0x9f, 0x39, 0xe6, 0x7f, 0xf1, 0xff, 0xff,
    0xff, 0x3d, 0x3e, 0x79, 0xcf, 0x9e, 0x73, 0xce, 0x79, 0x9f, 0x39, 0xe6, 0x7f, 0xf7, 0xff, 0xff,
    0xff, 0x3c, 0x3e, 0x79, 0xcf, 0x9e, 0x73, 0xce, 0x79, 0x9f, 0x39, 0xe6, 0x7f, 0xe7, 0xff, 0xff,
    0xde, 0x7c, 0x7e, 0x79, 0xcf, 0x9e, 0x71, 0x9e, 0x79, 0xce, 0x79, 0xe7, 0x3d, 0xf3, 0xfc, 0x3f,
    0xe0, 0xfe, 0x7e, 0x79, 0xe3, 0x9e, 0x72, 0x3e, 0x79, 0xe0, 0xf9, 0xe7, 0x83, 0xf0, 0xfb, 0xdf,
    0xff, 0xfe, 0x7f, 0xff, 0xff, 0xff, 0xf3, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x36, 0xcf,
    0xff, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xf3, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xed, 0x4f,
    0xff, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xf3, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xea, 0xcf,
    0xff, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xf3, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xed, 0xaf,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xef, 0x6f,
    0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xef,
    0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfa, 0xff, 0xff, 0xff, 0xff, 0xf8, 0xef,
    0x01, 0x3f, 0xff, 0xff, 0xfe, 0xaa, 0xbd, 0x7f, 0xff, 0x55, 0x5e, 0xbf, 0xff, 0xff, 0xf8, 0xcf,
    0x03, 0x9f, 0xff, 0xff, 0xd5, 0x4a, 0x42, 0xaa, 0xaa, 0xa5, 0x21, 0x55, 0x5f, 0xf0, 0xf8, 0x9f,
    0x07, 0xcf, 0xff, 0xf9, 0xa9, 0x55, 0x48, 0x95, 0x54, 0xaa, 0xa4, 0x4a, 0xaf, 0xe7, 0x71, 0xbf,
    0x0e, 0xe7, 0xff, 0xf6, 0xaa, 0xaa, 0xa5, 0x55, 0x55, 0x55, 0x52, 0xaa, 0xa9, 0xcd, 0x03, 0x7f,
    0x1c, 0x73, 0xff, 0xd5, 0xd5, 0x5f, 0x52, 0xaa, 0xaa, 0xaf, 0xa9, 0x55, 0x5a, 0xd7, 0x07, 0x7f,
    0x38, 0x39, 0xff, 0xa9, 0xab, 0xff, 0xff, 0xd5, 0x55, 0xff, 0xff, 0xea, 0xa1, 0xba, 0x0e, 0xff,
    0x6c, 0x6c, 0xfe, 0xa6, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfd, 0xac, 0xfd, 0xff,
    0xc6, 0xc6, 0x75, 0x4f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x39, 0xfb, 0xff,
    0x83, 0x83, 0x21, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x07, 0xff};

#pragma endregion Logo

char keys[ROWS][COLS] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'*', '0', '#'}};
byte rowPins[ROWS] = {R1, R2, R3, R4}; // connect to the row pinouts of the keypad
byte colPins[COLS] = {C1, C2, C3};     // connect to the column pinouts of the keypad

Tone tone1;
Tone tone2;

// initialize an instance of class NewKeypad
Adafruit_Keypad customKeypad = Adafruit_Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup()
{
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.invertDisplay(true);
  display.drawBitmap(0, 0, epd_bitmap_8008INC, 128, 32, 1);
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.display();
  display.clearDisplay();

  tone1.begin(AUDIOPIN);
  tone2.begin(AUDIOPIN);

  pinMode(encoderPinA, INPUT_PULLUP);
  pinMode(encoderPinB, INPUT_PULLUP);
  // pinMode(encoder0Btn, INPUT_PULLUP);
  attachInterrupt(0, doEncoder, CHANGE);

  customKeypad.begin();
}

void loop()
{

  display.clearDisplay();
  display.invertDisplay(true);
  display.drawBitmap(0, 0, epd_bitmap_8008INC, 128, 32, 1);
  display.display();
  // put your main code here, to run repeatedly:
  customKeypad.tick();

  // sensorValue = analogRead(analogInPin);
  display.drawBitmap(128, 32, epd_bitmap_8008INC, 128, 32, 1);
  display.display();
  Serial.print("sensor = ");
  Serial.print(valRotary);
  Serial.println();
  lastValRotary = valRotary;
  // if the rotary is greater than some max # of steps reset to 0
  if (valRotary > maxRotations)
    valRotary = 0;
  else if (valRotary < 0)
    valRotary = maxRotations;

  int step = 0;
  // TODO: normalize the rotary value
  if (valRotary > lastValRotary)
  {
    step = 1;
  }
  else if (valRotary < lastValRotary)
  {
    step = -1;
  }
  Serial.print("step = ");
  Serial.print(step);
  Serial.println(" ");

  /*
    if we are in play mode,
    read the encoder for volume
    update the screen to say the volume

    if we are in key mode,
    read the encoder for key index switching
    update the screen to say what key we are on

    if we are in mode mode (lol),
    read the encoder for mode index switching
    update the screen to say what mode we are on

    if we are in octave mode,
    read the encoder for octave switching
    update the screen to say the current key with the octave number next to it

    else play notes in current mode, key, and octave
    play notes in the current mode and key,
    update the screen to say what note is paying
  */

  switch (mode)
  {
  case PLAY:

    break;
  case KEY:
    /* code */
    break;
  case MODE:
    /* code */
    break;
  case OCTAVE:
    octave += step;

    break;

  default:
    break;
  }

  while (customKeypad.available())
  {
    keypadEvent e = customKeypad.read();
    char key = e.bit.KEY;
    // Serial.print((char)e.bit.KEY);
    // if(e.bit.EVENT == KEY_JUST_PRESSED) Serial.println(" pressed");
    // else if(e.bit.EVENT == KEY_JUST_RELEASED) Serial.println(" released");
    switch (key)
    {
    case '1':
      if (e.bit.EVENT == KEY_JUST_PRESSED)
      {
        PlayTone(F, D);
        display.invertDisplay(true);
      }
      else if (e.bit.EVENT == KEY_JUST_RELEASED)
      {
        StopTone();
        display.invertDisplay(false);
      }
      break;
    case '2':
      if (e.bit.EVENT == KEY_JUST_PRESSED)
      {
        PlayTone(F, E);
        display.clearDisplay();
        display.display();
        display.drawBitmap(0, 0, epd_bitmap_8008INC, 128, 32, 1);
        display.display();
      }
      else if (e.bit.EVENT == KEY_JUST_RELEASED)
      {
        StopTone();
      }
      break;
    case '3':
      if (e.bit.EVENT == KEY_JUST_PRESSED)
      {
        PlayTone(F, FS);
      }
      else if (e.bit.EVENT == KEY_JUST_RELEASED)
      {
        StopTone();
      }
      break;
    case '4':
      if (e.bit.EVENT == KEY_JUST_PRESSED)
      {
        PlayTone(G, D);
      }
      else if (e.bit.EVENT == KEY_JUST_RELEASED)
      {
        StopTone();
      }
      break;
    case '5':
      if (e.bit.EVENT == KEY_JUST_PRESSED)
      {
        PlayTone(G, E);
      }
      else if (e.bit.EVENT == KEY_JUST_RELEASED)
      {
        StopTone();
      }
      break;
    case '6':
      if (e.bit.EVENT == KEY_JUST_PRESSED)
      {
        PlayTone(G, FS);
      }
      else if (e.bit.EVENT == KEY_JUST_RELEASED)
      {
        StopTone();
      }
      break;
    case '7':
      if (e.bit.EVENT == KEY_JUST_PRESSED)
      {
        PlayTone(GS, D);
      }
      else if (e.bit.EVENT == KEY_JUST_RELEASED)
      {
        StopTone();
      }
      break;
    case '8':
      if (e.bit.EVENT == KEY_JUST_PRESSED)
      {
        PlayTone(GS, E);
      }
      else if (e.bit.EVENT == KEY_JUST_RELEASED)
      {
        StopTone();
      }
      break;
    case '9':
      if (e.bit.EVENT == KEY_JUST_PRESSED)
      {
        PlayTone(GS, FS);
      }
      else if (e.bit.EVENT == KEY_JUST_RELEASED)
      {
        StopTone();
      }
      break;
    case '*':
      if (e.bit.EVENT == KEY_JUST_PRESSED)
      {
        mode = KEY;
      }
      else if (e.bit.EVENT == KEY_JUST_RELEASED)
      {
        mode = PLAY;
      }
      break;
    case '0':
      if (e.bit.EVENT == KEY_JUST_PRESSED)
      {
        mode = MODE;
      }
      else if (e.bit.EVENT == KEY_JUST_RELEASED)
      {
        mode = PLAY;
      }
      break;
    case '#':
      if (e.bit.EVENT == KEY_JUST_PRESSED)
      {
        mode = OCTAVE;
      }
      else if (e.bit.EVENT == KEY_JUST_RELEASED)
      {
        mode = PLAY;
      }
      break;
    default:
      break;
    }
  }

  delay(10);
}

void PlayTone(int note1, int note2)
{
  tone1.play(note1);
  tone2.play(note2);
}

void StopTone()
{
  tone1.stop();
  tone2.stop();
}

void doEncoder()
{
  if (digitalRead(encoderPinA) == digitalRead(encoderPinB))
  {
    encoderPos++;
  }
  else
  {
    encoderPos--;
  }
  valRotary = encoderPos / 2.5;
}