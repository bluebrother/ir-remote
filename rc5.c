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

#define RC5TIME         1.778e-3 /* 1.778msec */
#define PULSE_MIN       (unsigned char)(XTAL / 512 * RC5TIME * 0.4 + 0.5)
#define PULSE_1_2       (unsigned char)(XTAL / 512 * RC5TIME * 0.8 + 0.5)
#define PULSE_MAX       (unsigned char)(XTAL / 512 * RC5TIME * 1.2 + 0.5)


static unsigned char rc5_bit;   /* bit value */
static unsigned char rc5_time;  /* count bit time */
static unsigned int  rc5_tmp;   /* shift bits in */
volatile unsigned int rc5_data; /* store result */
volatile uint16_t tick = 0;


SIGNAL (SIG_OVERFLOW0)
{
    unsigned int tmp = rc5_tmp;                 /* for faster access */

    TCNT0 = -2;                                 /* 2 * 256 = 512 cycle */

    if( ++rc5_time > PULSE_MAX ){               /* count pulse time */
        if( !(tmp & 0x4000) && tmp & 0x2000 )   /* only if 14 bits received */
            rc5_data = tmp;
        tmp = 0;
    }

    if( (rc5_bit ^ xRC5_IN) & 1<<xRC5 ){        /* change detect */
        rc5_bit = ~rc5_bit;                     /* 0x00 -> 0xFF -> 0x00 */

        if( rc5_time < PULSE_MIN )              /* to short */
            tmp = 0;

        if( !tmp || rc5_time > PULSE_1_2 ){     /* start or long pulse time */
            if( !(tmp & 0x4000) )               /* not to many bits */
                tmp <<= 1;                      /* shift */
            if( !(rc5_bit & 1<<xRC5) )          /* inverted bit */
                tmp |= 1;                       /* insert new bit */
            rc5_time = 0;                       /* count next pulse time */
        }
    }

    rc5_tmp = tmp;
    tick++;

    if(current_press.button == PLAY) {
        if(tick < current_press.until) {
            PORTD |= 1<<PD5;
        }
        else {
            PORTD &= ~(1<<PD5);
            current_press.button = NONE;
        }
    }
    else if(current_press.button != NONE) {
        if(tick < current_press.until) {
            switch(current_press.button) {
                case REW:
                    DDRB |= 1<<PB2;
                    break;
                case FWD:
                    DDRB |= 1<<PB3;
                    break;
                case VOLUP:
                    DDRB |= 1<<PB0;
                    break;
                case VOLDOWN:
                    DDRB |= 1<<PB1;
                    break;
            }

        }
        else {
            switch(current_press.button) {
                case REW:
                    DDRB &= ~(1<<PB2);
                    break;
                case FWD:
                    DDRB &= ~(1<<PB3);
                    break;
                case VOLUP:
                    DDRB &= ~(1<<PB0);
                    break;
                case VOLDOWN:
                    DDRB &= ~(1<<PB1);
                    break;
            }
            current_press.button = NONE;
        }
    }
}

