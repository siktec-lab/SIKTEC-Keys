//**************************************************************//
//  Name    : shiftIn Example 1.3                               //
//  Author  : Carlyn Maw                                        //
//  Date    : 25 Jan, 2007                                      //
//  Version : 1.0                                               //
//  Notes   : Code for using a CD4021B Shift Register       //
//          :                                                   //
//****************************************************************

#include <Arduino.h>
#include "SIKTEC_Keys.h"

/***************************************************************************/
/**** Create the keypad instance and initiate it ***************************/
/***************************************************************************/

SIKtec::SIKTEC_Keys keypad = SIKtec::SIKTEC_Keys(true);
//SIKtec::SIKTEC_Keys keypad = SIKtec::SIKTEC_Keys(true /* combinations enabled */, latchPin /* A2 */, dataPin /* A0 */, clockPin /* A1 */, interPin /* 2 */);

/***************************************************************************
 * Setup function - 
 * executed once mainly used for definitions and initialization 
 ***************************************************************************/

void setup() {
    //start serial for debugging
    Serial.begin(9600);
    while (!Serial) { ; } // wait for serial

    //Register you callbacks:
    keypad.on("any", &cb_any_key);
    keypad.on("m",   &cb_middle);
    keypad.on("u",   &cb_up);
    keypad.on("d",   &cb_down);
    keypad.on("l",   &cb_left);
    keypad.on("r",   &cb_right);
    keypad.on("lr",  &cb_left_right);
}


/***************************************************************************
 * Key events callback  - 
 * all function takes KeyEvent parameter and return void
 * keep those short and quick as they block the interupt. 
 ***************************************************************************/
void cb_any_key(SIKtec::KeyEvent event)
{
    Serial.print("Any key Callback, Event - [");
    Serial.print(event.str());
    Serial.print("::");
    Serial.print(event.bits, BIN); 
    Serial.print("] - none? ");
    Serial.print(event.none ? "true" : "false");
    Serial.print(" - multi? ");
    Serial.println(event.multi ? "true" : "false");
}
void cb_middle(SIKtec::KeyEvent event)
{
    Serial.println("Callback middle fired");
}
void cb_up(SIKtec::KeyEvent event)
{
    Serial.println("Callback up fired");
}
void cb_down(SIKtec::KeyEvent event)
{
    Serial.println("Callback down fired");
}
void cb_left(SIKtec::KeyEvent event)
{
    Serial.println("Callback left fired");
}
void cb_right(SIKtec::KeyEvent event)
{
    Serial.println("Callback right fired");
}
void cb_left_right(SIKtec::KeyEvent event)
{
    Serial.println("Callback left + right fired");
}

/***************************************************************************
 * Program logic loop.
 * Note its empty as the SIKTEC_KeyPad uses interupt events.
 ***************************************************************************/
void loop() {

}
