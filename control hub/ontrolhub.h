//7elol Home automation  V1.00 -mini remote
//
//By: Eng.Mahmoud Shokry    Tel: 01115205568
//Date :13-jan-2013
//This file is a copywrite for 7elol ltd.
//
/*******************************************************************************
structure:
  cable  coonection std.
  1   inp                              _________
  2   inp                             \  1   2 \
  3   inp                             \        \
  4   inp                             \  3   4
  5   gnd                             \
  6   gnd                             \  5   6
  7   5V                              \        \
  8   12V                             \  7   8 \
                                      \________\
 Vars
 of = on off devices
 d = dimmer
 
******************************************************************************/


#define of1 PORTD.f0
#define of2 PORTD.f1
#define of3 PORTD.f2
#define d1 PORTD.f3
#define d2 PORTD.f4
#define d3 PORTD.f5

#define ir_pin PORTB.F4
#define ir_rx_indicator PORTB.F3



/*******************************************************************************
IR remote constants
Open 1 o_1
******************************************************************************/

const char
o_1           = 0x16,
o_2           = 0x0C,
o_3           = 0x08,
o_4           = 0x42,
f_1           = 0x0D,
f_2           = 0x5E,
f_3           = 0x5A,
f_4           = 0x4A,
f_a           = 0x45,
swp           = 0x19,
d1u           = 0x07,                                 //
d2u           = 0x15,
d3u           = 0x09,
d1d           = 0x07,                                 //
d2d           = 0x15,
d3d           = 0x09,

a1s           = 0x18,                                 //alarm 1 set
a2s           = 0x1C,
a3s           = 0x52,
acl           = 0x47
;