/**
 * Krake Software
 *  2007, Andreas Bergauer
 *
 * Function:
 */

// NOTE: Be sure to burn fuses to select internal oscillator!
// 9.6MHz on default on internal oscillator
#define F_CPU 9600000

#include <ctype.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#include <avr/wdt.h>

#include <avr/delay.h>
#include <stdlib.h> 

// Pin mappings
#define PIN_LED1    PB3
#define PIN_LED2    PB4

// write digital "high" to pin <pn> on port <prt>
#define DIGIWRITE_H(prt, pn) prt |= (1<<pn)

// write digital "low" to pin <pn> on port <prt>
#define DIGIWRITE_L(prt, pn) prt &= ~(1<<pn)

// Set bits corresponding to pin usage above       this is to enable pullups
#define PORTB_MASK  (1<< PB3)|(1<< PB4)
// these are the outputs
#define DDRB_MASK   (1<< PB3)|(1<< PB4)
#define INPUT_MASK  (1<< PB0)|(1<< PB1)


typedef uint8_t byte;

byte state;
int  keydown_counter;
uint16_t timer;
uint8_t toggle = 0;

#define STATE_START             0
#define STATE_RESET_TIMER       1
#define STATE_CHANGE_COLOR      2
#define STATE_END               3

#define DELAY 20



/*
 *
 */
void init(void)
{
  DDRB = DDRB_MASK;            // set port pins to  output (and input)
  
  TIFR0  = (1 << TOV0);          // clear interrupt flag
  TIMSK0 = (1 << TOIE0);         // enable overflow interrupt
  TCCR0B = (1 << CS00);          // start timer, no prescale
  
  sei();                      // enable interrupts
  
  state = STATE_RESET_TIMER;
  DIGIWRITE_L(PORTB, (toggle == 0 ? PIN_LED1 : PIN_LED2));
  DIGIWRITE_H(PORTB, (toggle == 0 ? PIN_LED2 : PIN_LED1));

}

/*
 *
 */
int main(void)
{
    init();

    for(;;) {

        // little state machine: first detect button push,
        if( bit_is_clear(PINB,PB0) ) {  // PB0 is timer resetting
            _delay_ms(DELAY);
            if( bit_is_clear(PINB,PB0) )
                state = STATE_RESET_TIMER;
        }
        else {
            if( state == STATE_RESET_TIMER ) 
                state = STATE_START;
        }
        
        if( bit_is_clear(PINB,PB1) ) {  // PB1 is color changing
            _delay_ms(DELAY);
            if( bit_is_clear(PINB,PB1) )
                state = STATE_CHANGE_COLOR;
        }
        else {
            if( state == STATE_CHANGE_COLOR ) 
                state = STATE_START;
        }

        // handle actions within a state
        switch(state) { 
        case STATE_START:
		    timer--;
			if ( timer < 10 ) {
			   // switch off
               DIGIWRITE_H(PORTB, PIN_LED1);
               DIGIWRITE_H(PORTB, PIN_LED2);
			}   
            break;
        case STATE_RESET_TIMER:
		    timer = 500;
            DIGIWRITE_L(PORTB, (toggle == 0 ? PIN_LED1 : PIN_LED2));
            DIGIWRITE_H(PORTB, (toggle == 0 ? PIN_LED2 : PIN_LED1));
            break;
        case STATE_CHANGE_COLOR:
            // alternate the toggle variable
            toggle = !toggle;
            // alternate between the LEDs
            DIGIWRITE_L(PORTB, (toggle == 0 ? PIN_LED1 : PIN_LED2));
            DIGIWRITE_H(PORTB, (toggle == 0 ? PIN_LED2 : PIN_LED1));
            break;
        }

        _delay_ms(DELAY);
    }
    
}



