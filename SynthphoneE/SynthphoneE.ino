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
#include "tilesheets.h"

void PlayTone(int, int);
void PlayTone(int);
void StopTone();
int Selector();
void Ring();
void DoEncoder();
void DrawSynthValues(String, String, String, int, int);

const byte ROWS = 4; // rows
const byte COLS = 3; // columns
#define encoderPinA 2
#define encoderPinB 3
#define encoderBtn A3            
int encoderPos = 0;
int valRotary = 0;
int lastValRotary = 0;
//int maxRotations = 12;

#define AUDIOPIN A1
#define RINGER 6
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
const char *NOTE_NAMES[12] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
double notesMap[9];
const char *noteDisplayMap[9];

const bool MODE_STEPS[7] = {1, 1, 0, 1, 1, 1, 0};
const char *MODE_NAMES[7] = {"Ionian", "Dorian", "Phrygian", "Lydian", "Mixolydian", "Aeolion", "Locrian"};

int octave = 4;
int modeIndex = 0;
int keyIndex = 0;
int volume = 5;

bool recalculateNotes = true;

int ringerDelayTick = 0;
int ringerDelayMax = 5;
bool isRingerOn = false;

enum instrumentMode
{
  PLAY,
  KEY = 9,
  MODE = 10,
  OCTAVE = 11
};
instrumentMode mode = PLAY;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SSD1306_NO_SPLAS true
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

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
  display.drawBitmap(0, 0, TILESHEETS_H::epd_bitmap_SynthphoneE, 128, 32, 1);
  display.display();
  delay(2000); // Pause for 2 seconds


  // Clear the buffer
  display.clearDisplay();
  //TODO:print the defaults
  display.invertDisplay(false);
  //display.drawBitmap(0, 0, TILESHEET_H::epd_bitmap_SynthphoneE_MenuBlank, 128, 32, 1);
  //display.display();
  DrawSynthValues(noteDisplayMap[0], NOTE_NAMES[0], MODE_NAMES[0], 4, 10);

  tone1.begin(AUDIOPIN);
  tone2.begin(AUDIOPIN);

  pinMode(encoderPinA, INPUT_PULLUP);
  pinMode(encoderPinB, INPUT_PULLUP);
  pinMode(encoderBtn, INPUT_PULLUP);
  pinMode(RINGER, OUTPUT);
  attachInterrupt(0, DoEncoder, CHANGE);

  customKeypad.begin();
}

void loop()
{
  // put your main code here, to run repeatedly:
  customKeypad.tick();
  
  
  // if the rotary is greater than some max # of steps reset to 0
  int selectorStatus = Selector();

  // if the button is pressed, run the ringer
  int btn = digitalRead(encoderBtn);
//  Serial.println("button ");
//  Serial.println(btn);
  if(btn == 0)
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
    int registerJump = 0;

    //for each of the other notes on the keypad
    for (int i = 0; i < 9; i++)
    {
      Serial.print("index : ");
      Serial.println(i);
      
      //add the octave modifier (A4 = 440, measure the change)
      int octaveModifier = ( (abs(octave - 4) + registerJump) * 2);
      Serial.print("octave mod : ");
      Serial.println(octaveModifier);

      //set the playable note in the note map
      //add octave modifier if are not in 4th octave
      if(octave > 4 || registerJump)
        notesMap[i] = notes[note_i] * octaveModifier;
      else if(octave < 4 || registerJump)
        notesMap[i] = notes[note_i] / octaveModifier;
      else
        notesMap[i] = notes[note_i];

      noteDisplayMap[i] = NOTE_NAMES[note_i];

      //based on the mode instructions, move a whole step or half step
      note_i += MODE_STEPS[mode_i] ? 2 : 1;

      //restart the cycle
      if(note_i > 11)
      {
        note_i = note_i - 12;
        registerJump = 1;
      }
    
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
    DrawSynthValues(" ", NOTE_NAMES[keyIndex], MODE_NAMES[modeIndex], octave, volume);
    recalculateNotes = false;
  }

  while (customKeypad.available())
  {
    keypadEvent e = customKeypad.read();
    char key = e.bit.KEY;

    int notePressedIndex = ((3 * e.bit.ROW )+ e.bit.COL);
    //handle the event
    if (e.bit.EVENT == KEY_JUST_PRESSED)
    {

      if(notePressedIndex < 9)
      {
        //handle note playing and drawing
        PlayTone(notesMap[notePressedIndex]);
        DrawSynthValues(noteDisplayMap[notePressedIndex], NOTE_NAMES[keyIndex], MODE_NAMES[modeIndex], octave, volume);

      }
      else
      {
        //handle mode switch event
        //9 = KEY, 10 = MODE, 11 = OCTAVE
        mode = instrumentMode(notePressedIndex);
      }
    }
    else if (e.bit.EVENT == KEY_JUST_RELEASED)
    {
      StopTone();
      mode = PLAY;
    }
  //TODO: research if this delay is necesary
  delay(10);
  }
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

void DoEncoder()
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
    Serial.println(isRingerOn);
    digitalWrite(6, isRingerOn ? HIGH : LOW);
    ringerDelayTick = 0;
    isRingerOn = !isRingerOn;
  }
}

void DrawSynthValues(const char *display_note, const char *display_key, const char *display_mode, int display_octave, int display_volume)
{
 display.clearDisplay();
 //draw the blank background
 display.drawBitmap(0, 0, TILESHEETS_H::epd_bitmap_SynthphoneE_MenuBlank, 128, 32, 1);
 display.display();

 display.setTextColor(BLACK);
 display.setTextSize(1);
 
 //display the key
 display.setCursor(20, 0);
 display.print(display_key);

 //display the mode
 //right align the text based on length of the string (each character is 5 pixels wide)
 display.setCursor(100-(strlen(display_mode) * 5), 0);
 display.print(display_mode);

 //display the octave
 display.setCursor(16, 25);
 display.print(display_octave);

 //display the Volume
 display.setCursor(114, 25);
 display.print(display_volume);


 //display the Note
 if( strlen( display_note ) < 2)
 {
   display.setCursor(58, 10);
 }else{
   display.setCursor(50, 10);
 }
 display.setTextSize(2);
 display.print(display_note);

 display.setFont();  
 display.display();

 
 //https://learn.adafruit.com/adafruit-gfx-graphics-library/graphics-primitives

} 
