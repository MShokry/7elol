 //7elo Home automation  V1.00 -mini remote
//
//By: Eng.Mahmoud Shokry    Tel: 01115205568
//Date :13-jan-2013




/*******************************************************************************
ToDo:
int
ON/OFF
Dimmer

PC connection
   USART

NEC protocol
    mini remote
    edge detect on port b  one

Touch
   keypad

WireLess
   SPI

Done:


//TODO Final Project
*******************************************************************************
21032013    timer1
01042013    + swap control all
*******************************************************************************/
#include "neccodes.h"
char adress;
char swap=0x00;
char sec=0x00,msec=0x00;     //4*10^6/(4*200*4*10)=125
#define rooms 2
#define alarms 3
char period=period1,alarm0[3],alarm1[3],ai,aj;
char ir_mask=0x70,ir_pin=0x00;
char edge_old=0x00,edge_new=0x00,edge=0x00;

char
rx_tmp=0x00,
rx_data[4]
;                             //Full Frame Data 4 byte "adree adress` data data`
char
data_bit=0x00,               //Bit selector for shifting
data_byte=0x00,              //byte selector for frame data
code[2]={0x00,0x00},         //The Lo value of rx_data "adress, data
room=0x00,                   //Room to control

on_off=0x00;                 //for Device to PC

//#define nec code[1]          //nec for shorter use of the data from the frame data
char nec=0x00;               //nec for shorter use of the data from the frame data and PC

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

void interrupt();
void count();      //Pulse procesing function

void main(){
CMCON=0x07;                  // compartor off all digital
TRISA=0x00;
PORTA=0x00;
TRISB=0b01110000;                 //
PORTB=0x06;

ir_rx_indicator=1;
delay_ms(500);
ir_rx_indicator=0;
delay_ms(500);
ir_rx_indicator=1;
delay_ms(500);
ir_rx_indicator=0;

OPTION_REG=0b00001000;       //RBPU' INTEDG T0CS T0SE PSA PS2 PS1 PS0
T1CON=0b00000100;            //— — T1CKPS1 T1CKPS0 T1OSCEN T1SYNC' TMR1CS TMR1ON
                             //1:8 prescale ,no t1 osc. , no sync, inernal OSC/4 T1 off
PIE1 =0b00000011;            //EEIE CMIE RCIE TXIE — CCP1IE TMR2IE TMR1IE
INTCON=0b11001000;           //GGIE PEIE T0IE INTE RBIE T0IF INTF RBIF
PR2=199;
T2CON=0b01001101;            //- TOUTPS2 TOUTPS1 TOUTPS0 TMR2ON T2CKPS1 T2CKPS0

while(1){                    //loop FOR ever

if(code[1]){
adress=code[0];
 if(adress==r_address)
   nec=code[1];
  code[1]=0;
}else if(Usart_Data_Ready()){
 nec=Usart_Read();

}
 if(nec){                    //nec =code[1] the data from IR
         if(swap==1)
            room=0x10;
         if(swap==2)
            room=0x20;
         switch (nec){
         case p1s: period=period1;room=0x00;break;
         case p2s: period=period2;room=0x00;break;
         case p3s: period=period3;room=0x00;break;
         case acl: period=0x00   ;room=0x00;break;
         }

         if(room==0x10 ){    //master room
         switch (nec){
         case o_1: DEV1=1;on_off.f0=1;break;
         case o_2: DEV2=1;on_off.f1=1;break;
         case o_3: DEV3=1;on_off.f2=1;break;
         case o_4: DEV4=1;on_off.f3=1;break;
         case f_1: DEV1=0;on_off.f0=0;break;
         case f_2: DEV2=0;on_off.f1=0;break;
         case f_3: DEV3=0;on_off.f2=0;break;
         case f_4: DEV4=0;on_off.f3=0;break;
         case f_a: DEV1=0;DEV2=0;DEV3=0;DEV4=0;
                   on_off=on_off&0XF0;break;
         case swp: if(swap)swap=0x00;else swap=0x02;break;

         case a1s: alarm0[0]=period;break;
         case a2s: alarm0[1]=period;break;
         case a3s: alarm0[2]=period;break;
         }
       }else if(room==0x20){
         switch (nec){
         case o_1: DEV5=1;on_off.f4=1;break;
         case o_2: DEV6=1;on_off.f5=1;break;
         case o_3: DEV7=1;on_off.f6=1;break;
         case o_4: DEV8=1;on_off.f7=1;break;
         case f_1: DEV5=0;on_off.f4=0;break;
         case f_2: DEV6=0;on_off.f5=0;break;
         case f_3: DEV7=0;on_off.f6=0;break;
         case f_4: DEV8=0;on_off.f7=0;break;
         case f_a: DEV5=0;DEV6=0;DEV7=0;DEV8=0;
                   on_off=on_off&0x0F;break;
         case swp: if(swap)swap=0x00;else swap=0x01;break;

         case a1s: alarm1[0]=period;break;
         case a2s: alarm1[1]=period;break;
         case a3s: alarm1[2]=period;break;
         }
       }

    }//if
     nec=0x00;

    if(sec>=60){
       sec-=60;

          for(ai=0;ai<alarms;ai++){
             if(alarm0[ai]==0x01){
                 switch (ai){
                  case 0: DEV2=0;on_off.f1=0;break;
                  case 1: DEV3=0;on_off.f2=0;break;
                  case 2: DEV4=0;on_off.f3=0;break;
                 }
               alarm0[ai]=0x00;
               }
             else if(alarm0[ai] >0x01)
              -- alarm0[ai];
            }

            for(ai=0;ai<alarms;ai++){
             if(alarm1[ai]==0x01){
                 switch (ai){
                  case 0: DEV6=0;on_off.f5=0;break;
                  case 1: DEV7=0;on_off.f6=0;break;
                  case 2: DEV8=0;on_off.f7=0;break;
                 }
               alarm1[ai]=0x00;
               }
            else if(alarm1[ai] >0x01)
              -- alarm1[ai];
            }
    }


 }//while
}//main
/******************************************************************************
interrupts
   Timer 1 for pulse mesur
   Timer 2 for Second "timer"
   PORTB RB4 to RB7  For ir input


*******************************************************************************/

void interrupt() {           //Interrupt routines
                             // T0IE INTE TMR1IE RBIE
 char ii;                    //general use i_interrupt for interrupt loops
 if (PIR1.TMR1IF){           //Timer 1 => used as pulse width measure
    ir_rx_err=1;             //data error
    T1CON.F0=0;              //Timer1 disable
    count();                    //call every interrupt routine
    PIR1.TMR1IF=0;           //<=First error i forget this
 }
 ////////////////////////HERE???????????????????
 ///If Multi IR Rx Get the First Pressed
 if(INTCON.RBIF){            // PORTB interrupt at RB4 to RB7 PB
  edge_new=PORTB;
  edge=edge_new^edge_old;
  edge=edge & ir_mask;
  ir_pin=edge_new & ir_mask;

  if(edge){                 //If RX pin changed !!!
    if(ir_pin){                 //If RX pin changed to high
      ir_rx_width_l=0x00;      //Timer 1 reset counter
      ir_rx_width=0x00;
      T1CON.F0=1;              //Timer1 enable to start measuring pulse width
      ir_rx_ok=0;              //ReSet bit recived indicator
      }
    else{
      T1CON.F0=0;              //Timer1 disable
      ir_rx_ok=1;              //Set bit recived indicator
      }
   }
  edge_old=edge_new;
  count();                    //call every interrupt routine
  INTCON.RBIF=0;             //Clear the interrupt pin
 }
 if(PIR1.TMR2IF){
  msec++;
  if(msec>=125){
    sec++;
    //PORTA.F4=~PORTA.F4;
    msec=0;
  }
  PIR1.TMR2IF=0;
 }
}

void count(){
 //First read Logic1 or Logic0 Or leading "Get the width of data
  if(ir_mask==0x70)
   if(edge){
    ir_mask=edge;
    room=ir_mask;
    }
 if(ir_rx_ok){               //if bit recived from ir
   if(!ir_rx_err){           //if no error test bit width
     if(ir_rx_width>=0x01 && ir_rx_width<=0x03)
       ir_rx_bit=0;          // high width >=424 ,< 819 usec   abs 560
     else if(ir_rx_width>=0x05 && ir_rx_width<=0x07)
       ir_rx_bit=1;          // high width >=1.23 ,<2.05  msec  abs 1.69
      else if(ir_rx_width>=0x10 && ir_rx_width<=0x13)
       ir_rx_lead=1;         // leading start >=4.1 ,<4.9  msec  abs 4.5

       ////copy under
     else
       ir_rx_lead=0;
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

       if(data_byte>=4){
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
     ir_mask=0x70;
     rx_data[0]=0x00;rx_data[1]=0x00;
     rx_data[2]=0x00;rx_data[3]=0x00;
                             //clear all data
     ir_rx_data =0x00;      //clear all RX parameters
 }
 ir_rx_indicator=ir_rx_start;
}


/*
OPTION_REG RBPU INTEDG T0CS T0SE PSA PS2 PS1 PS0
T1CON — — T1CKPS1 T1CKPS0 T1OSCEN T1SYNC TMR1CS TMR1ON --00 0000
INTCON GIE PEIE TMR0IE INTE RBIE TMR0IF INTF RBIF
PIE1 PSPIE ADIE RCIE TXIE SSPIE CCP1IE TMR2IE TMR1IE
PIR1 PSPIF ADIF RCIF TXIF SSPIF CCP1IF TMR2IF TMR1IF
*/

/*
  if(ir_rx_width>=0x01 && ir_rx_width<0x03)
       ir_rx_bit=0;          // high width >=424 ,< 819 usec   abs 560
     else if(ir_rx_width>=0x06 && ir_rx_width<=0x07)
       ir_rx_bit=1;          // high width >=1.23 ,<2.05  msec  abs 1.69
      else if(ir_rx_width>=0x11 && ir_rx_width<=0x13)
       ir_rx_lead=1;         // leading start >=4.1 ,<4.9  msec  abs 4.5

*/