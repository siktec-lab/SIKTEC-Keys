/******************************************************************************/
// Created by: Shlomo Hassid.
// Release Version : 1.0.2
// Creation Date: 2021-06-18
// Copyright 2021, SIKTEC / SIKDEV.
// Source: 
/******************************************************************************/
#ifndef SIKTEC_KEYS_H
#define SIKTEC_KEYS_H

// Arduino versioning.
#if defined(ARDUINO) && ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif

// bperrybap - Thanks for a well reasoned argument and the following macro(s).
// See http://arduino.cc/forum/index.php/topic,142041.msg1069480.html#msg1069480
#ifndef INPUT_PULLUP
    #warning "Using  pinMode() INPUT_PULLUP AVR emulation"
    #define INPUT_PULLUP 0x2
    #define pinMode(_pin, _mode) _mypinMode(_pin, _mode)
    #define _mypinMode(_pin, _mode)     \
    do {							    \
        if(_mode == INPUT_PULLUP) {     \
            pinMode(_pin, INPUT);       \
            digitalWrite(_pin, 1);      \
            }                           \
        if(_mode != INPUT_PULLUP)	{   \
            pinMode(_pin, _mode);	    \
            }                           \
    }while(0)
#endif

namespace SIKtec { //Namespace SIKtec

/***************************************************************************/
/**** DEFAULTS *************************************************************/
/***************************************************************************/

#define SIKETC_KEYS_LATCH_PIN A2
#define SIKETC_KEYS_DATA_PIN A0
#define SIKETC_KEYS_CLOCK_PIN A1
#define SIKETC_KEYS_INTER_PIN 2
#define SIKETC_KEYS_KEYS_ATTACHED 7  // physical keys
#define SIKETC_KEYS_REGISTER_BITS 8  // which is the shift registers bits size
#define SIKETC_KEYS_MAX_CALLBACKS 15  // which is the shift registers bits size
#define SIKETC_KEYS_DEBOUNCE_DELAY 250  // which is the shift registers bits size

/***************************************************************************/
/**** Types to be used *****************************************************/
/***************************************************************************/
/* 
* RotaryRange: Struct for defining counter range.
*/
struct KeyEvent {
    private:
        uint8_t _ki = 0;     //counts how many keys detected
        char _keys[SIKETC_KEYS_KEYS_ATTACHED]; // Key pressed -> mapped
        char _str[SIKETC_KEYS_KEYS_ATTACHED + 1]; // Key pressed -> mapped will hold a null terminated for string like char array

    public:
        unsigned char bits; // Raw reading bits
        bool multi;         // Multikey press flag. 
        bool none;          // No keypress flag
        size_t  count() {
            return this->_ki;
        }
        bool  is(const char kc) {
            return this->_ki == 1 && this->_keys[0] == kc;
        }
        bool  is(const char* kcs) {
            return this->has(kcs) && strlen(kcs) == this->_ki;
        }
        bool has(const char kc) {
            for (uint8_t i = 0; i < this->_ki; i++)
                if (this->_keys[i] == kc) return true;
            return false;
        }
        bool has(const char* kcs) {
            char * p;    
            for (p = kcs; *p != '\0'; p++)
                if (!this->has(*p)) 
                    return false;
            return true;
        }
        void merge(KeyEvent &source) {
            for (uint8_t i = 0;  i < source.count(); i++) {
                char k = source.get(i);
                if (k == '\0') break;
                this->add(k);
            }
            this->bits = this->bits | source.bits;
        }
        void add(const char kc) {
            if (this->has(kc)) return; // avoid multiple keys of the sme code
            this->_keys[this->_ki++] = kc;
            if (this->none) 
                this->none = false;
            if (!this->multi && this->_ki > 1) 
                this->multi = true;
        }
        char* str() {
            memcpy(this->_str, this->_keys, SIKETC_KEYS_KEYS_ATTACHED);
            this->_str[this->_ki] = '\0';
            return this->_str;
        }
        char get(uint8_t i) {
            if (i < this->_ki) 
                return this->_keys[i];
            return '\0';
        }
        //operators overload
        bool operator == (const char kc) const {
            return this->is(kc);
        }
        bool operator == (const char* kcs) const {
            return this->is(kcs);
        }
        bool operator != (const char kc) const {
            return !this->is(kc);
        }
        bool operator != (const char* kcs) const {
            return !this->is(kcs);
        }
        bool operator >= (const size_t size) const {
            return this->_ki >= size;
        }
        bool operator <= (const size_t size) const {
            return this->_ki <= size;
        }
        bool operator > (const size_t size) const {
            return this->_ki > size;
        }
        bool operator < (const size_t size) const {
            return this->_ki < size;
        }
        //Constructors:
        KeyEvent(unsigned char b, bool m = false, bool n = true) : bits(b), multi(m), none(n) {

        }
};



struct callback_pair {
    uint32_t key;
    void (*cb)(KeyEvent);
    callback_pair(): key(0), cb(nullptr) {}
    callback_pair(uint32_t k, void (*_cb)(KeyEvent)): key(k), cb(_cb) {}
};
/***************************************************************************/
/**** SikRot CLASS *********************************************************/
/***************************************************************************/

class SIKTEC_Keys {
    
    private:
        struct {
            uint8_t latch; 
            uint8_t data; 
            uint8_t clock; 
            uint8_t inter; 
        } pins;
        unsigned char buffer = 0b00000000; // Holds the shift register bits.
        inline static const char keyCodes[] = {
            'u', 
            'l', 
            'd', 
            'r', 
            'm', 
            'L', 
            'R', 
            'x'
        };
        callback_pair callbacks[SIKETC_KEYS_MAX_CALLBACKS];
        uint8_t registered = 0;
        bool active = true;
        bool multi;
        uint8_t _sensitivity = 30; /* 1-50 */
    public:
        volatile int32_t lastDebounceTime;               // Stores the last time in milli seconds a tick happend
        volatile inline static int32_t debounceDelay;    // Defines the debounce delay to be use    
        //static reflectedMethod reflect;
        static SIKTEC_Keys* instance;
        SIKTEC_Keys(bool allowMulti);
	    SIKTEC_Keys(bool allowMulti, uint8_t latchPin, uint8_t dataPin, uint8_t clockPin, uint8_t interPin);
        /*
        * enable: enables the keypad callbacks
        * ----------------------------
        *   returns: void 
        */
        void enable();
        /*
        * disable: disables the keypad callbacks
        * ----------------------------
        *   returns: void 
        */
        void disable();

        void sensitivity(uint8_t s);
        /*
        * read: reads keypad and return a KeyEvent object
        * ----------------------------
        *   returns: KeyEvent 
        */
        KeyEvent read();

        void on(const char* key, void (*cb)(KeyEvent));

        bool invoke(const char* key, KeyEvent &event);
        /*
        * ntDelay: nonblocking delay:
        * ----------------------------
        *   0.1s == 100000 micro seconds
        *   uint8_t t => number of 0.1 seconds to delay
        *   returns: void 
        */
        inline static void ntDelay(uint8_t t) {
            for (byte i = 0; i < t; i++) delayMicroseconds(100000);
        }
    private:
        /*
        * setPinModes: sets pin modes which were set by the constructor
        * ----------------------------
        *   returns: void 
        */
        void setPinModes();
        /*
        * read_shift: fills the buffer with shift register bits
        * ----------------------------
        *   returns: void 
        */
        void readShift();
        /*
        * hashKey: helper to keep key combination sorted
        * ----------------------------
        *   returns: uint32_t 
        */
        uint32_t hashKey(const char key);
        uint32_t hashKey(const char* key);
        static void isr();

};

} //END Namespace SIKtec

#endif