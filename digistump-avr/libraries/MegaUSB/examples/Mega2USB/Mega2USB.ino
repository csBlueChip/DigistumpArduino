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

#elif (COMPILE == FOR_MD)
#   include "Mega2USB.h"
#   define DELAY(ms)   Mega2USB.delay(ms)
#   define REFRESH_HZ  (60)

#else
#   error Unknown COMPILE method!
#endif

//-------------------------------------------------------------------------------
// ATTiny167 wiring
//
#define  C_CNT    (2)      // Number of supported controllers

#define  CX_P7    (8)      // [output] : Clock/Select
#define  C_CLK    (CX_P7)  // Pseudonyms
#define  C_SEL    (CX_P7)

# define  C1_P1   (0 )     // [input] Controller-1/Pin-1  -->  GPIO_D0
# define  C1_P2   (1 )     // [input] Controller-1/Pin-2  -->  GPIO_D1
# define  C1_P3   (2 )     // [input] Controller-1/Pin-3  -->  GPIO_D2
# define  C1_P4   (7 )     // [input] Controller-1/Pin-4  -->  GPIO_D7
# define  C1_P69  (A6)     // [input] Controller-1/Pins-{6,9}  -->  Resistor Network  -->  GPIO_A6

#if (C_CNT >= 2)
# define  C2_P1   (9 )     // [input] Controller-2/Pin-1  -->  GPIO_D9
# define  C2_P2   (10)     // [input] Controller-2/Pin-2  -->  GPIO_D10
# define  C2_P3   (11)     // [input] Controller-2/Pin-3  -->  GPIO_D11
# define  C2_P4   (12)     // [input] Controller-2/Pin-4  -->  GPIO_D12
# define  C2_P69  (A5)     // [input] Controller-2/Pins-{6,9}  -->  Resistor Network  -->  GPIO_A5
#endif

#if (C_CNT >= 3)
# error No pins defined for controller 3
# define  C3_P1   (? )     // [input] Controller-3/Pin-1  -->  GPIO_D?
# define  C3_P2   (? )     // [input] Controller-3/Pin-2  -->  GPIO_D?
# define  C3_P3   (? )     // [input] Controller-3/Pin-3  -->  GPIO_D?
# define  C3_P4   (? )     // [input] Controller-3/Pin-4  -->  GPIO_D?
# define  C3_P69  (? )     // [input] Controller-3/Pins-{6,9}  -->  Resistor Network  -->  GPIO_A?
#endif

#if (C_CNT == 4)
# error No pins defined for controller 4
# define  C4_P1   (? )     // [input] Controller-4/Pin-1  -->  GPIO_D?
# define  C4_P2   (? )     // [input] Controller-4/Pin-2  -->  GPIO_D?
# define  C4_P3   (? )     // [input] Controller-4/Pin-3  -->  GPIO_D?
# define  C4_P4   (? )     // [input] Controller-4/Pin-4  -->  GPIO_D?
# define  C4_P69  (? )     // [input] Controller-4/Pins-{6,9}  -->  Resistor Network  -->  GPIO_A?
#endif

#if (C_CNT > 4)
# error MegaDrive supports a maximum of 4 controllers
#endif

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
#define CX_P6     ((int)((DAC_MAX / (R_PLL_G9 + RG_OHM)) * R_PLL_G9))  //   Pressed: Pin-6
#define CX_P9     ((int)((DAC_MAX / (R_PLL_G6 + R9_OHM)) * R_PLL_G6))  //   Pressed: Pin-9 
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

// The maximum deviation allowed (caused by eg. resistor tolerances, wiring defects, RF inteference, etc.)
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
// The state machine resets after, errr, 1.5ms?  13.5ms?  <shrug>
// ...this seems to work OK
#define  RST_MS       (2)

// We need to allow time for logic levels to propagate before taking readings
// ...this seems to work OK
#define  PROPAGATE_US (5)

//-------------------------------------------------------------------------------
// Controller type (detected)
//
typedef
  enum mode {
    M_NONE = 0x00,           // No controller connected
    M_CONN = 0x80,           // Controller connected, not yet identified
    M_3BTN = 0x03 | M_CONN,  // Controller idenitified at 6-button
    M_6BTN = 0x06 | M_CONN,  // Controller idenitified at 6-button
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
// Variables for the serial console output
#if (COMPILE == FOR_DEBUG)
# define  GAP  (3)                             // Space between data
  char    ser_s[((B_CNT + GAP) * C_CNT) + 1];  // Friendly-output buffer
  int     e_flg;                               // An event occurred
#endif

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
      for (int b = 0;  b < B_CNT;  b++)
        ser_s[(c * (B_CNT + GAP)) + b] = '-';
    }
# endif

  // -------------------------------------------------------
  // Initialise the controller states
  // ...and define Pin usage
  //
  ctrl[0] = {0x0000, M_NONE, C1_P1, C1_P2, C1_P3, C1_P4, C1_P69};

# if (C_CNT >= 2)
    ctrl[1] = {0x0000, M_NONE, C2_P1, C2_P2, C2_P3, C2_P4, C2_P69};
# endif
# if (C_CNT >= 3)
    ctrl[2] = {0x0000, M_NONE, C3_P1, C3_P2, C3_P3, C3_P4, C3_P69};
# endif

# if (C_CNT == 4)
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
/*
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
*/
# endif
}

//+==============================================================================
// For controller 'ctl': Query the state of the specified db9 pin - via the Resistor Network
//
#define  ISHI_A(ctl,db9)  ( !ISLO_A(ctl,db9) )

static inline  bool  ISLO_A (int ctl,  int db9)
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

//-------------------------------------------------------------------------------
// For controller 'ctl': Query the state of the specified DIGITAL db9 pin
#define  ISLO_D(ctl,pin)  ( digitalRead(ctrl[ctl].p##pin) == LOW )
#define  ISHI_D(ctl,pin)  ( !ISLO_D(ctl,pin) )

//-------------------------------------------------------------------------------
// For controller 'ctl': Query the internal-state of the specified Button
#define  ISPRS(ctl,btn)   ( (ctrl[ctl].b & (btn)) != 0 )
#define  ISREL(ctl,btn)   ( !ISPRS(ctl,btn) )

//-----------------------------------------------------------------------------
// Sadly this block (of printing) means that it can take >1.5mS between states
// ...which means you will get quirky results on a 6-Button Controller
# if (COMPILE == FOR_DEBUG) && 0
void  ev_show (int c,  char* s,  int n)
{
  SerialUSB.print("\r\nEV: ");
  SerialUSB.print(c);
  SerialUSB.print(s);
  SerialUSB.print(n, HEX);
}

# else
#   define  ev_show(...)
# endif

//+==============================================================================
static inline  void  event_prs (int c,  int btn)
{
  ctrl[c].b |= btn;
  ev_show(c, "+", btn);

# if (COMPILE == FOR_DEBUG)
    e_flg = true;
# endif    
}

//+==============================================================================
static inline  void  event_rel (int c,  int btn)
{
  ctrl[c].b &= ~btn;
  ev_show(c, "-", btn);

# if (COMPILE == FOR_DEBUG)
    e_flg = true;
# endif    
}

//+==============================================================================
static inline  void  event_conn (int c,  mode_t m)
{
  ctrl[c].mode = m;
  ev_show(c, "=", m);

# if (COMPILE == FOR_DEBUG)
    e_flg = true;
# endif    
}

//+==============================================================================
static inline  void  scan (int c,  int s)
{
  switch (s) {
    case 0:
      if (ISLO_D(c,3) && ISLO_D(c,4)) {
        if (ctrl[c].mode == M_NONE)  ctrl[c].mode = M_CONN ;
      } else {
        if (ctrl[c].mode != M_NONE)  event_conn(c, M_NONE) ;
      }
      break;
      
    case 4:
      if (ctrl[c].mode == M_NONE)  break ;
      
      if (ISLO_D(c,1) && ISLO_D(c,2)) { 
        if (ctrl[c].mode != M_6BTN)  event_conn(c, M_6BTN) ;
      } else {
        if (ctrl[c].mode == M_CONN)  event_conn(c, M_3BTN) ;
      }
      break;

    case 2:
      if ((ctrl[c].mode != M_3BTN) && (ctrl[c].mode != M_6BTN))  break ;
      
      if (ISLO_A(c,6))  {  if (ISREL(c,BA))  event_prs(c, BA) ;  }  // A
      else              {  if (ISPRS(c,BA))  event_rel(c, BA) ;  }
      if (ISLO_A(c,9))  {  if (ISREL(c,BS))  event_prs(c, BS) ;  }  // Start
      else              {  if (ISPRS(c,BS))  event_rel(c, BS) ;  }
      break;

    case 3:
      if ((ctrl[c].mode != M_3BTN) && (ctrl[c].mode != M_6BTN))  break ;
      
      if (ISLO_D(c,1))  {  if (ISREL(c,BU))  event_prs(c, BU) ;  }  // Up
      else              {  if (ISPRS(c,BU))  event_rel(c, BU) ;  }
      if (ISLO_D(c,2))  {  if (ISREL(c,BD))  event_prs(c, BD) ;  }  // Down
      else              {  if (ISPRS(c,BD))  event_rel(c, BD) ;  }
      if (ISLO_D(c,3))  {  if (ISREL(c,BL))  event_prs(c, BL) ;  }  // Left
      else              {  if (ISPRS(c,BL))  event_rel(c, BL) ;  }
      if (ISLO_D(c,4))  {  if (ISREL(c,BR))  event_prs(c, BR) ;  }  // Right
      else              {  if (ISPRS(c,BR))  event_rel(c, BR) ;  }
      
      if (ISLO_A(c,6))  {  if (ISREL(c,BB))  event_prs(c, BB) ;  }  // B
      else              {  if (ISPRS(c,BB))  event_rel(c, BB) ;  }
      if (ISLO_A(c,9))  {  if (ISREL(c,BC))  event_prs(c, BC) ;  }  // C
      else              {  if (ISPRS(c,BC))  event_rel(c, BC) ;  }
      break;

    case 5:
      if (ctrl[c].mode != M_6BTN)  break ;
      
      if (ISLO_D(c,1))  {  if (ISREL(c,BZ))  event_prs(c, BZ) ;  }  // Z
      else              {  if (ISPRS(c,BZ))  event_rel(c, BZ) ;  }
      if (ISLO_D(c,2))  {  if (ISREL(c,BY))  event_prs(c, BY) ;  }  // Y
      else              {  if (ISPRS(c,BY))  event_rel(c, BY) ;  }
      if (ISLO_D(c,3))  {  if (ISREL(c,BX))  event_prs(c, BX) ;  }  // X
      else              {  if (ISPRS(c,BX))  event_rel(c, BX) ;  }
      if (ISLO_D(c,4))  {  if (ISREL(c,BM))  event_prs(c, BM) ;  }  // Mode
      else              {  if (ISPRS(c,BM))  event_rel(c, BM) ;  }
      break;

    default:
      break;
  }
}

//+==============================================================================
void loop (void)
{
  // -------------------------------------------------------
  // Note current button state (will be "old" after scan)
  uint16_t  old[C_CNT];
  
  for (int c = 0;  c < C_CNT;  c++)  old[c] = ctrl[c].b ;

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
  // This chunk of code is timing critical for the 6-Button Controller
  // ...so we disable interrupts for the duration of the scan
  noInterrupts();
  {
    // We do not support third-party controllers (yet), 
    // ...so we can eschew State-6 & State-7 
    // ...to minimise the amount of time for which interrupts are diabled
    // ...probably overkill, but good coding practice
    for (int s = 0;  s <= 5;  s++) {
      digitalWrite(C_CLK, (s & 1) ? HIGH : LOW);       // Set the clk
      delayMicroseconds(PROPAGATE_US);                 // Allow for propogation (at both ends)
      for (int c = 0;  c < C_CNT;  c++)  scan(c, s) ;  // Scan the controllers
    }
  }
  interrupts();

# if (COMPILE == FOR_MD)
    for (int c = 0;  c < C_CNT;  c++)
      Mega2USB.setButtons(c, ctrl[c].b) ;


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
    SerialUSB.print("\r");
    e_flg = false;
  }
# endif

}

