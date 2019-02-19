/*
 * Author: SMRAZA KEEN
 * Date:2016/9/9
 * IDE :wiringPi
 * Address:www.amazon.com/shops/smraza
 * Function: RGBled test
 * Compiling : gcc -Wall -o RGBled RGBled.c -lwiringPi
 * 
 */
#include <wiringPi.h>
#include <stdio.h>
#define Rled 0
#define Gled 2
#define Bled 3
int main(void)
{
  printf( "Welcome to Smraza\n");
  printf( "Raspberry Pi RGBled test program\n" );
  printf( "Press Ctrl+C to exit\n" );
  wiringPiSetup() ;
  pinMode (Rled,OUTPUT);
  pinMode (Gled,OUTPUT);
  pinMode (Bled,OUTPUT); 
  while(1)
  {
   digitalWrite(Rled,LOW);
   digitalWrite(Gled,HIGH);
   digitalWrite(Bled,HIGH); 
   delay(1000);
   digitalWrite(Rled,HIGH);
   digitalWrite(Gled,LOW);
   digitalWrite(Bled,HIGH); 
   delay(1000);
   digitalWrite(Rled,HIGH);
   digitalWrite(Gled,HIGH);
   digitalWrite(Bled,LOW); 
   delay(1000);
   digitalWrite(Rled,LOW);
   digitalWrite(Gled,LOW);
   digitalWrite(Bled,HIGH); 
   delay(1000);
   digitalWrite(Rled,LOW);
   digitalWrite(Gled,HIGH);
   digitalWrite(Bled,LOW); 
   delay(1000);
   digitalWrite(Rled,HIGH);
   digitalWrite(Gled,LOW);
   digitalWrite(Bled,LOW); 
   delay(1000);
  }
}
