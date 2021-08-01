
#include "SIKTEC_Keys.h"

namespace SIKtec {// Namespace SIKtec

SIKTEC_Keys *SIKTEC_Keys::instance = nullptr;

SIKTEC_Keys::SIKTEC_Keys(bool allowMulti) {
    this->pins.latch    = SIKETC_KEYS_LATCH_PIN;
    this->pins.data     = SIKETC_KEYS_DATA_PIN;
    this->pins.clock    = SIKETC_KEYS_CLOCK_PIN;
    this->pins.inter    = SIKETC_KEYS_INTER_PIN;
    this->multi         = allowMulti;
    SIKTEC_Keys::instance = this;
    SIKTEC_Keys::debounceDelay = SIKETC_KEYS_DEBOUNCE_DELAY;
    this->lastDebounceTime = millis();
    this->setPinModes();
    
}
SIKTEC_Keys::SIKTEC_Keys(bool allowMulti, uint8_t latchPin, uint8_t dataPin, uint8_t clockPin, uint8_t interPin) {
    this->pins.latch    = latchPin;
    this->pins.data     = dataPin;
    this->pins.clock    = clockPin;
    this->pins.inter    = interPin;
    this->multi         = allowMulti;
    SIKTEC_Keys::instance = this;
    SIKTEC_Keys::debounceDelay = SIKETC_KEYS_DEBOUNCE_DELAY;
    this->lastDebounceTime = millis();
    this->setPinModes();
}
void SIKTEC_Keys::setPinModes() {
    //define pin modes
    pinMode(this->pins.latch,   OUTPUT);
    pinMode(this->pins.clock,   OUTPUT);
    pinMode(this->pins.data,    INPUT);
    pinMode(this->pins.inter,   INPUT);
    digitalWrite(2, LOW);
    attachInterrupt(digitalPinToInterrupt(this->pins.inter), SIKTEC_Keys::isr, RISING);
}
void SIKTEC_Keys::isr() {
    cli();
    if ((millis() - SIKtec::SIKTEC_Keys::instance->lastDebounceTime) > SIKtec::SIKTEC_Keys::debounceDelay && digitalRead(SIKtec::SIKTEC_Keys::instance->pins.inter) == HIGH) {
        SIKtec::SIKTEC_Keys::instance->lastDebounceTime = millis();
        //First get the triggering key:
        SIKtec::KeyEvent key = SIKTEC_Keys::instance->read();
        if (SIKTEC_Keys::instance->active && key > 0) {
            //Delay to pick all keys:
            if (SIKtec::SIKTEC_Keys::instance->multi) {
                SIKTEC_Keys::ntDelay(SIKtec::SIKTEC_Keys::instance->_sensitivity); // wait for other keys
                SIKtec::KeyEvent additional_keys = SIKTEC_Keys::instance->read(); // get additional keys.
                key.merge(additional_keys); //merge them
            }
            //invoke all first:
            SIKtec::SIKTEC_Keys::instance->invoke("any", key);
            //handle callbacks
            if (!SIKtec::SIKTEC_Keys::instance->invoke(key.str(), key)) {
                //handle default callbacks
                SIKtec::SIKTEC_Keys::instance->invoke("def", key);
            }
        }
    }
    sei();
}
void SIKTEC_Keys::sensitivity(uint8_t s) {
    this->_sensitivity = s < 1 ? 1 : s;
}
void SIKTEC_Keys::enable() {
	this->active = true;
}
void SIKTEC_Keys::disable() {
	this->active = false;
}
KeyEvent SIKTEC_Keys::read() {
    this->readShift();
    KeyEvent key(this->buffer);
    //Add keyCodes:
    for (int8_t n = 0; n <= 7; n++) {
        if (this->buffer & (1 << n) ) {
            key.add(SIKTEC_Keys::keyCodes[n]);
            if (!this->multi) break;
        }
    }
    return key;
}
void SIKTEC_Keys::on(const char* key, void (*cb)(KeyEvent)) {
    if (this->registered < SIKETC_KEYS_MAX_CALLBACKS) {
        uint32_t numKey = this->hashKey(key);
        this->callbacks[this->registered++] = callback_pair(numKey, cb);
    }
}
bool SIKTEC_Keys::invoke(const char* key, KeyEvent &event) {
    uint32_t numKey = this->hashKey(key);
    for (uint8_t i = 0; i < SIKETC_KEYS_MAX_CALLBACKS; i++)
        if (this->callbacks[i].key == numKey) {
            (this->callbacks[i].cb)(event);
            return true;
        }
    return false;
}
void SIKTEC_Keys::readShift() {
    this->buffer = 0b00000000;
    digitalWrite(this->pins.latch, OUTPUT); //set it to 1 to collect parallel data
    delay(20);
    digitalWrite(this->pins.latch, INPUT); //set it to 0 to transmit data serially
    pinMode(this->pins.clock, OUTPUT);
    pinMode(this->pins.data,  INPUT);
    for (int8_t i = 7; i >= 0; i--) {
        //set clock as input
        digitalWrite(this->pins.clock, INPUT);
        delayMicroseconds(0.2);
        //read data at this pulse:
        if (digitalRead(this->pins.data)) {
            this->buffer = this->buffer | (1 << i);
        }
        digitalWrite(this->pins.clock, OUTPUT);
    }
}
uint32_t SIKTEC_Keys::hashKey(const char key) {
    return (uint32_t)key*2;
}
uint32_t SIKTEC_Keys::hashKey(const char* key) {
    uint32_t sum   = 0;
    uint32_t multi = 1;
    char * t; 
    for (t = key; *t != '\0'; t++) {
        sum += (uint32_t)*t;
        multi = multi * (uint32_t)*t;
    }
    return multi + sum;
}

}//END Namespace SIKtec