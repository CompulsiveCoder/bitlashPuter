#include <PS2Keyboard.h>
#include "bitlash.h"
#include <string.h>

#include <Adafruit_GFX.h>    // Core graphics library
#include "SWTFT.h" // Hardware-specific library

#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

// uno
//const int keyboardDataPin = 3;
//const int keyboardIrqPin =  2;

// mega
const int keyboardDataPin = 21;
const int keyboardIrqPin =  20;

PS2Keyboard keyboard;

SWTFT tft;

int lineNumber = 0;
int columnNumber = 0;

bool isVerbose = false;

int lineHeight = 10;
int columnWidth = 6;
float textSize = 1;

String currentLineText = "";

void setup(void) {

  delay(1000);

  initBitlash(9600);

  setOutputHandler(&serialHandler);

  keyboard.begin(keyboardDataPin, keyboardIrqPin);

  beginTFT();

  writeLine("Starting terminal...");
  newLine();
  writeChar('>');
  writeChar(' ');
}

void loop(void) {

  runBitlash();

  if (keyboard.available()) {

    bool isSpecial = false;

    char c = keyboard.read();

    if (c == PS2_ENTER) {
      Serial.println();
      newLine();
    } 
    else if (c == PS2_TAB) {
      Serial.print("[Tab]");
    } 
    else if (c == PS2_ESC) {
      Serial.print("[ESC]");
    } 
    else if (c == PS2_PAGEDOWN) {
      Serial.print("[PgDn]");
    } 
    else if (c == PS2_PAGEUP) {
      Serial.print("[PgUp]");
    } 
    else if (c == PS2_LEFTARROW) {
      Serial.print("[Left]");
    } 
    else if (c == PS2_RIGHTARROW) {
      Serial.print("[Right]");
    } 
    else if (c == PS2_UPARROW) {
      Serial.print("[Up]");
    } 
    else if (c == PS2_DOWNARROW) {
      Serial.print("[Down]");
    } 
    else if (c == PS2_DELETE) {
      Serial.print("[backspace]");
      backspace();
    } 
    else {
      // Serial.print(c);
    }

    doCharacter(c);
  }

}

void writeLine(char* text)
{
  writeText(text);
  newLine();
}

void writeChar(char character)
{
  if (isVerbose)
  {
    Serial.println("");
    Serial.print("Writing character to screen: '");
    Serial.print(character);
    Serial.print("' (");
    Serial.print(character, HEX);
    Serial.println(")");
    Serial.println("");
  }
  tft.setCursor(columnNumber*columnWidth, lineNumber*lineHeight);
  tft.print(character);
  columnNumber ++;

  if (character != 0x20)
    currentLineText += character;

  if (isVerbose)
    outStatus();
}

void writeText(char* text)
{
  tft.fillScreen(BLACK);
  tft.setCursor(columnNumber*columnWidth, lineNumber*lineHeight);
  tft.print(text);
  columnNumber += strlen(text);
  currentLineText += text;

  if (isVerbose)
    outStatus();
}

void beginTFT()
{
  tft.reset();

  uint16_t identifier = tft.readID();

  Serial.print(F("LCD driver chip: "));
  Serial.println(identifier, HEX);

  tft.begin(identifier);
  tft.fillScreen(BLACK);
  tft.setTextColor(WHITE);
  tft.setTextSize(textSize);
}

void newLine()
{
  lineNumber ++;
  columnNumber = 0;
  if (isVerbose)
    Serial.println("New line");
  currentLineText = "";
}

void outStatus()
{

  Serial.print("Line number: ");
  Serial.print(lineNumber);
  Serial.print("  Column number: ");
  Serial.println(columnNumber);
}

void serialHandler(byte b) {
  if (b != 8)
  {
    writeChar(char(b));
  }
  if (b == '\n')
  {
    newLine();
    newLine();
  }
}

void backspace()
{
  char previousCharacter = currentLineText[currentLineText.length() -1];

  if (isVerbose)
  {
    Serial.println();
    Serial.println("Backspace...");

    Serial.print("Current line text before: '");
    Serial.print(currentLineText);
    Serial.println("'");
    Serial.print("Current line length before: ");
    Serial.println(currentLineText.length());

    Serial.print("Previous character: ");
    Serial.println(previousCharacter);
  }

  tft.setCursor((columnNumber-1)*columnWidth, lineNumber*lineHeight);
  tft.setTextColor(BLACK);
  tft.print(previousCharacter);
  columnNumber -= 2;
  tft.setTextColor(WHITE);

  currentLineText = currentLineText.substring(0, currentLineText.length() - 1);
  currentLineText.trim();

  if (isVerbose)
  {
    Serial.println();
    Serial.print("Current line text after: '");
    Serial.print(currentLineText);
    Serial.println("'");
    Serial.print("Current line length after: ");
    Serial.println(currentLineText.length());
    Serial.println();
  }
}


