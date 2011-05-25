=====================
Iriver H100 IR Remote
=====================
This is a simple infrared remote control for the Iriver H100 series MP3
players. It also works on the H300 series since that uses the same technique
for interacting with the remote, the remotes can be changed between those
players. There are three different types of remotes known:

- *H100 LCD remote*.
  This remote has 11 buttons (3 rocker wheels and 2 separate buttons) and a
  monochrome LCD.
- *H300 non-LCD remote*.
  This remote has only the most important buttons to control playback: *Play*,
  *Fwd*, *Back*, *Volume Up*, and *Volume Down*.  Note that there is no *Stop*
  button on this remote, it's therefore not possible to stop playback but only
  to pause it. However, stopping playback isn't necessary in general: All Ipods
  don't have a Stop button either.
- *H300 LCD remote*.
  This remote has 11 buttons (2 rocker wheels and a joystick) with a monochrome
  LCD.

A picture of the different `Iriver Remotes`_ is available on the net.


How it works
============
The Iriver H100 remote uses two different techniques for generating button
events.

- *Hold Button*. The Hold button is not of interest for this project and
  therefore not discussed. It has a separate line that also seems to control
  the type of the remote. The emulated *H300 non-LCD remote* has this line open
  so there is no need to handle it.
- *Play Button*. The Play button has a dedicated line that will get pulled down
  to GND via a 560R resistor when the button is pressed.
- *All other buttons*. All other buttons are connected via a resistor network
  and pull an ADC line down to ``GND`` via different resistor values. The ADC
  line is pulled to ``Vcc`` internally in the player, making a voltage divider
  with the pull down resistor. The voltage of that divider is read out by the
  ADC and the player can act upon. As a consequence pressing multiple buttons
  is not possible, and the button with the lower resistor value will win.

See also the `Remote Description`_ in the Rockbox_ wiki.


Implementation
==============
The circuit emulates the *H300 non-LCD remote*. This has been selected because

- the buttons on that remote are usually sufficient to control playback. You
  need to go to your CD player to switch to different CDs as well, and changing
  playlists is a similar operation.
- the display of the main unit is usually too far away to be readable. A big
  font can help here but makes the use of the LCD rather limited.
- pressing a button on the remote does not light up the backlight of the main
  LCD, making it even less usable as display for greater distances. This could
  be changed by patching Rockbox_.
- the above assumes that the player is running Rockbox_. Since this remote
  doesn't affect the software on the player it should also work with the Iriver
  firmware, so such changes shouldn't be depend on.

Features
--------
- simulates the *H300 non-LCD remote*.
- powered by the player itself, so no separate power supply is needed.
- RC5 remote codes used which has been widely used in the past. It should be
  easy to find a universal remote that can send RC5.

Todo
----
- make the remote commands programmable
- fix the bug that prevents the player to power up with the remote attached.

Q&A
---
Why use an ATtiny2313?
  Simply because I had one lying around and looked into RC5 reception with that
  controller before. Any other controller that can do the decoding should work.

Why use the *H300 non-LCD remote*?
  Because of two reasons: (1) I don't see too much point in having much more
  buttons (since you can't read the main LCD from a great distance anyway, and
  the remote doesn't have a feedback channel for passing the remote LCD
  contents around), and (2) because I didn't figure how to tell the player a
  LCD remote is attached. I though I know how to do this but Rockbox_ didn't
  recognize it. Given (1) I decided not to spend too much time on that.

Why keep the serial port pins unused?
  With the serial port being still available it's possible to extend the
  microcontroller firmware to send Ipod Accessory Protocol messages, making the
  remote also work on Ipods.
  This hasn't been implemented yet and might need an additional crystal for the
  serial baudrate to be exact enough. There are varying reports on using the
  UART with the internal oscillator of the controller on the net.


.. _Rockbox: http://www.rockbox.org
.. _Iriver Remotes: https://picasaweb.google.com/peter.dhoye/RockboxDevcon2008#5216945741059780466
.. _Remote Description: http://www.rockbox.org/wiki/IriverHardwareComponents#Remote_Control
.. _RC5 description: http://www.sbprojects.com/knowledge/ir/rc5.htm
