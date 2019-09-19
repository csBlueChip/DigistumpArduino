#if 0  // This whole file is documentation   ***REJECTED IDEA***

Frees up a single pin,
at the cost of a shedload of resistors,
and only works if TWO controllers are connected!
...Ahh, the joys of new information AFTER you've designed the circuit :/

=================
 An "R2R Ladder"
=================

An R2R ladder allows you to connect many Digital Inputs to
a single analogue GPIO pin using only resistors.

The value of the resistors are either R or 2R
...where R is an arbitrary choice ...10K works well, and will draw no more than ~235uA

...Generally, the tolerance of the resistors MUST be BETTER THAN
   (100 / (2 ^ N))  ...where N is the number of pins
EG.  4 pins  ->  (100 / (2 ^ 4)))  ->  (100 /  16)  ->  6.25%   --> 5%
     5 pins  ->  (100 / (2 ^ 5)))  ->  (100 /  32)  ->  3.125%  --> 2%
     6 pins  ->  (100 / (2 ^ 6)))  ->  (100 /  64)  ->  1.563%  --> 1%
     7 pins  ->  (100 / (2 ^ 7)))  ->  (100 / 128)  ->  0.781%  --> 0.1%
	 :| :     :    :        :       :    :     :    :     :     --> 0.1%
    10 pins  ->  (100 / (2 ^ 9)))  ->  (100 / 128)  ->  0.781%  --> <0.097656%

In tests I found that the 9th and 10th order bits (aka bits 2^8 and 2^9) on 
a home-built ATTiny167 based circuit gave random values due to RF inteference.
...So there is probably no point in going beyond 8 pins - which can be done
   ...with 0.1% tolerance "Precision" resistors (or 0.1% SMD resistors)

There's no harm in over-compensating, so:
   I recommend using 2% resisitors to make this 4-bit R2R Ladder (DAC) rock-solid

         :           :           :           :
 #=======:===========:===========:===========:========#
 #       :       R   :       R   :       R   :        #
 #  +----:--+--^v^v--:--+--^v^v--:--+--^v^v--:--+----->-[Out] --> [Analogue GPIO Pin]
 #  |    :  |        :  |        :  |        :  |     #
 #  <    :  <        :  <        :  <        :  <     #
 #  > 2R :  > 2R     :  > 2R     :  > 2R     :  > 2R  #
 #  <    :  <        :  <        :  <        :  <     #
 #  |    :  |        :  |        :  |        :  |     #
 #==|====:==^========:==^========:==^========:==^=====#
    |    :  |        :  |        :  |        :  |
  [Gnd]  : [1]       : [2]       : [4]       : [8]   <-- Value
         : 2^0       : 2^1       : 2^2       : 2^3   <-- Bit number


Circuit Symbol:
   ,------.
   |0)  (o|
   |1)    |
   |2)    |
   |3)  (g|
   `------'

One small problem is you are not going to find resistors of value '2R'
unless you spent a LOT of money ...so you will need to use 2 'R' resistors in series
New information: 
   Metal film resistors come in 10K and 20K :D 
   ...and are inexpensive even at 1% :D

So given you want N bits, you will need this many resistors:
    ___R___   ___2R___
    (N - 1) + 2(N + 1)
EG. (4 - 1) + 2(4 + 1)  =>  3 + (2 * 5)  =>  13

If you use Proto-board, you can join the holes with 0805 SMD resistors
...so you will need a piece of board of dimentions >= {x, y} where:
  x = N + 2,  and  y = 3

    o--o##o##o##o--o
    #  #  #  #  #  |
    o  o  o  o  o  o
    #  #  #  #  #  |
    o  o  o  o  o  o
    |  |  |  |  |  |
    G  0  1  2  3  O

If you want to do it with through-hole resistors, there are many solutions for you to find!

=============================================
 Connecting the Controllers to the ATTiny167
=============================================

[5v0]---+-------+--------------------------------[5v0]
        |       |
        |       | ,------------------------------[GPIO-D6]
        |       | | ,----------------------------[GPIO-D5]
        |       | | | ,--------------------------[GPIO-D2]
        |       | | | | ,------------------------[GPIO-D0]
        |       | | | | |
        |    .--|-|-|-|-|--.
        |     \ 5 4 3 2 1 / DB9/1
        |      \ 9 8 7 6 /
        |       `|-|-|-|'  .--------(Clk)--------[GPIO-D8]
        |        | | | |   |
        |        | | | `---|----.
        |        `-|-|-----|--. |  ,------.
        |          | +-----'  | `--|3)  (o|------[GPIO-A12]
        |  .-------+ |        `----|2)    |
        |  |     ,-|-|-------------|1)    |
        |  |     | | | ,-----------|0)  (g|--+
        |  |     | | | |           `------'  |
        |  |    ,|-|-|-|.                    |
        |  |   / 9 8 7 6 \                   |
        |  |  / 5 4 3 2 1 \ DB9/2            |
        |  | '--|-|-|-|-|--`                 |
        |  |    | | | | |                    |
        `--|----' | | | `--------------------|---[GPIO-D7]
           |      | | `----------------------|---[GPIO-D9]
           |      | `------------------------|---[GPIO-D10]
           |      `--------------------------|---[GPIO-D11]
           |                                 |
[Gnd]------+---------------------------------+---[Gnd]

#endif
