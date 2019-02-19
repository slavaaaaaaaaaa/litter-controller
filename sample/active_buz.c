/*
 * Author: SMRAZA KEEN
 * Date:2016/9/9
 * IDE :wiringPi
 * Address:www.amazon.com/shops/smraza
 * Function: Active test
 * Compiling : gcc -Wall -o active_buz active_buz.c -lwiringPi
 * 
 */
#include <wiringPi.h>
#include <stdio.h>
#define buzzer 0
int main(void)
{
  printf( "Welcome to Smraza\n");
  printf( "Raspberry Pi Active buzzer test program\n" );
  printf( "Press Ctrl+C to exit\n" );
  wiringPiSetup() ;
  pinMode (buzzer, OUTPUT) ; 
  while(1)
  {
   digitalWrite(buzzer,HIGH);
  }
}

