# SIKTEC-Keys
Light weight library to easily use the siktec keypad shield.

#### Description
The library integrates the shield with any AVR board and enables detecting keypress (without debouncing problems) with easy to use definition and code.

## Features
- Single key press and multi key combinations.
- Easy callback assignment to key and combination.
- KeyEvent object with common methods to easily interact and use the keypad.
- Dynamic Enable / Disable states.
- Dynamic sensitivity settings for better multi key press detection.
- Initialize pin definition and interrupt handler.
- Great well documented Examples.

## Installation:
You can install the library via one of the following:
1. Arduino library manager: Search for "SIKTEC Keys" and click install.
2. Download the repositories as a ZIP file and install it on the Arduino IDE by:
	`Sketch -> Include library -> Add .ZIP Library.`
3. Download the library and include it in your project folder - Than you can Include it:
	`#include `{path to}\SIKTEC_Keys.h`

## Example included:
The example included with this library demonstrates how to declare and initialize the keypad, control the sensitivity assign callbacks and use the `KeyEvent` object which is created for each keypress.
The code is well documented and should be easy to build upon and extend the functionality. 


## Initialization of SIKTEC_Keys
This is the full constructor which should be used if you change the shield pins. This way you can
use other pins with different boards.
```c_cpp
SIKtec::SIKTEC_Keys keypad = SIKtec::SIKTEC_Keys(
	true,       // combinations support enabled
	latchPin,   // latch pin number - default is A2
	dataPin,    // data pin number - default is A0
	clockPin,   // data pin number - default is A1
	interPin    // interupt pin - default is 2
);
```
For AVR board (UNO, MEGA, LEONARDO etc) when connected directly you can use the shorter constructor:
```c_cpp
SIKtec::SIKTEC_Keys keypad = SIKtec::SIKTEC_Keys(true /* combinations support enabled */);
```