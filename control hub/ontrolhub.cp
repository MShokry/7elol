#line 1 "F:/7elol/SW/control hub/ontrolhub.c"
#line 1 "f:/7elol/sw/control hub/ontrolhub.h"
#line 46 "f:/7elol/sw/control hub/ontrolhub.h"
const char
o_1 = 0x16,
o_2 = 0x0C,
o_3 = 0x08,
o_4 = 0x42,
f_1 = 0x0D,
f_2 = 0x5E,
f_3 = 0x5A,
f_4 = 0x4A,
f_a = 0x45,
swp = 0x19,
p1s = 0x07,
p2s = 0x15,
p3s = 0x09,
a1s = 0x18,
a2s = 0x1C,
a3s = 0x52,
acl = 0x47
;
#line 38 "F:/7elol/SW/control hub/ontrolhub.c"
char ir_rx_data=0x00;
#line 50 "F:/7elol/SW/control hub/ontrolhub.c"
char
rx_tmp=0x00,
rx_data[4]
;

char
data_bit=0x00,
data_byte=0x00,
code[2]={0x00,0x00},
of=0x00;

char nec,
nec_a;


char dim[6],
dim_t[6],
df=0x00;





void interrupt();
void count();


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

 PORTB.F6 =1;
delay_ms(100);
 PORTB.F6 =0;
delay_ms(100);
 PORTB.F6 =1;
delay_ms(100);
 PORTB.F6 =0;


OPTION_REG=0b00000001;

T1CON=0b00100100;

PIE1 =0b00000001;
INTCON=0b11111000;

Usart_Init(19200);


dim[0]=40;
dim[1]=10;
while(1){
if(code[1]){
nec_a=code[0];

 nec=code[1];
 code[1]=0;
}
if(nec){
switch (nec){
 case o_1:  PORTA.f0 =1;of.f0=1;break;
 case o_2:  PORTA.f1 =1;of.f1=1;break;
 case o_3:  PORTA.f2 =1;of.f2=1;break;
 case f_1:  PORTA.f0 =0;of.f0=0;break;
 case f_2:  PORTA.f1 =0;of.f1=0;break;
 case f_3:  PORTA.f2 =0;of.f2=0;break;
 case f_a:  PORTA.f0 =0; PORTA.f1 =0; PORTA.f2 =0;
 of=of&0XF0;break;
 }
 Usart_Write(nec_a);
 Usart_Write(nec);
 }

 nec=0x00;

}

}
#line 146 "F:/7elol/SW/control hub/ontrolhub.c"
void interrupt() {

 char ii;
 if (PIR1.TMR1IF){
  ir_rx_data.F2 =1;
 T1CON.F0=0;
 count();
 PIR1.TMR1IF=0;
 }

 if(INTCON.RBIF){
 if( PORTB.F7 ){
  TMR1L =0x00;
  TMR1H =0x00;
 T1CON.F0=1;
  ir_rx_data.F1 =0;
 }
 else{
 T1CON.F0=0;
  ir_rx_data.F1 =1;
 }
 count();
 INTCON.RBIF=0;
 }


 if(INTCON.INTF){
 for(ii=0;ii<6;ii++){
 dim_t[ii]=dim[ii];
 }
  PORTB.f1 =0; PORTB.f2 =0; PORTB.f3 =0;
 df=0x00;
 TMR0=0x00;
 INTCON.TMR0IF=0;
 INTCON.INTF=0;
 }

 if(INTCON.TMR0IF){
 for(ii=0;ii<6;ii++){
 if (dim_t[ii]==0x01)
 switch (ii){
 case 0:  PORTB.f1 =1;df.f0=1;break;
 case 1:  PORTB.f2 =1;df.f1=1;break;
 case 2:  PORTB.f3 =1;df.f2=1;break;
 }
 if(dim_t[ii])
 dim_t[ii]--;
 }
 PORTD.f1=~PORTD.f1;
 INTCON.TMR0IF=0;
 }

}
#line 214 "F:/7elol/SW/control hub/ontrolhub.c"
void count(){

 if( ir_rx_data.F1 ){
 if(! ir_rx_data.F2 ){
 if( TMR1H >=0x01 &&  TMR1H <=0x03)
  ir_rx_data.F0 =0;
 else if( TMR1H >=0x07 &&  TMR1H <=0x09)
  ir_rx_data.F0 =1;
 else if( TMR1H >=0x14 &&  TMR1H <=0x16)
  ir_rx_data.F3 =1;
 else
  ir_rx_data.F3 =0;
 }
  ir_rx_data.F1 =0;

 if( ir_rx_data.F3 ){
 if( ir_rx_data.F6 ){
 if(data_bit){
 rx_tmp >>= 1;
 if( ir_rx_data.F0 )
 rx_tmp |= 0x80;
 }
 else
 if( ir_rx_data.F0 )
 rx_tmp = 0x80;

 data_bit++;
 if(data_bit >= 8){
 data_bit=0x00;
 rx_data[data_byte]=rx_tmp;
 rx_tmp=0x00;
 data_byte++;

 if(data_byte>=4){
 data_byte=0x00;
  ir_rx_data.F5 =1;
 if(!(rx_data[2]==~rx_data[3])){

 goto error;
 }
 }
 }
 }
  ir_rx_data.F6 =1;
 }else{
 goto error;
 }

 if( ir_rx_data.F5 ){
 code[0]=rx_data[0];
 code[1]=rx_data[2];
 goto error;
 }
 }

 if( ir_rx_data.F2 ){
 error:
 data_bit=0X00;
 rx_data[0]=0x00;rx_data[1]=0x00;
 rx_data[2]=0x00;rx_data[3]=0x00;

 ir_rx_data =0x00;
 }
  PORTB.F6 = ir_rx_data.F6 ;
}
