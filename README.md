# Connect Sega Mega-Drive/Genesis controllers to a PC via USB 

HID mode, so you will NOT need any special device drivers ...They will simply show up as a couple of HID Joysticks

You will need
* a "Digispark Pro" (or cheap clone thereof) off ebay/aliexpress
* half a dozen resistors
* a couple of 9-pin D-Sub (PC serial port) sockets
* some real-basic soldering skillz

Glue all the bits together ...and install the Meag2USB sketch

Connect it to your PC, Raspberry Pi, <whatever> with a USB cable and test it with joy.cpl (or the linux equivalent).

The following is a couple of the README files from the library ...If you don't care about all the technical details, such as: 
* how it reads the controllers
* or how it talks like a joystick to the USB port
* why the decision was made to support only 2 controllers
* why did I chose a basic resistor network to multiplex buttons
* how to modify the code to support 4 controllers

Just scroll to the end to see the wiriing diagram ...AKA. "Sonic is waiting ...What do I glue to what to make this work?"

If you already have Arduino IDE installed, you own a soldering iron, and have a box of random resisitors laying about ...you should be gaming in a hour-or-two :)

If you already have some other version of the "digistump" library installed (very likely) ...just copy the MegaUSB/ directory and drop it in your existing digispark directory: https://github.com/csBlueChip/Mega2USB/tree/master/digistump-avr/libraries/MegaUSB

If you can program, and want to tweak the code for 4 controllers - probably best put aside a day for the job.

```
-------------------------------------------------------------------------------
I have pulled the latest USB code from obdev's github page
the digistump version was rather out-of-date.
...so there is now support for the new ATmel chips

Although, ?obviously?, if you are not using a Digispark [Pro] [Clone]
then you will have to solder your own USB port to pins 3 & 4

If you want to use different pins (for the USB) you will need to look
In the "Hardware Config" section of usbconfig.h
	#define USB_CFG_IOPORTNAME      B
	#define USB_CFG_DMINUS_BIT      1
	#define USB_CFG_DPLUS_BIT       2
...and to do that, you will need an understanding of how the ATmel chips
identify pin numbers internally.

-------------------------------------------------------------------------------
I had to "extern C" a couple of bits of obdev's code to get things
working with "the digispark way", but it's his code
So I've been infected with GPL on my changes to HIS code

BUT *my* code (the example code, and much of Mega2USB.h
               - which will be "statically linked")
               is released under (the GPL compatible) MIT license.

The worst bit is that EVERY device has its own copy of the USB code!? :o
So the update will only benefit people who use Mega2USB or derivations thereof :(

-------------------------------------------------------------------------------
I haven't been through all the libs, but I get the feeling
I am the first person to have multiple HID devices on a single device
...IE. 2 game controllers
So take a peek at my HID descriptor for an example and some handy URLs & toolz

There is no reason you could not have a Mouse-Keyboard-Joystick-Touchpad device

-------------------------------------------------------------------------------
My code is heavily commented, and cleanly sectioned up
But if you've got any questions, raise an issue on github
...the chances are, other people also have the same problem :)

Err, Take a look at wiring.ino which will load when the example loads
that will tell you how to connect the 9-pin ports to the Arduino.

-------------------------------------------------------------------------------
WARNING:
You're gonna need four 10K resistors, and two 6.8K resistors to build this
...Maximum 5% tolerance [for reliable operation]
...Any type. Carbon are cheap and will 'do the job' here.
   Metal Film are my choice for many (google'able) reasons

-------------------------------------------------------------------------------
If you only want to support one controller, you can do it without resistors
...BUT ...you will need to make a few (non-trivial) changes to the example code*

* You need to replace the analogue code with digital code (in the example)
You will need pin-6 and pin-9, instead of pin69
here:
    #define  C1_P69      (A6)     // [input] Controller-1/Pins-{6,9}  -->  Resistor Network  -->  GPIO_A6
here:
    int       p1, p2, p3, p4;
and here:
    ctrl[0] = {0x0000, M_NONE, C1_P1, C1_P2, C1_P3, C1_P4, C1_P69};

And swap out all calls to ISLO_A() and ISHI_A()
             for calls to ISLO_D() and ISHI_D()

You may wish to change the name of the device to Mega1USB
here (in usbconfig.h):
	#define USB_CFG_DEVICE_NAME     'M','e','g','a','2','U','S','B'
	#define USB_CFG_DEVICE_NAME_LEN 8

You may wish to stop the HID device as showing up as two joysticks
here (in usbconfig.h):
	#define USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH    68
and here (in Mega2USB.h):
	//4: HID Gamepad #2 ----------------------------------------------------------------------

Configuring the Mega2USB for only one controller,
you end up with a FOUR available GPIO pins
...You are well within your rights to, I dunno,
# Add an LED which lights up when a 6-button Controller is connected
errrr
# Have a button you can press on the adpator which will press
  UP, C, DOWN, C, LEFT, C, RIGHT, C, A + START
  [those who know, and those who need google]
How about
# Auto-fire enable/diable button
# Change controller ID {1..4}

-------------------------------------------------------------------------------
Support for FOUR controllers has been allowed for in the code
but it is NOT implementd

Read the notes [above] about support for only ONE controller

-------------------------------------------------------------------------------
There is no switch debouncing code.
I did a lot of reading up and people were saying 40mS for debounce
...that's nearly 3 frames
Maybe I'll get a 'scope on my controllers at some point :)

If it becomes an issue, one idea I've had is:
  Read the buttons in to a circular buffer of size N (N=3 ?)
  Start by performing N reads (or bzero() it)
  Then the result at any moment is the (boolean) average of all values in the buffer

That said: I strongly suspect debounce is done in the MD
...If it's done by MD HARDware - we've got a problem [potential solution above]
...If it's done by MD SOFTware - it's never going to show up anyway
```

```
==========================
 Controller State Machine
==========================

A Controller will reset to State-0 if it does not see a clock-edge inside 1.3mS*
  * I have also read 1.6mS, 1.8mS, 13mS and a variety of others <shrug>
  This page has the vibe of "right" about it : http://applause.elfmimi.jp/md6bpad-e.html

The State Machine advances when it sees Clock Edge on DB9-7 (a.k.a. "Select")

,---------------------------------------------------.
| State | Clock |  1  |  2  |  3  |  4  |  6  |  9  |
|-------|-------|-----|-----|-----|-----|-----|-----|
|   0   |  Lo   |  U  |  D  |  0v |  0v |  A  |  S  |
|   1   |  Hi   |  U  |  D  |  L  |  R  |  B  |  C  |
| - - - | - - - | - - | - - | - - | - - | - - | - - |
|   2   |  Lo   |  U  |  D  |  0v |  0v |  A  |  S  |
|   3   |  Hi   |  U  |  D  |  L  |  R  |  B  |  C  |
| - - - | - - - | - - | - - | - - | - - | - - | - - |
|   4   |  Lo   |  0v |  0v |  0v |  0v |  A  |  S  |
|   5   |  Hi   |  Z  |  Y  |  X  |  M  |  5  |  5  |
| - - - | - - - | - - | - - | - - | - - | - - | - - |
|   6   |  Lo   |  5v |  5v |  5v |  5v |  A  |  S  |
|   7   |  Hi   |  U  |  D  |  L  |  R  |  B  |  C  |
`---------------------------------------------------'

The 3-Button Controller only has States-{0,1}

The 6-Button Controller has internal-logic to:
  # become a          3-Button Controller, if M is depressed as it is connected
  # return to being a 6-Button Controller, if X is depressed as it is connected

Button states are inverted logic, so
  0v -> button pressed
  5v -> button not-pressed

So we can process everything nicely [readable code] with this logic:
  0 : If (D3 & D4 are LOW), controller is connected [T_CONN]
      ...If !T_CONN, we ignore all further states
  1 : -ignored-
  2 : Read D-{6,9}         == Button-{A,S}
  3 : Read D-{1,2,3,4,6,9} == Button-{U,D,L,R,B,C}
  4 : If (D1 & D2 are LOW), controller connected is 6-Button [T_6BTN]
      ...If !T_6BTN, we ignore all further states
  5 : Read D-{1,2,3,4}     == {Z,Y,X,M}
  6 : -ignored-
  7 : -ignored-
...Much rumour exists that States-{6,7} are used by third party controllers <shrug>

=================
 Timing Concerns
=================

The ATTiny  takes a moment to change the state of the clock pin : ~ 20nS
            takes a while for an analogue reading to propgate   : ~180nS
The 74HC157 takes a while to register a state change, up to     : ~190nS
...So you're looking at anywhere up to 0.5uS from changing the clk pin,
   to the input data being ready.

==============================
 Controller Wiring : 3-Button
==============================

On a 3-Button controller:
  U & D are wired directly to the DB9 with a 10K pull-ups
  L, R, A, B, C, S, are wired to the DB9 via a TTL multiplexer

          ,-------------------------------------------.
          | Clock |  1  |  2  |  3  |  4  |  6  |  9  |
          |-------|-----|-----|-----|-----|-----|-----|
          |  Lo   |  U  |  D  |  0v |  0v |  A  |  S  |
          |  Hi   |  U  |  D  |  L  |  R  |  B  |  C  |
          `-------------------------------------------'

  [DB9/5]---+--+--+---------+---+----------+--+--+--+--.
            |  |  |         |   |          |  |  |  |  |
            >  >  >         <   <          |  >  >  >  >
            <  <  <         >   >          |  <  <  <  <
            >  >  >         <   <          |  >  >  >  >
            |  |  |         |   |          |  |  |  |  |
            |  |  | [DB9/1]-+   +--[DB9/2] |  |  |  |  |
            |  |  |         |   |          |  |  |  |  |
,-----------{--{--{----(U)--'   `--(D)-----}--}--}--}--}-----------,
|           |  |  |                        |  |  |  |  |           |
|           |  |  |  ########._.#########  |  |  |  |  |           |
|           |  |  |  #                  #  |  |  |  |  |           |
| [DB9/7]>--{--{--+--#-Clk----.     5v0-#--'  |  |  |  |           |
|           |  |     #        |         #     |  |  |  |           |
+-----------{--{-----#----.   |     /En-#-----}--}--}--}-----------+
|           |  |     #    |   |         #     |  |  |  |           |
+---(L)-----{--+-----#---.|   |   .-----#-----+--}--}--}-----(S)---+
|           |        #  ,--.  |   |     #        |  |  |           |
| [DB9/3]<--{--------#--<  |--+   |.----#--------+--}--}-----(C)---+
|           |        #  `--'  |  ,--.   #           |  |           |
+-----------{--------#----.   +--|  >---#-----------}--}-->[DB9/9] |
|           |        #    |   |  `--'   #           |  |           |
+---(R)-----+--------#---.|   |   .-----#-----------+--}-----(A)---+
|                    #  ,--.  |   |     #              |           |
| [DB9/4]<-----------#--<  |--+   |.----#--------------+-----(B)---+
|                    #  `--'  |  ,--.   #                          |
|                 ,--#-Gnd    `--|  >---#----------------->[DB9/6] |
|                 |  #           `--'   #                          |
| [DB9/8]         |  #     74HC157N     #                          |
|    |            |  ####################                          |
|    |            |                                                |
`----+------------+------------------------------------------------'

==============================
 Controller Wiring : 6-Button
==============================

On a 6-Button controller:
  All buttons are wired to the DB9 via logic
  ...the chips used vary, but they all result in TTL logic states

The logic in the original 6-Button Controller is in a GAL
which implements the Controller State Machine.
...Some clone controllers use a pile of 74* logic chips.
...The details of which are beyond the scope of this document.

=====================
 GPIO Considerations
=====================

Controller detection requires checking if D3 and D4 are being Pulled Low
...If they are not being Pulled Low [ie. they are Floating], they must be seen as High
...Therefore we (need to) use the PULLUPs in the ATTiny167 (to stop the Float condition)
...And as such we cannot (easily) put these two pins through a resistor network

For the cost of 13 resistors, you could use an R2R Ladder [DAC]
and group 4 of the DB9 Inputs (ie. {1,2,6,9}) to a single analogue GPIO pin
...that would take us to 3 GPIO pins per Controller (+ clock)
...Which is still too many to be able to support 4 Controllers

For the cost of 3 resistors, you can build a voltage divider
and group 2 of the DB9 Inputs to a single analogue GPIO pin

As we only check for a 6-Button Controller iff we know a Controller is connected
we can group ANY 2 DB9 Inputs (of {1,2,6,9})
...and, of course, we can repeat this excercise for the other two pins

Ultimately, I would have needed to use a fair amount of external/additional
circuitry/components/hassle to support 4 controllers,
...so I stuck with 2 controllers for this project.

I also didn't see any real-world advantage to having lights or buttons
on the adaptor (for my use case),
...so it was only necessary to use TWO "2 Digital -> 1 Analogue" Resistor Networks
...To keep the code simple, I put one (in the same place) on each Controller.


==================
 Resistor Network
==================

Gnd   >---^v^v^---+
                  |
DBx:6 >---^v^v^---+---> GPIO-A
                  |
DBx:9 >---^v^v^---+

Naming: Rxy ->  x -> Controller number
                y -> Pin Number (g=Gnd)

Rxg == Rx9 == 10K
       Rx6 == 6.8K  (~70.2% of Rx9)

See this live circuit to try it out :)
  https://www.falstad.com/circuit/circuitjs.html?cct=$+1+0.000005+9.001713130052181+50+5+43%0Ar+144+64+224+64+0+10000%0Ar+224+16+304+16+0+6800%0Ar+144+16+224+16+0+10000%0Aw+224+16+224+64+0%0Ag+144+64+144+96+0%0AO+320+64+384+64+0%0AR+48+0+16+0+0+0+40+1023+0+0+0.5%0Ag+48+32+16+32+0%0AS+144+16+48+16+0+1+false+0+2%0AS+304+16+384+16+0+0+false+0+2%0AR+384+0+416+0+0+0+40+1023+0+0+0.5%0Ag+384+32+416+32+0%0Ap+224+64+320+64+1+0%0A207+336+80+336+96+5+Vcc%5Cq1023,%5Csso%5CsVout%5Csis%5CsDAC%5CsReading%0Ax+79+20+94+23+4+14+%5Ch6%0Ax+347+20+362+23+4+14+%5Ch9%0A

================
 Wiring Diagram
================

[5v0]---+-------+--------------------------------[5v0]
        |       |
        |       | ,------------------(4)---------[GPIO-D7]
        |       | | ,----------------(3)---------[GPIO-D2]
        |       | | | ,--------------(2)---------[GPIO-D1]
        |       | | | | ,------------(1)---------[GPIO-D0]
        |       | | | | |
        |    .--|-|-|-|-|--.
        |     \ 5 4 3 2 1 / DB9/1
        |      \ 9 8 7 6 /
        |       `|-|-|-|'
        |        | | | |
        |        | | +-}------------(Clk)--------[GPIO-D8]
        |        | | | |      R16
        |        | | | `-----^v^v^---+-------+---[GPIO-A6]
       (5)       | | |               |       |
        |        `-}-}-------^v^v^---'       >
        |          | |        R19            < R1g
        |  .--(8)--+ |                       >
        |  |       | |        R29            |
        |  |     ,-}-}-------^v^v^---+---+---{---[GPIO-A5]
        |  |     | | |               |   |   |
        |  |     | | | ,-----^v^v^---'   |   |
        |  |     | | | |      R26        |   |
        |  |     | | | |                 |   |
        |  |    ,|-|-|-|.                >   |
        |  |   / 9 8 7 6 \           R2g <   |
        |  |  / 5 4 3 2 1 \ DB9/2        >   |
        |  | '--|-|-|-|-|--`             |   |
        |  |    | | | | |                |   |
        `--}----' | | | `---------(1)----{---{---[GPIO-D9]
           |      | | `-----------(2)----{---{---[GPIO-D10]
           |      | `-------------(3)----{---{---[GPIO-D11]
           |      `---------------(4)----{---{---[GPIO-D12]
           |                             |   |
[Gnd]------+-----------------------------+---+---[Gnd]
```
