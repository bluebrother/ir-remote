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

#ifndef RC5_H
#define RC5_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#define xRC5_IN         PIND    /* Input Pin */
#define xRC5            PD2     /* IR input low active */

#define XTAL 8e6

#define BUTTON_HOLD_TICKS 2000  /* interrups triggered every 256/8MHz = 32us so
                                   this results in a timeout of 64ms */

struct keypress
{
    uint8_t button;
    uint16_t until;
};

enum actions {
    NONE,
    PLAY,
    STOP,
    FWD,
    REW,
    VOLUP,
    VOLDOWN,
};
extern volatile unsigned int rc5_data;  /* RC5 data */
extern struct keypress current_press;
extern volatile uint16_t tick;          /* continuous tick */

#endif

