#include "Adafruit_Keypad.h"

const byte ROWS = 4; // rows
const byte COLS = 3; // columns

// define the symbols on the buttons of the keypads
#define R1 4  // 1
#define R2 5  // 2
#define R3 11 // 7
#define R4 A0 // 6
#define C1 7  // 3
#define C2 8  // 4
#define C3 12 // 8

char keys[ROWS][COLS] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'*', '0', '#'}};
byte rowPins[ROWS] = {R1, R2, R3, R4}; // connect to the row pinouts of the keypad
byte colPins[COLS] = {C1, C2, C3};     // connect to the column pinouts of the keypad

// initialize an instance of class NewKeypad
Adafruit_Keypad customKeypad = Adafruit_Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup()
{
  Serial.begin(9600);

  customKeypad.begin();
}

void loop()
{
  // put your main code here, to run repeatedly:
  customKeypad.tick();

  while (customKeypad.available())
  {
    keypadEvent e = customKeypad.read();
    char key = e.bit.KEY;

    //gets the note index (there are 3 per col)
    int notePressedIndex = ((3 * e.bit.ROW )+ e.bit.COL);

    //handle the event
    if (e.bit.EVENT == KEY_JUST_PRESSED)
    {

      //handle note playing and drawing
      Serial.println(notePressedIndex);
      Serial.println("Pressed");

    }
    else if (e.bit.EVENT == KEY_JUST_RELEASED)
    {
      //Serial.println(notePressedIndex);
      //Serial.println("Released");
    }

  delay(10);
  }
}
