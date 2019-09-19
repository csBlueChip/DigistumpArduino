#if 0  // This whole file is documentation

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
...If it's done by MD hardware - we've got a problem [potential solution above]
...If it's done by MD software - it's never going to show up anyway

#endif
