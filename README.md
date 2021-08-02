# SIKTEC-Keys
Light weight library to easily use the SIKTEC KeyPad shield.

### Description
The library integrates the shield with any AVR board and enables detecting keypress (without debouncing problems) with easy to use callback definitions and code.

#### Table of Contents:
- [Library Features](#features)
- [Quick Installation](#installation)
- [Example Included](#example-included)
- [Initialization of SIKTEC_Keys](#initialization-of-siktec_keys)
- [Key Codes](#all-the-key-codes)
- [Controling Sensitivity](#controling-the-sensitivity)
- [Assigning a Callback](#assigning-a-callback)
- [KeyEvent object](#the-keyevent-object)
- [SIKTEC_Keys Methods](#siktec_keys-methods)
- [Important Notes](#important-notes)

## Features
- Single key press and multi key combinations.
- Easy callback assignment to keys and combinations.
- `KeyEvent` object with helper methods to easily interact and use the keypad.
- Dynamic Enable / Disable states.
- Dynamic sensitivity setting for smooth multi-key press detection.
- Initialize pin definition and interrupt handler.
- Well documented Examples.

## Installation:
You can install the library through one of the following:
1. Arduino library manager: Search for "SIKTEC Keys" and click install.
2. Download the repositories as a ZIP file and install it on the Arduino IDE by:
    `Sketch -> Include library -> Add .ZIP Library.`
3. Download the library and include it in your project folder - Then you can Include it:
    `#include "{path to}\SIKTEC_Keys.h"`

## Example included:
The example included with this library demonstrates how to declare and initialize the keypad, control the sensitivity assign callbacks and use the `KeyEvent` object which is created for each keypress.
The code is well documented and should be easy to build upon and extend the functionality. 

## Initialization of SIKTEC_Keys
This is the full constructor which should be used if you change the shield pins. This way you can
use other pins with different boards.
```cpp
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

## All the key codes:
By default the shield has 7 keys - Keys are mapped to chars by the library;
| Code | Button              | More info |
|:----:|:--------------------|------------|
| 'u'  | Direction UP        |            |
| 'd'  | Direction DOWN      |            |
| 'l'  | Direction LEFT      |            |
| 'r'  | Direction RIGHT     |            |
| 'm'  | Direction MIDDLE    |            |
| 'L'  | Button TOP LEFT     | this button may also be the reset button (jumper on the back of the board) |
| 'R'  | Button TOP RIGHT    |            |
Any key combination may be used ex: ` 'ud', 'mR', 'LRm' `. Note that the key order are not important and maps to the same combination.

## Controling the sensitivity 
When multi keys combination is enabled you can and should adjust the sensitivity of the the multi keys 
captures - Each type of board may need slight adjustments to ensure clean captures of groups of keys.
this setting is basically adjusting the time frame keys will be captured in.
```cpp
//Keypad sensitivity setting [1-50 default 30]:
keypad.sensitivity(30);
```
 *A good practice will be to call it inside the `setup()` function.

## Assigning a callback
By default there is no action (invoked function) when a key is pressed. To assign your custom functions use the `.on()` method.
```cpp
//Register you callbacks:
keypad.on("any", &cb_any_key);
keypad.on("def", &cb_default);
keypad.on("m",   &cb_middle);
keypad.on("u",   &cb_up);
keypad.on("d",   &cb_down);
keypad.on("l",   &cb_left);
keypad.on("r",   &cb_right);
keypad.on("lr",  &cb_left_right);
```
 *A good practice will be to define callbacks inside the `setup()` function.

There are two special callbacks `any` and `def`:
1. **any** - Always executed on any key press or combination. It will be executed before a specific callback if its defined.
2. **def** - Executed on keys or combinations that don't have their own callback assigned.

A callback must return `void` and take one `KeyEvent` argument:
```cpp
//Example of simple callbacks declaration:
void cb_right(SIKtec::KeyEvent event)
{
    Serial.println("Callback right fired");
}
void cb_left_right(SIKtec::KeyEvent event)
{
    Serial.println("Callback left + right fired");
}
```

## The KeyEvent object
The `KeyEvent` object is a small `struct` that stores the event data and exposes some usefull methods:
- **KeyEvent.multi** => boolean, true if multiple keys were pressed otherwise false 
- **KeyEvent.none**  => boolean, true only if any key was pressed otherwise false 
- **KeyEvent.count()** => size_t, return the number of keys in the combination
- **KeyEvent operators [< > >= <=]** => size_t, logical comparison to number of keys pressed
- **KeyEvent.is(char | char\*)** => boolean, check if the keyevent is a specific key or combination
- **KeyEvent == (char | char\*)** => boolean, same as KeyEvent.is.
- **KeyEvent.has(char | char\*)** => boolean, check if the keyevent includes a key or a combination
- **KeyEvent.get(uint8_t i)** => char, returns a specific key in the combination
- **KeyEvent.str(i)** => char*, return the key or combination as a string
- **KeyEvent.add(char)** => void, add a key to the combination.
- **KeyEvent.merge(KeyEvent &source)** => merge two KeyEvent objects.

Here is an example of the `KeyEvent` usage:
```cpp
void cb_any_key(SIKtec::KeyEvent event)
{
    Serial.println(event.str());    // prints teh key combination such as "u" "md" etc
    Serial.println(event.bits, BIN);  // the raw bit as read from the shift register ex 0110000
    Serial.println(event.none); // keys pressed or not?
    Serial.println(event.multi); // key combination or not?
    Serial.println(event.is("m")); // is a middle button press?
    Serial.println(event.is("lr")); // is a left+middle button press?
    Serial.println(event == "lr");  // exactly as the is() method.
    Serial.println(event.has("m")); // combination contains a middle key? so fo middle+left this will be true.
    Serial.println(event > 1); // has more than 1 key in the combination? same goes for < >= <=
}
```

## SIKTEC_Keys Methods:
**Dynamically enable / disable callbacks**:
```cpp
keypad.enable();
keypad.disable();
```
**Reading the keypad input programatically on demand**:
```cpp
SIKtec::KeyEvent key = keypad.read();
```
**Programatically invoking a specific callback**:
```cpp
SIKtec::KeyEvent key();
key.add("m"); 
keypad.invoke("m", key); //returns true if invoked.
```

## Important notes:
1. Keep callbacks short and fast.
2. By default `SIKTEC_Keys` can hold up to 15 callbacks (13 user defined, 2 builtin). if you need more, you can change the defined value `SIKETC_KEYS_MAX_CALLBACKS` in `SIKTEC_Keys.h`.
3. The library is implementing a delay of `250` for debouncing. If you need to adjust that - change the defined value of `SIKETC_KEYS_DEBOUNCE_DELAY` in `SIKTEC_Keys.h`.
