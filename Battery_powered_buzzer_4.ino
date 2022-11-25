/*----------------------------------------------------------------------*
 * sleep_ATmega328P.ino by Jack Christensen is used, which is licensed  *
 * under the Creative Commons Attribution-ShareAlike 3.0 Unported       *
 * License. To view a copy of this license,                             *
 * visit http://creativecommons.org/licenses/by-sa/3.0/ or send a       *
 * letter to Creative Commons, 171 Second Street, Suite 300,            *
 * San Francisco, California, 94105, USA.                               *
 * Adaptations were made to work with Atmega328P U on a breadboard.     *
 * The trigger must be open at rest and closed even momentarily or      *
 * 'bouncily' to wake up. This way almost no current is drawn.          *
 *----------------------------------------------------------------------*/ 
 
#include <avr/sleep.h>
const int buzzer = 9; //buzzer to arduino pin 9

void setup(void)
{

    for (byte i=0; i<20; i++) {    //make all pins inputs with pullups enabled
        pinMode(i, INPUT_PULLUP);
    }

    pinMode(buzzer, OUTPUT); // Set buzzer - pin 9 as an output
    digitalWrite(buzzer, LOW);        //drive it low so it doesn't source current
}

void loop(void)
{
    goToSleep();
}

void goToSleep(void)
{
    delay(1000);        // ...for 1 sec
    tone(buzzer, 1); // Send 1KHz sound signal...
    delay(100);
    tone(buzzer, 2); // Send 1KHz sound signal...
    delay(2000);        // ...for 1 sec
    noTone(buzzer);     // Stop sound...
    byte adcsra = ADCSRA;          //save the ADC Control and Status Register A
    ADCSRA = 0;                    //disable the ADC
    EICRA = _BV(ISC00);            //configure INT0 to trigger on change
    EIMSK = _BV(INT0);             //enable INT0
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    cli();                         //stop interrupts to ensure the BOD timed sequence executes as required
    sleep_enable();
    //disable brown-out detection while sleeping (20-25µA)
    uint8_t mcucr1 = MCUCR | _BV(BODS) | _BV(BODSE);
    uint8_t mcucr2 = mcucr1 & ~_BV(BODSE);
    MCUCR = mcucr1;
    MCUCR = mcucr2;
    //sleep_bod_disable();           //for AVR-GCC 4.3.3 and later, this is equivalent to the previous 4 lines of code
    sei();                         //ensure interrupts enabled so we can wake up again
    sleep_cpu();                   //go to sleep
    sleep_disable();               //wake up here
    ADCSRA = adcsra;               //restore ADCSRA
}

//external interrupt 0 wakes the MCU
ISR(INT0_vect)
{
    EIMSK = _BV(INT0);                    
}
