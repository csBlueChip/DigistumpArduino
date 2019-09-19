#if 0  // This whole file is documentation

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

So we can process everything nicely (readable code) with this logic:
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
                y -> Pin Number

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

#endif
