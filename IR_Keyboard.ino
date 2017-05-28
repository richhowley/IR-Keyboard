
/*
    IR Keyboard for LG Blu-ray.
 
 LG key codes: http://lirc.sourceforge.net/remotes/lg/AKB72915207
 
 IR library: http://www.arcfn.com/2009/08/multi-protocol-infrared-remote-library.html
 */



#include <IRremote.h>
#include "irkeyboard.h"

#include <LiquidCrystal.h>
#include <PS2Keyboard.h>

// IR interface
IRsend irsend;

const int GRID_WIDTH = 8;
const int GRID_HEIGHT = 7;

int cursorRow;  // cursor postion in grid
int cursorCol;


// PS/2 interface
const int DataPin = 7;
const int IRQpin =  8;

// cursor size
const int lcdRows = 4;
const int lcdCols = 20;

// keyboard buffer
char searchBuffer[(lcdRows*lcdCols)+1];

PS2Keyboard keyboard;  // keyboard object

// point at buffer
char *sPtr = searchBuffer;

// create LCD object
//LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
LiquidCrystal lcd(2, 3, 4, 5, 6, 9);

boolean remoteMode = false;    // flag for manual operation via arrows and enter

/*
  sendCmd
 
 Send passed commend to IR reciever.
 
 */
void sendCmd(COMMAND cmd)
{
  // combine pre data with specific code
  irsend.sendNEC(PRE_DATA | (long)(tv_codes[cmd] & 0xffff),32);

  // we must move deliberatly to avoid missed clicks
  delay(300);

}

/*
  posAtA
 
 Postions cursor at the 'a' character.  Call in startup to position at a known place.
 
 We assume that the cursor is somewhere in the 
 search grid and there is no wrapping at the grid edges.
 
 */
void posAtA()
{
  int i;


  // move all the way left
  for(i=0; i < GRID_WIDTH; i++) sendCmd(left);
  delay(40);

  // move all the way down
  for(i=0; i < GRID_HEIGHT; i++) sendCmd(down);
  delay(40);

  // now move up to 'a'
  for(i=0; i < (GRID_HEIGHT- 2); i++) sendCmd(up);
  delay(40);

  // init current posiiton
  cursorRow = 1;
  cursorCol = 0;

}



/*
  posAtChar
 
 Position at character in passed row, column
 */
void posAtChar(int gridRow, int gridCol)
{
  // posiiton at proper row
  while( cursorRow != gridRow )
  {
    if( cursorRow > gridRow ) 
    {
      sendCmd(up);
      cursorRow--;
    } 
    else {
      sendCmd(down);
      cursorRow++;
    } // else 

  } // while

  // posiiton at proper column
  while( cursorCol != gridCol )
  {
    if( cursorCol < gridCol ) 
    {
      sendCmd(right);
      cursorCol++;
    } 
    else {
      sendCmd(left);
      cursorCol--;
    } // else 

  } // while

}

/* typeChar
 
 Type character at passed positon in grid.
 
 */
void typeChar(int gridRow, int gridCol)
{
  // position at character
  posAtChar(gridRow, gridCol);

  // choose character
  sendCmd(ok);
  delay(40);

}


const int quoteval = '"';  // compiler blows up if '"" is in array
int specialChars[] = {
  '.',',','_','-','\'',quoteval,'/','&','@'};

/*
    checkForSpecialChar
 
 If passed char is a special character (appears after '9' on screen)
 return its grid posiiton, else return -1.
 */
int checkForSpecialChar(char c)
{
  int retVal = (-1);

  // while more special chars
  for(int idx=0; specialChars[idx]; idx++)
  {

    // if this is the special char
    if( c == specialChars[idx] )
    {

      // set position in grid
      retVal = 'z' - 'a' + '9' - '0'  + idx + 2;

      // stop looking
      break;

    } // if

  } // for

  return(retVal);

} 

//
// getGridPos
//
// Return char position on screen grid or -1 for illegal character.
//
int getGridPos(char c)
{
  int gridPos = (-1);    // init to illegal value

  // if this is a letter
  if( ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) )
  { 

    // calculate grid position
    ((c >= 'a') && (c <= 'z')) ? gridPos = c - 'a' : gridPos = c - 'A';

  } 
  else if( (c >= '0') && (c <= '9') ) {
    // if this is a number

    // numbers are after letters on grid
    gridPos = ('z' - 'a') + (c - '0') + 1;

  } 
  else {

    // this character might be a special character
    gridPos = checkForSpecialChar(c);  

  } // else

  return(gridPos);
}

// stringToLCD
//
// Print string to LCD begining at passed location in searchBuffer
//
void stringToLCD(char *p)
{
  
  byte colNo;
  byte rowNo;
  
  // while more charactres
  while( *p )
  {
    colNo = (p - searchBuffer) % lcdCols;
    rowNo = (p - searchBuffer) /lcdCols;
    lcd.setCursor(colNo, rowNo);
    lcd.print(*p++);
   
  } // while
  
  
}

/* restoreLCD
 
 A status message has been on screen, clear it and write the 
 search buffer.
 
 */
void restoreLCD()
{
  // print buffer to lcd
  lcd.clear();
 // lcd.print(searchBuffer);
  stringToLCD(searchBuffer);

  // Turn on the cursor:
  lcd.cursor();
  lcd.blink();

  // restore cursor position
  lcd.setCursor((sPtr-searchBuffer) % lcdCols, (sPtr-searchBuffer) / lcdCols);

}


/*
    processBuffer
 
 Write ir coammnds to spell contents of buffer.
 */
void processBuffer()
{
  char *c = searchBuffer;    // point at buffer
  int gridPos;               // offset from letter 'a'
  int gridRow, gridCol;

  // while more characters in buffer
  while( *c )
  {
    // get position on grid
    gridPos = getGridPos(*c);

    // if we have a legal character
    if( gridPos >= 0 ) {

      // calculate row and column
      gridRow = (gridPos / GRID_WIDTH) + 1;
      gridCol = gridPos % GRID_WIDTH;

      // type the character
      typeChar(gridRow,gridCol);

    } 
    else {
      // must be a top row character
      if( *c == ' ') 
      {
        // first position at 'b'
        gridRow = 1; 
        gridCol = 1; 
        posAtChar(gridRow, gridCol);             // positon at a
        sendCmd(up);          // up one
        sendCmd(ok);          // type it
        sendCmd(down);        // back to grid

        // down one from space is 'b'
        cursorRow = 1;
        cursorCol = 1;

      } // if

    } // else

    // advance pointer
    c++;

    // if there is a keystroke available 
    if (keyboard.available()) {

      char inChar;

      // read it
      inChar = keyboard.read();

      // if escape was clicked, bail out
      if( inChar == PS2_ESC )
      {

        // write message to LCD
        lcd.clear();
        lcd.noCursor();
        lcd.print("Quitting and resetting ...");

        // position at known place
        posAtA();

        // back to buffer on LCD
        restoreLCD();

        break;
      } // if

    } // if


  } // while

}




// insertChar
//
// Insert passed character into search buffer.
//
void insertChar(char c)
{

  // if at end of buffer
  if( !*sPtr )
  {
    // if buffer is full
    if( (sPtr - searchBuffer) == (lcdRows * lcdCols) )
    {
      // overwrite last character
      *(sPtr-1) = c;

    } 
    else {

      // insert into buffer, increment pointer
      *sPtr++ = c;

    } // else

  } 
  else {

    // shift right!
    char *bufStart, *bufEnd;

    // init pointers insert position
    bufStart = bufEnd = sPtr;

    // find end of buffer
    while( *bufEnd ) bufEnd++;

    // if buffer is full, overwrite last char
    if( (bufEnd - searchBuffer) == (lcdRows * lcdCols) ) bufEnd--;

    // copy chars
    while( bufEnd > bufStart ) 
    { 
      // copy one character
      *bufEnd = *(bufEnd - 1);

      // move end pointer left
      bufEnd--;

    } // while

    // insert character
    *sPtr++ = c;

  } // else

}

// shiftLeft
//
// Shift all chars in search buffer left.
// Update string on LCD.
//
void shiftLeft()
{
  char *p1, *p2;

  // copy characters one slot left
  for(p2=sPtr,p1=sPtr-1; *p2; p1++, p2++) *p1 = *p2;

  // terminate string
  *p1 = '\0';

  // move pointer left
  if( sPtr > searchBuffer ) --sPtr;

  // lcd clear
  lcd.clear();

  // print on LCD
  //char *p = searchBuffer;
  //while( *p ) lcd.print(*p++);
  stringToLCD(searchBuffer);

  // position cursor
  lcd.setCursor((sPtr-searchBuffer) % lcdCols, (sPtr-searchBuffer) / lcdCols);

}

// setup
//
//
void setup() {

  delay(1000);

  // startup keyboard
  keyboard.begin(DataPin, IRQpin);
  Serial.begin(9600);

  // startup LCD
  lcd.begin(lcdCols, lcdRows);

  delay(1000);

  // wait for character
  lcd.print("  Any Key to Begin ");
  while( !keyboard.available() );

  // read ikeyt
  char c = keyboard.read();

  lcd.clear();  
  lcd.print("One Moment ...");

  // position at known place in search window
  posAtA();


  lcd.clear();

  // Turn on the cursor:
  lcd.cursor();
  lcd.blink();

  // place curser at first char of first row
  lcd.setCursor(0,0);

}

char c;

void loop() {

  // if there is a keystroke available 
  if (keyboard.available()) {

    // read it
    char c = keyboard.read();

    // if not in remote mode
    if( !remoteMode )
    {

      // if this is a legal character
      if( (c == ' ') || (getGridPos(c) != (-1)) )
      {

        // insert it into buffer
        insertChar(c);

        // print from inserted char to end of string on LCD
        char *p = sPtr-1;
        //while( *p ) lcd.print(*p++);
        stringToLCD(p);

        // if buffer is full
        if( (sPtr - searchBuffer) == (lcdRows * lcdCols) ) 
        {
          // cursor at last char
          lcd.setCursor(lcdCols-1,lcdRows-1);

        } 
        else {

          //  cursor at next char
          lcd.setCursor((sPtr-searchBuffer) % lcdCols, (sPtr-searchBuffer) / lcdCols);

        } // else

      } // if

      // check for some of the special keys
      else if (c == PS2_ENTER) {

        // time to type search criteria
        processBuffer();

      } 
      else if (c == PS2_BACKSPACE) {

        // if we are not at the first character, shift left
        if( sPtr != searchBuffer )
        {
          shiftLeft();
        } // if
      } 
      else if (c ==  PS2_F1 ) {

        // enter  control mode

        // shut off cursor
        lcd.noCursor();
        lcd.noBlink();

        // indicate that we are in direct mode
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Use arrows and enter");
        lcd.setCursor(0,1);
        lcd.print("F1 when done");

        // flip flag
        remoteMode = true;

      }   

      else if (c ==  PS2_F2 ) {

        // clear search field on TV
        posAtChar(1, 2);      // positon at c
        sendCmd(up);          // up one
        sendCmd(ok);          // type it
        sendCmd(down);        // back to grid

        // down one from clear is 'c'
        cursorRow = 1;
        cursorCol = 2;


      }
      else if (c == PS2_TAB) {
        Serial.print("[Tab]");
      } 
      else if (c == PS2_ESC) {

        // set buffer to nulls
        for( sPtr=searchBuffer; *sPtr; sPtr++ ) *sPtr = '\0';

        // point to top of buffer
        sPtr = searchBuffer;

        // clear the LCD
        lcd.clear();

      } 
      else if (c == PS2_PAGEDOWN) {
        Serial.print("[PgDn]");
      } 
      else if (c == PS2_PAGEUP) {
        Serial.print("[PgUp]");
      } 
      else if (c == PS2_LEFTARROW) {

        // if not alreay at top of string
        if( sPtr > searchBuffer )
        {

          // move pointer left 
          sPtr--;

          // move cursor 
          lcd.setCursor((sPtr-searchBuffer) % lcdCols, (sPtr-searchBuffer) / lcdCols);

        } // if


      } 
      else if (c == PS2_RIGHTARROW) 
      {
        // if not at end of string
        if( *sPtr && ((sPtr+1) - searchBuffer) < ((lcdRows*lcdCols)) )
        {

          // move pointer right 
          sPtr++;

          // move cursor 
          lcd.setCursor((sPtr-searchBuffer) % lcdCols, (sPtr-searchBuffer) / lcdCols);

        } // if

      } 
      else if (c == PS2_UPARROW) {

        // if there is a row above the cursor
        if( (sPtr - searchBuffer) / lcdCols )
        {

          // position one row up
          sPtr -= lcdCols;
          lcd.setCursor((sPtr-searchBuffer) % lcdCols, (sPtr-searchBuffer) / lcdCols);

        }  // if


      }
      else if (c == PS2_DOWNARROW) {
        Serial.print("[DOWN ARROW]"); 

        // if there is a row below the cursor
        if( ((sPtr - searchBuffer)/lcdCols) < lcdRows )
        {
          // count chars in buffer
          int len = 0;
          char *p = searchBuffer;
          while( *p++ ) len++;

          // calculate position one row down
          int newPos = (sPtr - searchBuffer) + lcdCols;

          // new position is one row down or end of string
          sPtr = searchBuffer + ((newPos < len) ? newPos : len);
          lcd.setCursor((sPtr-searchBuffer) % lcdCols, (sPtr-searchBuffer) / lcdCols);

        } // if

      }
      else if (c == 10) {
        Serial.print("[^Enter]");
      }

    } 
    else {

      // remote mode - only respond to arrows and enter
      switch( c)
      {
      case  PS2_LEFTARROW:          // up arrow
        sendCmd(left);
        break;

      case  PS2_RIGHTARROW:          // right arrow
        sendCmd(right);
        break;

      case PS2_UPARROW:              // up arrow
        sendCmd(up);
        break;

      case PS2_DOWNARROW:            // down arrow
        sendCmd(down);
        break;

      case PS2_ENTER:                // enter
        sendCmd(ok);
        break;

      case  PS2_F1:                  // end remote mode

        // position at known place on grid
        lcd.clear();
        lcd.print("One Moment ...");
        posAtA();

        // back to normal
        restoreLCD();

        // clear flag
        remoteMode = false;

        break;


      default:
        break;

      } // switch


    } // else

  } // if

}










