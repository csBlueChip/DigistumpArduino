//------------------------------------------------------------------------------- 
// Pick an output device
//
#define  FOR_DEBUG  (1)     // Serial debug mode
#define  FOR_MD     (3)     // Megadrive controller mode
#define  COMPILE    FOR_MD  // Choose your compile mode

// We must use the correct delay function to maintain the USB connection
#if   (COMPILE == FOR_DEBUG)
#   include <DigiCDC.h>
#   define DELAY(ms)  SerialUSB.delay(ms)
//#   ifdef F  // For some reason I haven't found, F() will not play ball! >:-/
//#     undef F
//#   endif
//#   define  F(s)  s

#elif (COMPILE == FOR_MD)
#   include "Mega2USB.h"
#   define DELAY(ms)   Mega2USB.delay(ms)
#   define REFRESH_HZ  (50)

#else
#   error Unknown COMPILE method!
#endif    

//------------------------------------------------------------------------------- 
// ATTiny167 wiring
//
#define  C_CNT       (2)      // Number of supported controllers

#define  C1_P1       (0 )     // [input] Controller-1/Pin-1  -->  GPIO_D0
#define  C1_P2       (1 )     // [input] Controller-1/Pin-2  -->  GPIO_D1
#define  C1_P3       (2 )     // [input] Controller-1/Pin-3  -->  GPIO_D2
#define  C1_P4       (7 )     // [input] Controller-1/Pin-4  -->  GPIO_D7
#define  C1_P69      (A6)     // [input] Controller-1/Pins-{6,9}  -->  Resistor Network  -->  GPIO_A6

#define  C2_P1       (9 )     // [input] Controller-2/Pin-1  -->  GPIO_D9
#define  C2_P2       (10)     // [input] Controller-2/Pin-2  -->  GPIO_D10
#define  C2_P3       (11)     // [input] Controller-2/Pin-3  -->  GPIO_D11
#define  C2_P4       (12)     // [input] Controller-2/Pin-4  -->  GPIO_D12
#define  C2_P69      (A5)     // [input] Controller-2/Pins-{6,9}  -->  Resistor Network  -->  GPIO_A5

#if (C_CNT > 2)
# error No pins defined for controllers 3 or 4

# define  C3_P1      (? )     // [input] Controller-3/Pin-1  -->  GPIO_D?
# define  C3_P2      (? )     // [input] Controller-3/Pin-2  -->  GPIO_D?
# define  C3_P3      (? )     // [input] Controller-3/Pin-3  -->  GPIO_D?
# define  C3_P4      (? )     // [input] Controller-3/Pin-4  -->  GPIO_D?
# define  C3_P69     (? )     // [input] Controller-3/Pins-{6,9}  -->  Resistor Network  -->  GPIO_A?

# define  C4_P1      (? )     // [input] Controller-4/Pin-1  -->  GPIO_D?
# define  C4_P2      (? )     // [input] Controller-4/Pin-2  -->  GPIO_D?
# define  C4_P3      (? )     // [input] Controller-4/Pin-3  -->  GPIO_D?
# define  C4_P4      (? )     // [input] Controller-4/Pin-4  -->  GPIO_D?
# define  C4_P69     (? )     // [input] Controller-4/Pins-{6,9}  -->  Resistor Network  -->  GPIO_A?
#endif

#define  CX_P7       (8)      // [output] : Clock/Select 
#define  C_CLK       (CX_P7)  // Pseudonyms
#define  C_SEL       (CX_P7)

//------------------------------------------------------------------------------- 
// Play with the resistor network here:
// https://www.falstad.com/circuit/circuitjs.html?cct=$+1+0.000005+9.001713130052181+50+5+43%0Ar+144+64+224+64+0+10000%0Ar+224+16+304+16+0+6800%0Ar+144+16+224+16+0+10000%0Aw+224+16+224+64+0%0Ag+144+64+144+96+0%0AO+320+64+384+64+0%0AR+48+0+16+0+0+0+40+1023+0+0+0.5%0Ag+48+32+16+32+0%0AS+144+16+48+16+0+1+false+0+2%0AS+304+16+384+16+0+0+false+0+2%0AR+384+0+416+0+0+0+40+1023+0+0+0.5%0Ag+384+32+416+32+0%0Ap+224+64+320+64+1+0%0A207+336+80+336+96+5+Vcc%5Cq1023,%5Csso%5CsVout%5Csis%5CsDAC%5CsReading%0Ax+79+20+94+23+4+14+%5Ch6%0Ax+347+20+362+23+4+14+%5Ch9%0A
//
// Let's get the compiler to do all the resistor-network calculations
// ...This way you can change:  [see fields marked *]
//       a) the number of bits in the ADC
//   and b) the values of the resistors

// ADC definition
#define DAC_BITS  (10)                                                 // * Number of bits in ADC
#define DAC_MAX   ((double)((1 << DAC_BITS) - 1))                      //   Max value of ADC
#define DAC_MIN   ((double)(0))                                        //   Min value of ADC

// Resistors
#define RG_OHM    (10000)                                              // * Resistor: Ground
#define R6_OHM    (RG_OHM)                                             //   Resistor: Pin-6   == RG
#define R9_OHM    (6800)                                               // * Resistor: Pin-9   ~= RG * 0.702

// Parallel calculations
#define R_PLL_G6  ((int)(1.0 / ( (1.0 / RG_OHM) + (1.0 / R6_OHM))))    //   (RG | R6)
#define R_PLL_G9  ((int)(1.0 / ( (1.0 / RG_OHM) + (1.0 / R9_OHM))))    //   (RG | R9)
#define R_PLL_69  ((int)(1.0 / ( (1.0 / R6_OHM) + (1.0 / R9_OHM))))    //   (R6 | R9)

// Network readings 
#define CX_PX     ((int)((DAC_MAX / (R_PLL_69 + RG_OHM)) * RG_OHM))    //   Pressed: None
#define CX_P6     ((int)((DAC_MAX / (R_PLL_G6 + R9_OHM)) * R_PLL_G6))  //   Pressed: Pin-6 
#define CX_P9     ((int)((DAC_MAX / (R_PLL_G9 + RG_OHM)) * R_PLL_G9))  //   Pressed: Pin-9
#define CX_P69    ((int)(DAC_MIN))                                     //   Pressed: Pin-6 + Pin-9

// Couple of handy-dandy all-purpose macros
#define  MIN(x,y) ( ((x) < (y)) ?     (x)     :     (y)     )          // Minimum of two numbers
#define  DIF(x,y) ( ((x) < (y)) ? ((y) - (x)) : ((x) - (y)) )          // Absolute difference between two numbers

// Find the smallest difference
#define  DIF_MIN  MIN(MIN(MIN(MIN(MIN(DIF(CX_PX, CX_P6), DIF(CX_PX, CX_P9)),\
                                  DIF(CX_PX, CX_P69)),\
                              DIF(CX_P6, CX_P9)),\
                          DIF(CX_P6, CX_P69)),\
                      DIF(CX_P9, CX_P69)) 

// The maximum deviation (caused by eg. resistor tolerances, wiring defects, RF inteference, etc.)
// NB. "&~1" is a way to round-down to the nearest even number
#define  DEV      ((DIF_MIN & ~1) / 2)

// Lowest and highest readings for a give pin (combo)
#define  LO(p)    ((p) - DEV)
#define  HI(p)    ((p) + DEV)

#define  CX_PX_MIN   LO(CX_PX)
#define  CX_PX_MAX   HI(CX_PX)
#define  CX_P6_MIN   LO(CX_P6)
#define  CX_P6_MAX   HI(CX_P6)
#define  CX_P9_MIN   LO(CX_P9)
#define  CX_P9_MAX   HI(CX_P9)
#define  CX_P69_MIN  LO(CX_P69)
#define  CX_P69_MAX  HI(CX_P69)

//------------------------------------------------------------------------------- 
// There are 12 (possible) buttons (per controller)
//
#define  B_CNT  (12)

#define  BU     (0x0001)  // Up
#define  BD     (0x0002)  // Down
#define  BL     (0x0004)  // Left
#define  BR     (0x0008)  // Right

#define  BA     (0x0010)  // A
#define  BB     (0x0020)  // B
#define  BC     (0x0040)  // C
#define  BS     (0x0080)  // Start

// X, Y, Z, & M are only on the "6-Button" {A,B,C,X,Y,Z} Controller
#define  BX     (0x0100)  // X
#define  BY     (0x0200)  // Y
#define  BZ     (0x0400)  // Z
#define  BM     (0x0800)  // Mode

//------------------------------------------------------------------------------- 
// All Possible events
//   Format : 0x 00'NTF'BBb
//   0   = Unused
//   N   = Controller Number {1,2,3,4}
//   T   = Event Type    -> 1:Connection, 2:Button
//   F   = Event Feature -> (T==Connection) -> 1:Add  , 0:Remove
//                          (T==Button    ) -> 1:Press, 0:Release
//   BBb = Button ID     -> (T==Button)     -> Button ID [see above]
//   BB  = Reserved      -> (T==Connection) -> Always 00
//     b = Button Count  -> (T==Connection) -> 1:3-button, 2:6-button
//
#define  EV_C1    (0x100000)
#define  EV_C2    (0x200000)
#define  EV_C3    (0x300000)
#define  EV_C4    (0x400000)
#define  EVM_C    (0xF00000)  // Controller Mask

#define  EV_C_GET(ev)    (( ((ev) & EVM_C) >> 20 ) - 1)  // Find controller array-index
#define  EV_C_SET(ev,c)  ( ((ev) & ~EVM_C) | (((c) + 1) << 20) )

// Connection events
#define  EV_CONN  ((0x010000))            // Controller system event
#define  EV_ADD   ((0x001000) | EV_CONN)  // Controller Added
#define  EV_RMV   ((0x000000) | EV_CONN)  // Controller Removed
#define  EV_ADD3  ((0x000001) | EV_ADD)   // Controller Added : 3-button
#define  EV_ADD6  ((0x000002) | EV_ADD)   // Controller Added : 6-button

// Button Events
#define  EV_BTN   ((0x020000))            // Button Event
#define  EV_PRS   ((0x001000) | EV_BTN)   // Button Pressed
#define  EV_REL   ((0x000000) | EV_BTN)   // Button Released

#define  EV_PU    (EV_PRS | BU)           // Press: Up
#define  EV_PD    (EV_PRS | BD)           // Press: Down
#define  EV_PL    (EV_PRS | BL)           // Press: Left
#define  EV_PR    (EV_PRS | BR)           // Press: Right
#define  EV_PA    (EV_PRS | BA)           // Press: A
#define  EV_PB    (EV_PRS | BB)           // Press: B
#define  EV_PC    (EV_PRS | BC)           // Press: C
#define  EV_PS    (EV_PRS | BS)           // Press: Start
#define  EV_PX    (EV_PRS | BX)           // Press: X
#define  EV_PY    (EV_PRS | BY)           // Press: Y
#define  EV_PZ    (EV_PRS | BZ)           // Press: Z
#define  EV_PM    (EV_PRS | BM)           // Press: Mode

#define  EV_RU    (EV_REL | BU)           // Release: Up
#define  EV_RD    (EV_REL | BD)           // Release: Down
#define  EV_RL    (EV_REL | BL)           // Release: Left
#define  EV_RR    (EV_REL | BR)           // Release: Right
#define  EV_RA    (EV_REL | BA)           // Release: A
#define  EV_RB    (EV_REL | BB)           // Release: B
#define  EV_RC    (EV_REL | BC)           // Release: C
#define  EV_RS    (EV_REL | BS)           // Release: Start
#define  EV_RX    (EV_REL | BX)           // Release: X
#define  EV_RY    (EV_REL | BY)           // Release: Y
#define  EV_RZ    (EV_REL | BZ)           // Release: Z
#define  EV_RM    (EV_REL | BM)           // Release: Mode

//------------------------------------------------------------------------------- 
// Controller type (detected)
//
typedef
  enum mode {
    M_NONE = 0x00,           // No controller connected
    M_CONN = 0x01,           // Controller connected, not yet identified
    M_3BTN = 0x02 | M_CONN,  // Controller idenitified at 6-button
    M_6BTN = 0x04 | M_CONN,  // Controller idenitified at 6-button
  }
mode_t;

//------------------------------------------------------------------------------- 
// Controller data
//
typedef
  struct ctrl {
    uint16_t  b;
    mode_t    mode;
    int       p1, p2, p3, p4;
    int       p69;
  }
ctrl_t;

ctrl_t ctrl[C_CNT];

//------------------------------------------------------------------------------- 
// The state machine resets after, errr, 1.5ms?  13.5ms?  <shrug>
// ...this seems to work OK
//
#define  RST_MS  (2)

//------------------------------------------------------------------------------- 
// The event flag is set when one-or-more events have occurred during a scan
//
bool  e_flg;

//------------------------------------------------------------------------------- 
// Variables for the serial console output
#if (COMPILE == FOR_DEBUG)
# define  GAP  (3)
  char      ser_s[((B_CNT + GAP) * C_CNT) + 1];
  char      ser_b[sizeof(ser_s)];
  uint16_t  old[C_CNT];
#endif

//------------------------------------------------------------------------------- 
// For controller 'ctl': Query the internal-state of the specified Button
#define  ISPRS(ctl,btn)   ( (ctrl[ctl].b & (btn)) != 0 )
#define  ISREL(ctl,btn)   ( !ISPRS(ctl,btn) )

// For controller 'ctl':  Set  the internal-state of the specified Button
#define  MKPRS(ctl,btn)   ( ctrl[ctl].b |=  (btn))
#define  MKREL(ctl,btn)   ( ctrl[ctl].b &= ~(btn))

//------------------------------------------------------------------------------- 
// For controller 'ctl': Query the state of the specified DIGITAL db9 pin
#define  ISLO_D(ctl,pin)  ( digitalRead(ctrl[ctl].p##pin) == LOW )
#define  ISHI_D(ctl,pin)  ( !ISLO_D(ctl,pin) )

//+============================================================================== 
// For controller 'ctl': Query the state of the specified db9 pin - via the Resistor Network
//
#define  ISHI_A(ctl,db9)  ( !ISLO_A(ctl,db9) )

bool  ISLO_A (int ctl,  int db9) 
{
  int a = analogRead(ctrl[ctl].p69);  
  switch (db9) {
    case 6:   return ( ((a >= CX_P6_MIN ) && (a <= CX_P6_MAX )) ||
                       ((a >= CX_P69_MIN) && (a <= CX_P69_MAX))   ) ;
    case 9:   return ( ((a >= CX_P9_MIN ) && (a <= CX_P9_MAX )) ||
                       ((a >= CX_P69_MIN) && (a <= CX_P69_MAX))   ) ;
    default:  return false ;
  }
}

//+============================================================================== 
// Configure the ATTiny167 (Digispark Pro)
//
void setup (void) 
 {
  // -------------------------------------------------------
  // Prepare for nicely formatted serial debugging
# if (COMPILE == FOR_DEBUG)
    SerialUSB.begin();
    SerialUSB.println(F("Serial is alive"));

    memset(ser_s, ' ', sizeof(ser_s) - 1);
    ser_s[sizeof(ser_s) - 1] = '\0';
    for (int c = 0;  c < C_CNT;  c++) {
      old[c] = 0xFFFF;
      for (int b = 0;  b < B_CNT;  b++)
        ser_s[(c * (B_CNT + GAP)) + b] = '-';
    }

    memset(ser_b, '\b', sizeof(ser_b) - 1);
    ser_b[sizeof(ser_b) - 1] = '\0';
# endif

  // -------------------------------------------------------
  // Initialise the controller states
  // ...and define Pin usage
  //
  ctrl[0] = {0x0000, M_NONE, C1_P1, C1_P2, C1_P3, C1_P4, C1_P69};
  ctrl[1] = {0x0000, M_NONE, C2_P1, C2_P2, C2_P3, C2_P4, C2_P69};
  
# if (C_CNT > 2)
    ctrl[2] = {0x0000, M_NONE, C3_P1, C3_P2, C3_P3, C3_P4, C3_P69};
    ctrl[3] = {0x0000, M_NONE, C4_P1, C4_P2, C4_P3, C4_P4, C4_P69};
# endif
  
  // -------------------------------------------------------
  // Describe how the DB9's are wired to the Tiny167
  //
  for (int c = 0;  c < C_CNT;  c++) {
    pinMode(ctrl[c].p1 , INPUT_PULLUP);
    pinMode(ctrl[c].p2 , INPUT_PULLUP);
    pinMode(ctrl[c].p3 , INPUT_PULLUP);
    pinMode(ctrl[c].p4 , INPUT_PULLUP);
    pinMode(ctrl[c].p69, INPUT);
  }
  pinMode(CX_P7, OUTPUT);  // Clock pin

  // -------------------------------------------------------
  // Initialise the USB controller
# if (COMPILE == FOR_MD)
    Mega2USB.setup(REFRESH_HZ);
# endif
  
  // -------------------------------------------------------
  // Give the dev time to connect a serial monitor...
# if (COMPILE == FOR_DEBUG)
    for (int i = 5;  i > 0;  i--) {
      SerialUSB.print(i);
      SerialUSB.print("..");
      DELAY(1000);
    }
    SerialUSB.println("Go!");

    // I've stopped caring why the F() causes strings to not-print
    // ...and why removing it causes a boot-loop
    // I suspect the former is a missing/broken method for accessing flash
    // I suspect the latter is the driver failing to pat the USB watchdog when printing long strings
    // Either way, it looks like a driver issue, and that's not today's problem
    SerialUSB.print(F("DAC_BITS = "));  SerialUSB.println(DAC_BITS, DEC);
    SerialUSB.print(F("DAC_MAX  = "));  SerialUSB.println(DAC_MAX , DEC);
    SerialUSB.print(F("RG_OHM   = "));  SerialUSB.println(RG_OHM  , DEC);
    SerialUSB.print(F("R6_OHM   = "));  SerialUSB.println(R6_OHM  , DEC);
    SerialUSB.print(F("R9_OHM   = "));  SerialUSB.println(R9_OHM  , DEC);
    SerialUSB.print(F("R_PLL_69 = "));  SerialUSB.println(R_PLL_69, DEC);
    SerialUSB.print(F("R_PLL_G6 = "));  SerialUSB.println(R_PLL_G6, DEC);
    SerialUSB.print(F("R_PLL_G9 = "));  SerialUSB.println(R_PLL_G9, DEC);
    SerialUSB.print(F("CX_PX    = "));  SerialUSB.println(CX_PX   , DEC);
    SerialUSB.print(F("CX_P6    = "));  SerialUSB.println(CX_P6   , DEC);
    SerialUSB.print(F("CX_P9    = "));  SerialUSB.println(CX_P9   , DEC);
    SerialUSB.print(F("CX_P69   = "));  SerialUSB.println(CX_P69  , DEC);
    SerialUSB.print(F("DEV      = "));  SerialUSB.println(DEV     , DEC);   
    SerialUSB.println();
# endif

}

//+============================================================================== 
void  event (uint32_t ev)
{
  int  c = EV_C_GET(ev);

  e_flg = true;  // Flag an event (for the caller)

// Sadly this block (of printing) means that it can take >1.5mS between states
// ...which means you will get quirky results on a 6-Button Controller
# if 0 // (COMPILE == FOR_DEBUG)
    interrupts();
    {
      SerialUSB.print("\r\nEV:");
      SerialUSB.print(ev, HEX);
    } 
    noInterrupts();
# endif

  switch (ev & ~EVM_C) {
    case EV_ADD3 :  ctrl[c].mode = M_3BTN;  break;
    case EV_ADD6 :  ctrl[c].mode = M_6BTN;  break;
    case EV_RMV  :  ctrl[c].mode = M_NONE;  break;

    case EV_PU   :  MKPRS(c, BU);  break;
    case EV_PD   :  MKPRS(c, BD);  break;
    case EV_PL   :  MKPRS(c, BL);  break;
    case EV_PR   :  MKPRS(c, BR);  break;
    case EV_PA   :  MKPRS(c, BA);  break;
    case EV_PB   :  MKPRS(c, BB);  break;
    case EV_PC   :  MKPRS(c, BC);  break;
    case EV_PX   :  MKPRS(c, BX);  break;
    case EV_PY   :  MKPRS(c, BY);  break;
    case EV_PZ   :  MKPRS(c, BZ);  break;
    case EV_PS   :  MKPRS(c, BS);  break;
    case EV_PM   :  MKPRS(c, BM);  break;
                                
    case EV_RU   :  MKREL(c, BU);  break;
    case EV_RD   :  MKREL(c, BD);  break;
    case EV_RL   :  MKREL(c, BL);  break;
    case EV_RR   :  MKREL(c, BR);  break;
    case EV_RA   :  MKREL(c, BA);  break;
    case EV_RB   :  MKREL(c, BB);  break;
    case EV_RC   :  MKREL(c, BC);  break;
    case EV_RX   :  MKREL(c, BX);  break;
    case EV_RY   :  MKREL(c, BY);  break;
    case EV_RZ   :  MKREL(c, BZ);  break;
    case EV_RS   :  MKREL(c, BS);  break;
    case EV_RM   :  MKREL(c, BM);  break;
  }
}

//+============================================================================== 
void  scan (int c,  int s)
{
  uint32_t  evc = EV_C_SET(evc, c);

  switch (s) {
    case 0:
      if (ISLO_D(c,3) && ISLO_D(c,4)) {
        if (ctrl[c].mode == M_NONE)  ctrl[c].mode = M_CONN ;
      } else {
        if (ctrl[c].mode != M_NONE)  event(evc | EV_RMV) ;
      }
      break;
      
    case 4:
      if (ctrl[c].mode == M_NONE)  break ;
      
      if (ISLO_D(c,1) && ISLO_D(c,2)) { 
        if (ctrl[c].mode != M_6BTN)  event(evc | EV_ADD6) ;
      } else {
        if (ctrl[c].mode == M_CONN)  event(evc | EV_ADD3) ;
      }
      break;

    case 2:
      if ((ctrl[c].mode != M_3BTN) && (ctrl[c].mode != M_6BTN))  break ;
      
      if (ISLO_A(c,6))  {  if (ISREL(c,BA))  event(evc | EV_PRS | BA) ;  }  // A
      else              {  if (ISPRS(c,BA))  event(evc | EV_REL | BA) ;  }
      if (ISLO_A(c,9))  {  if (ISREL(c,BS))  event(evc | EV_PRS | BS) ;  }  // Start
      else              {  if (ISPRS(c,BS))  event(evc | EV_REL | BS) ;  }
      break;

    case 3:
      if ((ctrl[c].mode != M_3BTN) && (ctrl[c].mode != M_6BTN))  break ;
      
      if (ISLO_D(c,1))  {  if (ISREL(c,BU))  event(evc | EV_PRS | BU) ;  }  // Up
      else              {  if (ISPRS(c,BU))  event(evc | EV_REL | BU) ;  }
      if (ISLO_D(c,2))  {  if (ISREL(c,BD))  event(evc | EV_PRS | BD) ;  }  // Down
      else              {  if (ISPRS(c,BD))  event(evc | EV_REL | BD) ;  }
      if (ISLO_D(c,3))  {  if (ISREL(c,BL))  event(evc | EV_PRS | BL) ;  }  // Left
      else              {  if (ISPRS(c,BL))  event(evc | EV_REL | BL) ;  }
      if (ISLO_D(c,4))  {  if (ISREL(c,BR))  event(evc | EV_PRS | BR) ;  }  // Right
      else              {  if (ISPRS(c,BR))  event(evc | EV_REL | BR) ;  }
      
      if (ISLO_A(c,6))  {  if (ISREL(c,BB))  event(evc | EV_PRS | BB) ;  }  // B
      else              {  if (ISPRS(c,BB))  event(evc | EV_REL | BB) ;  }
      if (ISLO_A(c,9))  {  if (ISREL(c,BC))  event(evc | EV_PRS | BC) ;  }  // C
      else              {  if (ISPRS(c,BC))  event(evc | EV_REL | BC) ;  }
      break;

    case 5:
      if (ctrl[c].mode != M_6BTN)  break ;
      
      if (ISLO_D(c,1))  {  if (ISREL(c,BZ))  event(evc | EV_PRS | BZ) ;  }  // Z
      else              {  if (ISPRS(c,BZ))  event(evc | EV_REL | BZ) ;  }
      if (ISLO_D(c,2))  {  if (ISREL(c,BY))  event(evc | EV_PRS | BY) ;  }  // Y
      else              {  if (ISPRS(c,BY))  event(evc | EV_REL | BY) ;  }
      if (ISLO_D(c,3))  {  if (ISREL(c,BX))  event(evc | EV_PRS | BX) ;  }  // X
      else              {  if (ISPRS(c,BX))  event(evc | EV_REL | BX) ;  }
      if (ISLO_D(c,4))  {  if (ISREL(c,BM))  event(evc | EV_PRS | BM) ;  }  // Mode
      else              {  if (ISPRS(c,BM))  event(evc | EV_REL | BM) ;  }
      break;

    default:  
      break;
  }
}

//+============================================================================== 
void loop (void) 
{
# if (COMPILE == FOR_DEBUG)
    for (int c = 0;  c < C_CNT;  c++)  old[c] = ctrl[c].b ; 
# endif

  // -------------------------------------------------------
  // Force/allow/wait-for the controllers to reset to State-0
  //
  digitalWrite(C_CLK, LOW);
  DELAY(RST_MS);

  // -------------------------------------------------------
  // Scan all the controllers
  //   # Send (dis)connect events
  //   # Send button press/release events
  //   # Track connect/button state (internally)
  //
  noInterrupts();
  {
    e_flg = false;                                     // Clear the event flag
    for (int s = 0;  s < 8;  s++) {                    // Go through all 8 possible states
      digitalWrite(C_CLK, (s & 1) ? HIGH : LOW);       // Toggle the clk
      delayMicroseconds(1);                            // Allow for propogation (at both ends)
      for (int c = 0;  c < C_CNT;  c++)  scan(c, s) ;  // Scan the controllers
    }
  } 
  interrupts();

# if (COMPILE == FOR_MD)
    if (e_flg)
      for (int c = 0;  c < C_CNT;  c++)  
        Mega2USB.setButtons(c, ctrl[c].b);

# elif (COMPILE == FOR_DEBUG)
  // Serial output takes "forever" on the whole scheme of things
  // ...so don't expect to see "fast" button presses!
  {
    int i = 0;
    for (int c = 0;  c < C_CNT;  c++) {
      if (old[c] == ctrl[c].b) {
        i += B_CNT + GAP;
      } else {
        ser_s[i++] = ISPRS(c,BU) ? '^' : '.' ;  // 'u' ;
        ser_s[i++] = ISPRS(c,BD) ? 'v' : '.' ;  // 'd' ;
        ser_s[i++] = ISPRS(c,BL) ? '<' : '.' ;  // 'l' ; 
        ser_s[i++] = ISPRS(c,BR) ? '>' : '.' ;  // 'r' ;
        ser_s[i++] = ISPRS(c,BA) ? 'A' : '.' ;  // 'a' ;
        ser_s[i++] = ISPRS(c,BB) ? 'B' : '.' ;  // 'b' ;
        ser_s[i++] = ISPRS(c,BC) ? 'C' : '.' ;  // 'c' ;
        ser_s[i++] = ISPRS(c,BX) ? 'X' : '.' ;  // 'x' ;
        ser_s[i++] = ISPRS(c,BY) ? 'Y' : '.' ;  // 'y' ;
        ser_s[i++] = ISPRS(c,BZ) ? 'Z' : '.' ;  // 'z' ;
        ser_s[i++] = ISPRS(c,BS) ? 'S' : '.' ;  // 's' ;
        ser_s[i++] = ISPRS(c,BM) ? 'M' : '.' ;  // 'm' ;
        i += GAP;
      }
    }
    ser_s[i] = '\0';
    
    if (e_flg)  SerialUSB.println() ;
    SerialUSB.print(ser_s);
      SerialUSB.print(": ");
      SerialUSB.print( (ctrl[0].mode == M_6BTN) ? "6" : 
                      ((ctrl[0].mode == M_3BTN) ? "3" : "-"));
      SerialUSB.print( (ctrl[1].mode == M_6BTN) ? "/6" : 
                      ((ctrl[1].mode == M_3BTN) ? "/3" : "/-"));
//      SerialUSB.print("\b\b\b\b\b");
//    SerialUSB.print(ser_b);
    SerialUSB.print("\r");
  } 
# endif
  
}

