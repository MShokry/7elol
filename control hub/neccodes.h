#define DEV1 PORTA.F6
#define DEV2 PORTA.F7
#define DEV3 PORTA.F0
#define DEV4 PORTA.F1
#define DEV5 PORTA.F2
#define DEV6 PORTA.F3
#define DEV7 PORTB.F0
#define DEV8 PORTB.F3
#define ir_rx_indicator PORTB.F7 //IR Reciving indicator

#define k1 1
#define k2 2
#define k3 3
#define k4 4
#define k5 5
#define k6 6
#define k7 7
#define k8 8
#define k9 9
#define k10 10
#define k11 11
#define k12 12
#define k13 13
#define k14 14
#define k15 15
#define k16 16
#define kd 00


#define r_address 0x00
//IR Codes from NEC
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
p1s           = 0x07,                                 //alarm  period 1
p2s           = 0x15,
p3s           = 0x09,
a1s           = 0x18,                                 //alarm 1 set
a2s           = 0x1C,
a3s           = 0x52,
acl           = 0x47
;


///alarm
#define period1 15
#define period2 60
#define period3 120

