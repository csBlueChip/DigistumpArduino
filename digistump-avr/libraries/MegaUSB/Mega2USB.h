/*
 * Based on Obdev's AVRUSB code and under the same license.
 *
 * Actually, this is clearly an old version of Obdev's work (not just "based on") 
 * with a few (trivial) "Digispark" tweaks [qv. diff]
 *
 * My first task was to update to Obdev's latest code (on github) [10th/Sept/2019]
 * So if you're going to build a new driver, consider using this as your template!
 * ...It adds things like: driver support for the atmel model 'p'chips (eg. At328p)
 * 
 * As for "the same license", this file is infected with copyleft, but (for what it's worth) 
 * all my modifications are all released as MIT license (which is GPL compatible)
 *
 * TODO: Make a proper file header. :-)  <-- I too laughed at the wasted effort in doing this
 *
 * Modified for Digispark by Digistump
 *
 * And now modified by Sean Murphy (duckythescientist) from a keyboard device to a joystick device
 *
 * And now modified by Bluebie to have better code style, not ruin system timers, and have delay() function
 * Most of the credit for the joystick code should go to Raphaël Assénat
 *
 * Me next: csbluechip did another code cleanup and converted the joystick code to be a pair of 12-button 
 * gamepads ...example code will show you how to use this for a "2 mega-drive controllers" device.
 * The reason for Up/Down/Left/Right as Buttons, not Axes, is because technically/electronically 
 * you can press Up+Down and/or Left+Right at the same time - this functionality is NOT supported with Axes!
 */
#ifndef __mega2usb_h__
#define __mega2usb_h__

//----------------------------------------------------------------------------- ----------------------------------------
#include <Arduino.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/delay.h>
#include <string.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

#include "usbdrv.h"
#include "usbconfig.h"

//----------------------------------------------------------------------------- ----------------------------------------
// 'cos readable code ...and this is the naming convention used by the USB core code
//
#ifndef uchar
#	define  uchar  unsigned char
#endif

#ifndef ulong
#	define  ulong  unsigned long
#endif
 
//----------------------------------------------------------------------------- ----------------------------------------
// Great tutorial:
//   https://eleccelerator.com/tutorial-about-usb-hid-report-descriptors/
// Info about V-USB hardware:
//   http://eleccelerator.com/usnoobie/
// Handy tool:
//   https://www.usb.org/document-library/hid-descriptor-tool
// IF you change the following descriptor
// ...you MUST patch up USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH in usbconfig.h 
// ...I'm gonna go ahead and assume there's a good reason for that!
//
PROGMEM  const  uchar  x_usbHidReportDescriptor[USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH] = {
	//4: HID Gamepad #1 ----------------------------------------------------------------------
		0x05, 0x01,          // USAGE_PAGE (Generic Desktop)
		0x09, 0x05,          // USAGE (Gamepad)
	//6: Physical device     
		0xa1, 0x01,          // COLLECTION (Application)
			0xa1, 0x00,      //   COLLECTION (Physical)
				0x85, 1,     //     REPORT_ID (1)  <---- O_o
	//16: 12 buttons                
				0x05, 0x09,  //     USAGE_PAGE (Button)
				0x19, 1,     //     USAGE_MINIMUM (Button 1)
				0x29, 12,    //     USAGE_MAXIMUM (Button 12)
				0x15, 0,     //     LOGICAL_MINIMUM (0)
				0x25, 1,     //     LOGICAL_MAXIMUM (1)
				0x95, 12,    //     REPORT_COUNT (12)
				0x75, 1,     //     REPORT_SIZE (1)
				0x81, 0x02,  //     INPUT (Data,Var,Abs)
	//6: 4 spare bits               
				0x95, 1,     //     REPORT_COUNT (1) // (1 * 4) unused bits
				0x75, 4,     //     REPORT_SIZE (4)
				0x81, 0x03,  //     INPUT (Const,Var,Abs)
	//2:                            
			0xc0,            //   END_COLLECTION (Physical)
		0xc0,                // END_COLLECTION (Application)
	                         
	//4: HID Gamepad #2 ----------------------------------------------------------------------
		0x05, 0x01,          // USAGE_PAGE (Generic Desktop)
		0x09, 0x05,          // USAGE (Gamepad)
	//6: Physical device     
		0xa1, 0x01,          // COLLECTION (Application)
			0xa1, 0x00,      //   COLLECTION (Physical)
				0x85, 2,     //     REPORT_ID (2)  <---- O_o
	//16: 12 buttons             
				0x05, 0x09,  //     USAGE_PAGE (Button)
				0x19, 1,     //     USAGE_MINIMUM (Button 1)
				0x29, 12,    //     USAGE_MAXIMUM (Button 12)
				0x15, 0,     //     LOGICAL_MINIMUM (0)
				0x25, 1,     //     LOGICAL_MAXIMUM (1)
				0x95, 12,    //     REPORT_COUNT (12)
				0x75, 1,     //     REPORT_SIZE (1)
				0x81, 0x02,  //     INPUT (Data,Var,Abs)
	//6: 4 spare bits            
				0x95, 1,     //     REPORT_COUNT (1) // (1 * 4) unused bits
				0x75, 4,     //     REPORT_SIZE (4)
				0x81, 0x03,  //     INPUT (Const,Var,Abs)
	//2:                         
			0xc0,            //   END_COLLECTION (Physical)
		0xc0                 // END_COLLECTION (Application)
	//== 2*(4+6+16+6+2) = 2*34 = 68 -> USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH in usbconfig.h 
};

#define  REPORT_SIZE  (3)  // ReportID + 16bits of (unused|buttons)

//----------------------------------------------------------------------------- ----------------------------------------
// USB device descriptor
// Again, a bunch of this stuff is actually defined and configured in usbconfig.h
//
PROGMEM  const  uchar  usbDescrDevice[] = {		
		18,                       // sizeof(usbDescrDevice): length of descriptor in bytes

		USBDESCR_DEVICE,          // descriptor type
		0x01, 0x01,               // USB version supported
		USB_CFG_DEVICE_CLASS,
		USB_CFG_DEVICE_SUBCLASS,
		0,                        // protocol
		REPORT_SIZE,              // max packet size
		USB_CFG_VENDOR_ID,        // 2 bytes
		USB_CFG_DEVICE_ID,        // 2 bytes
		USB_CFG_DEVICE_VERSION,   

#if USB_CFG_VENDOR_NAME_LEN
		1,
#else
		0,
#endif

#if USB_CFG_DEVICE_NAME_LEN
		2,
#else
		0,
#endif

#if USB_CFG_SERIAL_NUMBER_LENGTH
		3,
#else
		0,
#endif

		1
};

//----------------------------------------------------------------------------- ----------------------------------------
// Configured by the class, used by the driver
//
static uchar*  rt_usbHidReportDescriptor     = NULL ;
static uchar   rt_usbHidReportDescriptorSize = 0 ; 

static uchar*  rt_usbDeviceDescriptor        = NULL ;
static uchar   rt_usbDeviceDescriptorSize    = 0 ;

static uchar   idle_rate                     = 0;

//$ - I decided to pad the buffer so I could (easily and efficiently) avoid calling memcpy()...
uchar   reportSend[4]/*[REPORT_SIZE]*/;  //$ Report that is being sent

//&============================================================================ ========================================
class  Mega2USBDevice
{
private:
	uchar  reportBuild[4]/*[REPORT_SIZE]*/;  //$ Report we are building
	ulong  reportTime = 0;                   // Last time a report was sent

public:
	//+======================================================================== ========================================
	// Send a USB report
	// If the watchdog need patting, we will resend the last report
	//
	bool  update (int force = 0) 
	{
		usbPoll();
		
		if ( usbInterruptIsReady()  &&  (force  ||  (millis() >= reportTime + (idle_rate * 4))) ) 
		{
			//$	memcpy(reportSend, reportBuild, REPORT_SIZE) ;
			*((ulong*)reportSend) = *((ulong*)reportBuild);
			
			usbSetInterrupt(reportSend, REPORT_SIZE);
			reportTime = millis();
			return true;
		}
		return false;
	}

	//+======================================================================== ========================================
	// By exposing setup(), we can implement feature changes without powering down the device!
	//
	void  setup (int hz) 
	{
		// Set idle rate
		idle_rate = (1000 / hz) / 4; // USB uses 4mS units
		
		cli();
		{
			usbDeviceDisconnect();
			_delay_ms(250);
			usbDeviceConnect();
		
			rt_usbDeviceDescriptor        = (uchar*)usbDescrDevice;
			rt_usbDeviceDescriptorSize    = sizeof (usbDescrDevice);
			
			rt_usbHidReportDescriptor     = (uchar*)x_usbHidReportDescriptor;
			rt_usbHidReportDescriptorSize = sizeof (x_usbHidReportDescriptor);
			
			usbInit();
		}
		sei();

		reportTime = millis();
	}
	
	//+======================================================================== ========================================
	// USB must be poked on a regular basis 
	// So, to prevent unwanted detach events, use this delay rather than the Arduino delay()
	// This is a "busy wait" loop - so time to take off your scarf
	void  delay (long ms) 
	{
		ulong  last = millis();
		while (ms > 0) {
			ulong  now = millis();
			
			ms   -= now - last;
			last  = now;
			update();
		}
	}
	
	//+======================================================================== ========================================
	// Set the buttons for the specified controller
	//
	void setButtons(uint8_t ctrl,  uint16_t btns) 
	{
		reportBuild[0] = ctrl + 1;
		reportBuild[1] = (uchar)(btns &  0xFF);
		reportBuild[2] = (uchar)(btns >> 8   );
		
		while (!update(1)) ;  // Keep retrying until it is sent
	}

//	// Yep, I will never remember that I left this code here to remind me about reinterpret_cast !	
//	void  setButtons (char low,  char high) 
//	{
//		setButtons( *reinterpret_cast<unsigned char *>(&low),
//		            *reinterpret_cast<unsigned char *>(&high) );
//	}
};

//----------------------------------------------------------------------------- ----------------------------------------
// Instantiate a copy of the device
Mega2USBDevice  Mega2USB;

//#============================================================================ ========================================
// Current best guess : Driver hooks
//
//#ifdef __cplusplus
//extern "C"{
//#endif 

	//+======================================================================== ========================================
	// USB_PUBLIC uchar usbFunctionSetup
	//
	uchar  usbFunctionSetup (uchar* data) 
	{
		usbRequest_t*  rq = (usbRequest_t*)data;

		usbMsgPtr = reportSend;
		if ((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS) { // class request type
			if (rq->bRequest == USBRQ_HID_GET_REPORT){ 
				// wValue: ReportType (highbyte), ReportID (lowbyte)
				// Both reports are the same size, so don't look at wValue
				return REPORT_SIZE;
				
			} else if (rq->bRequest == USBRQ_HID_GET_IDLE) {
				usbMsgPtr = &idle_rate;
				return 1;
				
			} else if (rq->bRequest == USBRQ_HID_SET_IDLE) {
				idle_rate = rq->wValue.bytes[1];
			}
			
		} else {
			/* no vendor specific requests implemented */
		}
		return 0;
	}

	//+======================================================================== ========================================
	// Surely this has to be a driver hook that requests the descriptors from our device code
	//
	uchar  usbFunctionDescriptor (usbRequest_t* rq) 
	{
		if ((rq->bmRequestType & USBRQ_TYPE_MASK) != USBRQ_TYPE_STANDARD)  
			return 0;

		if (rq->bRequest == USBRQ_GET_DESCRIPTOR) {
			// USB spec 9.4.3, high byte is descriptor type
			switch (rq->wValue.bytes[1]) {
				case USBDESCR_DEVICE:
					usbMsgPtr = rt_usbDeviceDescriptor;
					return      rt_usbDeviceDescriptorSize;
					
				case USBDESCR_HID_REPORT:
					usbMsgPtr = rt_usbHidReportDescriptor;
					return      rt_usbHidReportDescriptorSize;
			}
		}
		
		return 0;
	}

//#ifdef __cplusplus
//} // extern "C"
//#endif

#endif // __mega2usb_h__
