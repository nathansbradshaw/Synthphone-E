/// Synthphone-E by Enoch and Nathan Bradshaw
///
///
///
///(if you are going to make spaghetti, at least leave a recipe)


#include "Adafruit_Keypad.h"
#include "Tone.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

void PlayTone(int, int);
void PlayTone(int);
void StopTone();
int Selector();
void Ring();

const byte ROWS = 4; // rows
const byte COLS = 3; // columns

#define encoderPinA 2
#define encoderPinB 3
#define encoderBtn 13
int encoderPos = 0;
int valRotary = 0;
int lastValRotary = 0;
//int maxRotations = 12;

#define AUDIOPIN 13

// define the symbols on the buttons of the keypads
#define R1 4  // 1
#define R2 5  // 2
#define R3 11 // 7
#define R4 10 // 6
#define C1 7  // 3
#define C2 8  // 4
#define C3 12 // 8

//#define F 697
//#define G 770
//#define GS 852
//#define AS 941
//#define D 1209
//#define E 1336
//#define FS 1477


double notes[12] = {261.6256, 277.1826, 293.6648, 311.1270, 329.6276, 349.2282, 369.9944, 391.9954, 415.3047, 440.0000, 466.1638, 493.8833};
const char *NOTE_NAMES[12] = {"C ", "C#", "D ", "D#", "E ", "F ", "F#", "G ", "G#", "A ", "A#", "B "};
double notesMap[9];

const bool MODE_STEPS[7] = {1, 1, 0, 1, 1, 1, 0};
const char *MODE_NAMES[7] = {"Ionian", "Dorian", "Phrygian", "Lydian", "Mixolydian", "Aeolion", "Locrian"};

int octave = 4;
int modeIndex = 0;
int keyIndex = 0;
int volume = 5;

bool recalculateNotes = true;

int ringerDelayTick = 0;
int ringerDelayMax = 10000;
bool isRingerOn = false;

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
  pinMode(encoderBtn, INPUT_PULLUP);
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
  
  // if the rotary is greater than some max # of steps reset to 0
  int selectorStatus = Selector();

  // if the button is pressed, run the ringer
  int btn = digitalRead(encoderBtn);
  if(btn == 1)
  {
    Ring();
  }

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
  int newMenuValue;
  //if we have a change in the selector make the change
  //selector = -1,0, or 1
  if(selectorStatus != 0)
  {

    switch (mode)
    {
    case PLAY:
      /* change the volume */
      newMenuValue = volume + selectorStatus;
      if(newMenuValue > -1 && newMenuValue < 11 )
      {
        volume = newMenuValue;
        Serial.println();
        Serial.print("Volume: ");
        Serial.print(volume);
      }
      break;
    case KEY:
      /* switch betwix thine 12 keys */
      newMenuValue = keyIndex + selectorStatus;
      //mode is an index so this one will be 0 based
      if(newMenuValue > 11)
      {
        newMenuValue = 0;
      }
      else if(newMenuValue < 0)
      {
        newMenuValue = 11;
      }
        Serial.println();
      Serial.print("new KEY: ");
      Serial.print(NOTE_NAMES[newMenuValue]);
      Serial.print(" | ");
      Serial.println(newMenuValue);
      keyIndex = newMenuValue;

      //notify that we need to rebuild the notes map
      recalculateNotes = true;
      break;
    case MODE:
      /* switch betwixed thy 7 modes */
      newMenuValue = modeIndex + selectorStatus;
      //mode is an index so this one will be 0 based
      if(newMenuValue > 6)
      {
        newMenuValue = 0;
      }
      else if(newMenuValue < 0)
      {
        newMenuValue = 6;
      }
      Serial.println();
      Serial.print("new Mode: ");
      Serial.print(MODE_NAMES[newMenuValue]);
      Serial.print(" | ");
      Serial.println(newMenuValue);
      modeIndex = newMenuValue;

      recalculateNotes = true;
      break;
    case OCTAVE:
      /* switch betwix octaves 1-7 */
      newMenuValue = octave + selectorStatus;
      if(newMenuValue > 0 && newMenuValue < 8 )
      {
        octave = newMenuValue;
        Serial.println();
        Serial.print("new Octave: ");
        Serial.println(octave);

        recalculateNotes = true;
      }
      break;

    default:
      break;
    }
  }

  //if we have made changes, lets rebuild our notes map
  if(recalculateNotes)
  {
    //starting on the current key, loop through the notes using the current mode instructions to find the notes to play
    
    //starting on the first note of the key
    int note_i = keyIndex;
    Serial.println();
    Serial.print("note index : ");
    Serial.println(note_i);
    Serial.print("note name : ");
    Serial.println(NOTE_NAMES[note_i]);

    //starting in the current mode
    int mode_i = modeIndex;
    Serial.print("mode index : ");
    Serial.println(mode_i);
    

    //for each of the other notes on the keypad
    for (int i = 0; i < 9; i++)
    {
      Serial.print("index : ");
      Serial.println(i);
      
      //add the octave modifier (A4 = 440, measure the change)
      int octaveModifier = ((octave - 4) * 2);
      Serial.print("octave : ");
      Serial.println(octaveModifier);

      //set the playable note in the note map
      //add octave modifier if are not in 4th octave
      if(octaveModifier > 0)
        notesMap[i] = notes[note_i] * octaveModifier;
      else if(octaveModifier < 0)
        notesMap[i] = notes[note_i] / octaveModifier;
      else
        notesMap[i] = notes[note_i];

      //based on the mode instructions, move a whole step or half step
      note_i += MODE_STEPS[mode_i] ? 2 : 1;

      //restart the cycle
      if(note_i > 11)
        note_i = note_i - 11;
    
      //increment mode
      mode_i++;

      //if we are at the end of the mode loop, go to the start
      if(mode_i > 6)
        mode_i = 0;

      Serial.print("hertz : ");
      Serial.println(notesMap[i]);
      Serial.print("note : ");
      Serial.println(NOTE_NAMES[note_i]);
    }

    recalculateNotes = false;
  }

  while (customKeypad.available())
  {
    keypadEvent e = customKeypad.read();
    char key = e.bit.KEY;
    Serial.print((char)e.bit.KEY);
    if(e.bit.EVENT == KEY_JUST_PRESSED) Serial.println(" pressed");
    //else if(e.bit.EVENT == KEY_JUST_RELEASED) Serial.println(" released");
    switch (key)
    {
    case '1':
      if (e.bit.EVENT == KEY_JUST_PRESSED)
      {
        PlayTone(notesMap[0]);
        //display.invertDisplay(true);
      }
      else if (e.bit.EVENT == KEY_JUST_RELEASED)
      {
        StopTone();
        //display.invertDisplay(false);
      }
      break;
    case '2':
      if (e.bit.EVENT == KEY_JUST_PRESSED)
      {
        PlayTone(notesMap[1]);
        // display.clearDisplay();
        // display.display();
        // display.drawBitmap(0, 0, epd_bitmap_8008INC, 128, 32, 1);
        // display.display();
      }
      else if (e.bit.EVENT == KEY_JUST_RELEASED)
      {
        StopTone();
      }
      break;
    case '3':
      if (e.bit.EVENT == KEY_JUST_PRESSED)
      {
        PlayTone(notesMap[2]);
      }
      else if (e.bit.EVENT == KEY_JUST_RELEASED)
      {
        StopTone();
      }
      break;
    case '4':
      if (e.bit.EVENT == KEY_JUST_PRESSED)
      {
        PlayTone(notesMap[3]);
      }
      else if (e.bit.EVENT == KEY_JUST_RELEASED)
      {
        StopTone();
      }
      break;
    case '5':
      if (e.bit.EVENT == KEY_JUST_PRESSED)
      {
        PlayTone(notesMap[4]);
      }
      else if (e.bit.EVENT == KEY_JUST_RELEASED)
      {
        StopTone();
      }
      break;
    case '6':
      if (e.bit.EVENT == KEY_JUST_PRESSED)
      {
        PlayTone(notesMap[5]);
      }
      else if (e.bit.EVENT == KEY_JUST_RELEASED)
      {
        StopTone();
      }
      break;
    case '7':
      if (e.bit.EVENT == KEY_JUST_PRESSED)
      {
        PlayTone(notesMap[6]);
      }
      else if (e.bit.EVENT == KEY_JUST_RELEASED)
      {
        StopTone();
      }
      break;
    case '8':
      if (e.bit.EVENT == KEY_JUST_PRESSED)
      {
        PlayTone(notesMap[7]);
      }
      else if (e.bit.EVENT == KEY_JUST_RELEASED)
      {
        StopTone();
      }
      break;
    case '9':
      if (e.bit.EVENT == KEY_JUST_PRESSED)
      {
        PlayTone(notesMap[8]);
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

void PlayTone(double note1)
{
  Serial.println();
  Serial.print("Hertz : ");
  Serial.println(note1);
  tone1.play(note1);
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

// Let us know if we are moving up, down or not at all;
int Selector()
{
  int selectorChange = 0;

  //Serial.println(valRotary);
  if (valRotary > lastValRotary)
  {
    Serial.print("  +1");
    selectorChange = 1;
  }
  else if(valRotary < lastValRotary)  {
    Serial.print("  -1");
    selectorChange = -1;
  }
  lastValRotary = valRotary;
  //Serial.println();
  return selectorChange;
}

void Ring()
{
  ringerDelayTick++;

  if(ringerDelayTick > ringerDelayMax)
  {
    ringerDelayTick = 0;
    isRingerOn = !isRingerOn;
    digitalWrite(6, isRingerOn);
  }
}