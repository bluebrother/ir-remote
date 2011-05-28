/* *********************************************************************
 * Iriver H100 / H300 IR remote
 *
 * This builds a IR receiver for the H100 / H300 series DAPs.
 * The remote simulates the H300 non-LCD remote using a resistor network
 * as used in the real remote.
 *
 * (c) 2011 Dominik Riebeling
 *
 * Based on RC5 decoder implementation by Peter Dannegger:
 * http://www.mikrocontroller.net/topic/12216
 * ********************************************************************* */

#include "rc5.h"

/* RC5 codes for individual key presses, matching Hauppauge WinTV remote */
#define KEYCODE_NEXT        32
#define KEYCODE_PREV        33
#define KEYCODE_PAUSE       38
#define KEYCODE_VOLUP       16
#define KEYCODE_VOLDOWN     17
#define KEYCODE_PLAY        46

struct keypress current_press;

int main(void)
{
    unsigned int i;
    unsigned char old_toggle_bit = 0;
    unsigned char current_toggle_bit = 0;
    unsigned char startup = 1;

    current_press.button = NONE;
    current_press.until = 0;

    TCCR0B = 1<<CS02;       /* enable division by 256 */
    TIMSK = 1<<TOIE0;       /* enable timer interrupt */

    /* Pins used for button emulation.
     * We want them to be output low (sink) or input Hi-Z. Therefore the remote
     * will toggle DDR only.
     */
    PORTB &= ~(1<<PB0 | 1<<PB1 | 1<<PB2 | 1<<PB3 | 1<<PB4);
    PINB  &= ~(1<<PB0 | 1<<PB1 | 1<<PB2 | 1<<PB3 | 1<<PB4);

    /* initialize PD3 as output -- LED */
    PORTD |= 1<<PD3;    /* output high */
    DDRD |= 1<<PD3;     /* use as output */

    /* initialize PD5 as output -- PLAY button */
    PORTD &= ~(1<<PD5); /* output low */
    DDRD |= 1<<PD5;     /* use as output */

    /* main loop */
    while(1) {
        cli();
        /* read bytes from ISR */
        i = rc5_data;
        rc5_data = 0;
        sei();

        if(i) {
            /* handle toggle bit on startup */
            current_toggle_bit = (i>>11)&1;
            if(startup) {
                old_toggle_bit = !current_toggle_bit;
                startup = 0;
            }
            /* toggle LED on toggle bit */
            if((i>>11)&1)
                PORTD |= 1<<PD3;
            else
                PORTD &= ~(1<<PD3);

            /* TODO: make sure no other key is currently pressed and thus
             * overwritten */
            switch((i & 0x3F) | (~i >> 7 & 0x40)) {
                case KEYCODE_PREV:
                    current_press.button = REW;
                    current_press.until = tick + BUTTON_HOLD_TICKS;
                    break;

                case KEYCODE_NEXT:
                    current_press.button = FWD;
                    current_press.until = tick + BUTTON_HOLD_TICKS;
                    break;

                case KEYCODE_VOLUP:
                    current_press.button = VOLUP;
                    current_press.until = tick + BUTTON_HOLD_TICKS;
                    break;

                case KEYCODE_VOLDOWN:
                    current_press.button = VOLDOWN;
                    current_press.until = tick + BUTTON_HOLD_TICKS;
                    break;

                case KEYCODE_PLAY:
                    /* only press play if toggle bit changed.
                     * This avoids holding the button for too long.
                     */
                    if(old_toggle_bit != current_toggle_bit) {
                        current_press.until = tick + BUTTON_HOLD_TICKS;
                        current_press.button = PLAY;
                    }
                    break;

                default:
                    break;
            }
            old_toggle_bit = current_toggle_bit;
        }
    }
}

/* vim: ts=4 sw=4 expandtab : */

