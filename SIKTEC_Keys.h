/******************************************************************************/
// Created by: Shlomo Hassid.
// Release Version : 1.0.2
// Creation Date: 2021-06-18
// Copyright 2021, SIKTEC / SIKDEV.
// Source: https://github.com/siktec-lab/SIKTEC-Keys
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

#define SIKETC_KEYS_LATCH_PIN A2            //default latch pin used by the shield.
#define SIKETC_KEYS_DATA_PIN A0             //default data pin used by the shield.
#define SIKETC_KEYS_CLOCK_PIN A1            //default clock pin used by the shield.
#define SIKETC_KEYS_INTER_PIN 2             //default interupt pin used by the shield.
#define SIKETC_KEYS_KEYS_ATTACHED 7         // number of physical keys - shield has 7
#define SIKETC_KEYS_REGISTER_BITS 8         // the number of pins (bits) that are used on the shift register
#define SIKETC_KEYS_MAX_CALLBACKS 15        // the size of the container which will store callbacks
#define SIKETC_KEYS_DEBOUNCE_DELAY 250      // button debounce delay.
#define SIKETC_KEYS_DEFAULT_SENSITIVITY 30  // sensitivity for combination key pressed.
#define SIKETC_KEYS_DEFAULT_ACTIVE true     // active or disable callback by default.

/***************************************************************************/
/**** Types to be used *****************************************************/
/***************************************************************************/


/*!
* KeyEvent
* ----------------------------
* @method KeyEvent: the object that is created on each keypress and holds the base logic.
* KeyEvent.multi => boolean, true if multiple keys were pressed otherwise false 
* KeyEvent.none  => boolean, true only if any key was pressed otherwise false 
* KeyEvent.count() => size_t, return the number of keys in the combination
* KeyEvent  [< > >= <=] size_t, logical comparison to number of keys pressed
* KeyEvent.is(char | char*) => boolean, check if the keyevent is a specific key or combination
* KeyEvent == (char | char*) => boolean, same as KeyEvent.is.
* KeyEvent.has(char | char*) => boolean, check if the keyevent includes a key or a combination
* KeyEvent.get(uint8_t i) => char, returns a specific key in the combination
* KeyEvent.str(i) => char*, return the key or combination as a string
* KeyEvent.add(char) => void, add a key to the combination.
* KeyEvent.merge(KeyEvent &source) => merge two KeyEvent objects.
*/
struct KeyEvent {
    private:
        uint8_t _ki = 0;                            // internally counts how many keys detected
        char _keys[SIKETC_KEYS_KEYS_ATTACHED];      // Key pressed -> mapped
        char _str[SIKETC_KEYS_KEYS_ATTACHED + 1];   // Key pressed -> mapped will hold a null terminated for string char array

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

/*!
 * callback_pair: internal key pair for registering user defined callbacks 
 * ----------------------------
 * callback_pair.key => uint32_t, the unique id of the key combination 
 * callback_pair.(*cb)(KeyEvent) => the function pointer.
 */
struct callback_pair {
    uint32_t key;
    void (*cb)(KeyEvent);
    callback_pair(): key(0), cb(nullptr) {}
    callback_pair(uint32_t k, void (*_cb)(KeyEvent)): key(k), cb(_cb) {}
};

/***************************************************************************/
/**** SIKTEC_Keys CLASS ****************************************************/
/***************************************************************************/

class SIKTEC_Keys {
    
    private:
        
        // Internal struct of the pins used:
        struct {
            uint8_t latch; 
            uint8_t data; 
            uint8_t clock; 
            uint8_t inter; 
        } pins;
        
        // Holds the shift register bits.
        unsigned char buffer = 0b00000000; 
        
        // keyCodes as chars, index corresponds to the bit position.
        inline static const char keyCodes[] = {
            'u','l','d','r','m','L','R','x'
        };
        
        // Container to hold user defined callbacks
        callback_pair callbacks[SIKETC_KEYS_MAX_CALLBACKS];
        
        // internally counts callbacks.
        uint8_t registered = 0;
        
        // internal activation flag.
        bool active = SIKETC_KEYS_DEFAULT_ACTIVE;
        
        // internal multi key combination flag.
        bool multi;
        
        // stores the sensitivity level.
        uint8_t _sensitivity = SIKETC_KEYS_DEFAULT_SENSITIVITY; /* 1-50 */
    
    public:
        
        // Stores the last time in milli seconds a keypress triggered interupt. 
        volatile int32_t lastDebounceTime;
        
        // Defines the debounce delay to be use
        volatile inline static int32_t debounceDelay;
        
        //static reflectedMethod reflect;
        static SIKTEC_Keys* instance;
        SIKTEC_Keys(bool allowMulti);
	    SIKTEC_Keys(bool allowMulti, uint8_t latchPin, uint8_t dataPin, uint8_t clockPin, uint8_t interPin);
        
        /*!
         * enable
         * ----------------------------
         * @brief enables the keypad callbacks execution
         * @return void 
        */
        void enable();

        /*!
         * disable
         * ----------------------------
         * @brief disables the keypad callbacks execution
         * @return void 
        */
        void disable();
        
        /*!
         * sensitivity
         * ----------------------------
         * @brief sets teh sensitivity level
         * @param uint8_t s
         * @return void 
        */
        void sensitivity(uint8_t s);

        /*!
         * read
         * ----------------------------
         * @brief reads keypad and return a KeyEvent object
         * @return KeyEvent 
        */
        KeyEvent read();

        /*!
         * on
         * ----------------------------
         * @brief attach a callback to s key or combination
         * @param char* key
         * @param void (*)(KeyEvent) cb
         * @return void 
        */
        void on(const char* key, void (*cb)(KeyEvent));

        /*!
         * invoke
         * ----------------------------
         * @brief invoke a callback who is attached to a specific key or combination
         * @param char* key
         * @param KeyEvent &event
         * @return bool - true if invoked 
        */
        bool invoke(const char* key, KeyEvent &event);

        /*!
        * ntDelay
        * ----------------------------
        * @brief nonblocking delay - 0.1s == 100000 micro seconds
        * @param uint8_t t - number of 0.1 seconds to delay
        * @returns void 
        */
        inline static void ntDelay(uint8_t t) {
            for (byte i = 0; i < t; i++) delayMicroseconds(100000);
        }

    private:

        /*!
        * setPinModes: sets pin modes which were set by the constructor
        * ----------------------------
        * @brief sets pin modes which were set by the constructor
        * @returns void 
        */
        void setPinModes();
        
        /*!
        * readShift:
        * ----------------------------
        * @brief fills the buffer with shift register bits
        * @returns void 
        */
        void readShift();
        
        /*!
        * hashKey: 
        * ----------------------------
        * @brief returns a unique id give a sequence of chars independent of position
        * @param char key - the characters
        * @overload char*
        * @returns uint32_t - the id
        */
        uint32_t hashKey(const char key);
        uint32_t hashKey(const char* key);

        /*!
        * [static] isr: 
        * ----------------------------
        * @brief internal interupt function used to call the defined callbacks and read the keys
        * @returns void
        */
        static void isr();

};

} //END Namespace SIKtec

#endif