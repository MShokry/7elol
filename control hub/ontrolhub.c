//7elol Home automation  V1.00 -mini remote
//
//By: Eng.Mahmoud Shokry    Tel: 01115205568
//Date :13-jan-2013
//This file is a copywrite for 7elol ltd.
//
/*******************************************************************************
ToDo:
int
ON/OFF    6 dev
Dimmer    6 dev   + 1 inp   upto      50 leve
          4 conn or 3

NEC protocol
      one  1 inp


Touch
   keypad      8 inp
   
WireLess
   SPI         5 inp

Done:
NEC protocol
    edge detect on port b  one  1 inp    1 out
    edge detect on port b
Comment

PC connection
   USART       2 inp


******************************************************************************/

///////////////////////////////////////////////////////////////////////////////
//////////
#include "ontrolhub.h"

char ir_rx_data=0x00;               // the recived ir data control register
#define ir_rx_bit ir_rx_data.F0     //The revived data logic 1 or 0
#define ir_rx_ok ir_rx_data.F1      //Bit recived from IR
#define ir_rx_err ir_rx_data.F2     //Error reciving data pulse >.1 sec
#define ir_rx_lead ir_rx_data.F3    //Leading of code detected
#define ir_rx_adress ir_rx_data.F4  //recieving data or adderes
#define ir_rx_full ir_rx_data.F5    //code fully recived
#define ir_rx_start ir_rx_data.F6   //code Start reciving bits LSB first after lead

#define ir_rx_width_l TMR1L         //The Timer1 used for pulse width measure
#define ir_rx_width TMR1H

char
rx_tmp=0x00,
rx_data[4]
;                             //Full Frame Data 4 byte "adree adress` data data`

char
data_bit=0x00,               //Bit selector for shifting
data_byte=0x00,              //byte selector for frame data
code[2]={0x00,0x00},         //The Lo value of rx_data "adress, data
of=0x00;                     //On Off indicator          ..

char nec,                    //NEC data code readed
nec_a;                       //NEC address code
#define r_address    0x00

char dim[6],                 //Dimmer delay values
dim_t[6],                    //Dimmer delay temp for re-use
df=0x00;                     //Dimmer flag indicator
///////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////

void interrupt();            //
void count();                //


void main(){
ADCON1=0x06;
TRISA=0x00;
PORTA=0x00;
TRISB=0x81;
PORTB=0x00;
PORTC=0x00;
PORTC=0x00;
TRISD=0x00;
PORTD=0x00;

ir_rx_indicator=1;
delay_ms(100);
ir_rx_indicator=0;
delay_ms(100);
ir_rx_indicator=1;
delay_ms(100);
ir_rx_indicator=0;


OPTION_REG=0b00000001;       //RBPU' INTEDG T0CS T0SE PSA PS2 PS1 PS0
                             //up to 50 time dimmer
T1CON=0b00100100;            //— — T1CKPS1 T1CKPS0 T1OSCEN T1SYNC' TMR1CS TMR1ON
                             //1:4 prescale ,no t1 osc. , no sync, inernal OSC/4 T1 off
PIE1 =0b00000001;            //EEIE CMIE RCIE TXIE — CCP1IE TMR2IE TMR1IE
INTCON=0b11111000;           //GGIE PEIE T0IE INTE RBIE T0IF INTF RBIF

Usart_Init(19200);           //USART data rates


dim[0]=40;
dim[1]=10;
while(1){
if(code[1]){
nec_a=code[0];
//if(nec_a==r_address)
   nec=code[1];
   code[1]=0;
}
if(nec){                    //nec =code[1] the data from IR
switch (nec){
   case o_1: of1=1;of.f0=1;break;
   case o_2: of2=1;of.f1=1;break;
   case o_3: of3=1;of.f2=1;break;
   case f_1: of1=0;of.f0=0;break;
   case f_2: of2=0;of.f1=0;break;
   case f_3: of3=0;of.f2=0;break;
   case f_a: of1=0;of2=0;of3=0;
             of=of&0XF0;break;
             
   case d1u: of1=1;of.f0=1;break;
   case d2u: of2=1;of.f1=1;break;
   case d3u: of3=1;of.f2=1;break;
   case d1d: of1=0;of.f0=0;break;
   case d2d: of2=0;of.f1=0;break;
   case d3d: of3=0;of.f2=0;break;
 }
    Usart_Write(nec_a);
    Usart_Write(nec);
  }//if
    
     nec=0x00;

}//Stay forever here

}//Main fuction

/******************************************************************************
interrupts
   Timer 1 for pulse mesur
   PORTB RB7  For ir input
   INTE  external interrupt for dimmer use
   TMR0IF for dimmer delay

*******************************************************************************/

void interrupt() {           //Interrupt routines
                             //TMR1IE RBIE INTE TMR0IE
 char ii;                    //general use i_interrupt for interrupt loops
 if (PIR1.TMR1IF){           //Timer 1 => used as pulse width measure
    ir_rx_err=1;             //if over flow => data error
    T1CON.F0=0;              //Timer1 disable
    count();                 //call every interrupt routine
    PIR1.TMR1IF=0;           //clear interrupt
 }

 if(INTCON.RBIF){             // PORTB interrupt at RB7 as IR Rx
    if(ir_pin){               //If RX pin changed to high
      ir_rx_width_l=0x00;     //Timer 1 reset counter
      ir_rx_width=0x00;
      T1CON.F0=1;             //Timer1 enable to start measuring pulse width
      ir_rx_ok=0;             //ReSet bit recived indicator
      }
    else{
      T1CON.F0=0;              //Timer1 disable
      ir_rx_ok=1;              //Set bit recived indicator
      }
  count();                     //call every interrupt routine
  INTCON.RBIF=0;               //Clear the interrupt pin
 }


 if(INTCON.INTF){              //zero cross detected
   for(ii=0;ii<6;ii++){
    dim_t[ii]=dim[ii];        // reload dimmer values
   }
   d1=0;d2=0;d3=0;            //clear all out
   df=0x00;                   // dimmer flag
 TMR0=0x00;
 INTCON.TMR0IF=0;
 INTCON.INTF=0;
 }
 
 if(INTCON.TMR0IF){           //zero cross detected
    for(ii=0;ii<6;ii++){
      if (dim_t[ii]==0x01)
         switch (ii){
           case 0: d1=1;df.f0=1;break;
           case 1: d2=1;df.f1=1;break;
           case 2: d3=1;df.f2=1;break;
        }
      if(dim_t[ii])
         dim_t[ii]--;
   }
   PORTD.f1=~PORTD.f1;
 INTCON.TMR0IF=0;
 }

}

/******************************************************************************
count function
   used in measuring the width of the pulse
   Codes structure   NEC code

    A 9ms leading pulse burst (16 times the pulse burst length used for a logical data bit)
    A 4.5ms space
    The 8-bit address address' command command'
    Final 562.5µs pulse burst to show end of message transmission.
    Logical '0' – a 562.5µs pulse burst followed by a 562.5µs space, with a total transmit time of 1.125ms
    Logical '1' – a 562.5µs pulse burst followed by a 1.6875ms space, with a total transmit time of 2.25

   http://www.circuitvalley.com/2013/09/nec-protocol-ir-infrared-remote-control.html
*******************************************************************************/
void count(){
 //First read Logic1 or Logic0 Or leading "Get the width of data
 if(ir_rx_ok){               //if bit recived from ir
   if(!ir_rx_err){            //if no error test bit width
     if(ir_rx_width>=0x01 && ir_rx_width<=0x03)
       ir_rx_bit=0;          // high width >=xxx ,< xxx usec   abs 560  usec
     else if(ir_rx_width>=0x07 && ir_rx_width<=0x09)
       ir_rx_bit=1;          // high width >=xxx ,<xxx  msec  abs 1.69
      else if(ir_rx_width>=0x14 && ir_rx_width<=0x16)
       ir_rx_lead=1;         // leading start >=xxx ,<xxx  msec  abs 4.5
     else
       ir_rx_lead=0;         // No lead
    }
 ir_rx_ok=0;
//Second get the data and store it
 if(ir_rx_lead){             //if lead readed
   if(ir_rx_start){          //no to read leading indicator first time as RX bit
     if(data_bit){           //at first don't shift data in else shift
       rx_tmp >>= 1;
       if(ir_rx_bit)
         rx_tmp |= 0x80;
       }                     //data.Bit[Loop] = input bit
     else
       if(ir_rx_bit)
         rx_tmp = 0x80;
                             // at first data =rx "LSB =rx"
     data_bit++;             //select next bit
     if(data_bit >= 8){      //if readed 8 bits
       data_bit=0x00;
       rx_data[data_byte]=rx_tmp;//store the byte
       rx_tmp=0x00;         //clear the temp data
       data_byte++;         //next byte

       if(data_byte>=4){    //4 bytes recived
         data_byte=0x00;
         ir_rx_full=1;       //code fully recived
         if(!(rx_data[2]==~rx_data[3])){
                             //If data isn't 100% right and good recived
          goto error;
         }
       }
    }
  }
  ir_rx_start=1;
 }else{                      //leding = 0
  goto error;
 }
//Third if data fully ok
  if(ir_rx_full){
  code[0]=rx_data[0];
  code[1]=rx_data[2];
    goto error;
  }
 }
//Finally on error reset all data and clear all registes
 if(ir_rx_err){              //at any time error reset all
 error:
     data_bit=0X00;             //shifting rx data
     rx_data[0]=0x00;rx_data[1]=0x00;
     rx_data[2]=0x00;rx_data[3]=0x00;
                             //clear all data
     ir_rx_data =0x00;      //clear all RX parameters
 }
 ir_rx_indicator=ir_rx_start;
}



/*****************************************************************************











******************************************************************************/