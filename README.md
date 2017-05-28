12/17/13

This is code for an IR Keyboard used to perform YouTube searches on a LG Blu-Ray player.  

The hardware for this projects consists of a PS/2 Keyboard, Teensy 2.0, LCD screen and an IR LED triggered through a transistor, the power source is a 9V battery.

The code was written for the Arduino IDE with Teensyduino (http://www.pjrc.com/teensy/teensyduino.html).  It uses the LiquidCrystal, PS2Keyboard and IRremote libraries (see http://www.pjrc.com/teensy/td_libs.html).


This software was written specifically for the YouTube search grid on a LG Blu-Ray player, it would have to be modified to work on a search gird with a different layout.  See the project page for a picture of the grid layout.

When powered on the IR Keyboard expects the curser on the television to be somewhere in the search grid.  The cursor will be placed on the latter 'A' after power on so that it is in a known place.


To use the IR Keyboard, type a search term and hit the Enter key.  The proper IR commands will be sent to the Blu-ray to spell the term.  Characters typed are displayed on the LCD and the usual editing keys such as backspace, delete and the arrow keys may be used.  Any characters that do not appear on the search grid ("#', for example) are ignored.


Complete accuracy cannot be guaranteed, since there is no feedback in IR transmission.  If one wrong character is clicked the program will never correct itself, since it does not truly know where the cursor is on the television.


Some keys have special functions:


F1 :  Enter direct control mode

When in direct control mode the television remote control is mimicked with the left, right, up and down arrow keys and the Enter key.  This mode can be used to select from search results without having to use the remote control.

Click F1 again to exit direct control mode


F2 : clear TV search field

The search field on the television is cleared, the text on the LCD is left in tact.


ESC : clears search term or stops sending IR codes

If ESC is pressed when entering a search term any text on the LCD is erased.

If ESC is pressed when the IR Keyboard is sending IR commands the send is aborted and the LCD text is left in tact.  This is useful if the IR commands get out of sync so that gibberish is being typed on the TV.  In this case F2 can be used to clear the TV search field and then Enter will attempt to send the search term again.

![IR-KEYBOARD](https://cloud.githubusercontent.com/assets/318132/26525661/756678b2-432c-11e7-946c-709f5223df4f.jpg?raw=true "IR Keyboard")

![IR-KEYBOARD-LCD](https://cloud.githubusercontent.com/assets/318132/26525662/788ff09a-432c-11e7-8ed6-89ca7e1c3bf3.jpg "IR Keyboard LCD")
