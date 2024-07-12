#include <16F887.h>
#fuses NOMCLR NOBROWNOUT NOLVP INTRC_IO
#use delay(clock = 8MHz)
#use fast_io(B)
//#use I2C(master, I2C1, FAST = 100000)
#use   i2c(master,fast,sda=pin_c4,scl=pin_c3)
#include <mpu6050.c>
#USE RS232(baud =9600, xmit = pin_c6, rcv=pin_c7)
//!#include <math.h>

signed int8 A_data[6];
signed int8 G_data[6];
signed int16 Xa=0,Ya=0,Za=0;
signed int16 Xg=0,Yg=0,Zg=0;
signed int16 Xg_p,Yg_p,Zg_p;
//!signed int16 Xa_p,Ya_p,Za_p;
//!signed int16 AngleRoll, AnglePitch;

int bdn2; 
int1 tthd=0;

int1 inputcd(unsigned int8 pin){
   if(input(pin)==0){
      delay_ms(20);
      if(input(pin)==0){
         while(input(pin)==0);
         return 0;
      }
   }
   return 1;
}
void main(){
  setup_oscillator(OSC_8MHZ);                    // Set internal oscillator to 8MHz
  port_b_pullups(0xff); 
  //set_tris_c(0x80);
   //output_bit(PIN_C6, 0);
   //output_bit(PIN_C7, 1);
  set_tris_c(0x03);
  //lcd_init();  
  mpu6050_init();
  //lcd_putc('\f');                                // LCD clear
  
   setup_timer_2(T2_DIV_BY_4,240,16);
   set_timer2(0);
   enable_interrupts(INT_TIMER2);
   
   enable_interrupts(INT_RDA);
   enable_interrupts(GLOBAL);
   
  while(TRUE){
   mpu6050_write(0x1A,  0x05);     // low pass filter cut off 10khz
   //delay_ms(10);
   mpu6050_write(0x1c,  0x10);     //  thang do gia toc ke +-8g tuong ung voi do nhay 4096 LSB/g
   //delay_ms(10);
   A_data[0]=mpu6050_read(0x3B); //Read X axis(LSB)
   A_data[1]=mpu6050_read(0x3C); //Read X axis(MSB)
   A_data[2]=mpu6050_read(0x3D); //Read Y axis(LSB)
   A_data[3]=mpu6050_read(0x3E); //Read Y axis(MSB)
   A_data[4]=mpu6050_read(0x3F); //Read Z axis(LSB)
   A_data[5]=mpu6050_read(0x40); //Read Z axis(MSB)
   mpu6050_write(0x1B,  0x8);      // cai dat he so chuyen doi ti le 65.5
   //delay_ms(10);
   G_data[0]=mpu6050_read(0x43); //Read X axis(LSB)
   G_data[1]=mpu6050_read(0x44); //Read X axis(MSB)
   G_data[2]=mpu6050_read(0x45); //Read Y axis(LSB)
   G_data[3]=mpu6050_read(0x46); //Read Y axis(MSB)
   G_data[4]=mpu6050_read(0x47); //Read Z axis(LSB)
   G_data[5]=mpu6050_read(0x48); //Read Z axis(MSB)
    
   Xa=make16(A_data[0],A_data[1]);
   Ya=make16(A_data[2],A_data[3]);
   Za=make16(A_data[4],A_data[5]);
  
   Xg=make16(G_data[0],G_data[1]);
   Yg=make16(G_data[2],G_data[3]);
   Zg=make16(G_data[4],G_data[5]);
   
   Xg_p= (signed int16) (Xg/65.5 +4);
   Yg_p= (signed int16) (Yg/65.5 -1);
   Zg_p= (signed int16) (Zg/65.5);
   
//!   Xa_p= (signed int16) (Xa/4096);
//!   Ya_p= (signed int16) (Ya/4096);
//!   Za_p= (signed int16) (Za/4096);
//!   
//!   AngleRoll  = atan(Ya_p / sqrt(Xa_p*Xa_p + Za_p*Za_p)) *1/(3.142/180);
//!   AnglePitch = atan(Xa_p / sqrt(Ya_p*Ya_p + Za_p*Za_p)) *1/(3.142/180);
//!   
//!           if(AnglePitch>45)  {for(int i =0; i<1; i++) putc(2);}
//!   else if(AnglePitch<-45) {for(int i =0; i<1; i++) putc(1);}
//!   else if(AngleRoll>45)  {for(int i =0; i<1; i++) putc(4);}
//!   else if(AngleRoll<-45) {for(int i =0; i<1; i++) putc(3);}
   
        if(Yg_p>80)  {for(int i =0; i<1; i++) putc(2);}
   else if(Yg_p<-80) {for(int i =0; i<1; i++) putc(1);}
   else if(Xg_p>80)  {for(int i =0; i<1; i++) putc(4);}
   else if(Xg_p<-80) {for(int i =0; i<1; i++) putc(3);}
   
//!   lcd_gotoxy(1,1);
//!   printf(lcd_putc,"X:%ld  ",Xg_p);
//!   lcd_gotoxy(10,1);
//!   printf(lcd_putc,"Y:%ld  ",Yg_p);
//!   lcd_gotoxy(1,2);
//!   printf(lcd_putc,"Z:%ld  ",Zg_p);
//!   lcd_gotoxy(9,4);
//!   printf(lcd_putc,"data :%d  ",tthd);

   
   //while(!input(pin_b0)) {if(!inputcd(pin_b1)) tthd=~tthd; else {if(tthd==0)putc(1);}}
   //while(!input(pin_b1)) {if(!inputcd(pin_b0)) tthd=~tthd; else {if(tthd==0)putc(2);}}
   
  }
}

// dinh thoi 100 ms
#INT_TIMER2
void ngattimer2()       
{  
   bdn2++;
   if(bdn2>=13)
   {
   bdn2=0;

   if(!input(pin_b0)) putc(1);
   else if(!input(pin_b1)) putc(2);
   else if(!input(pin_b2)) {for(int i =0; i<10; i++) putc(3);}
   else if(!input(pin_b3)) putc(4);     
   else putc(0);
   
   }
}
