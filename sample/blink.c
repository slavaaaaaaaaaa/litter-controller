/*
 * Author: SMRAZA KEEN
 * Date:2016/9/5
 * IDE :wiringPi
 * Address:www.amazon.com/shops/smraza
 * Function: blink test
 * Compiling : gcc -Wall -o blink blink.c -lwiringPi
 */
#include <wiringPi.h>
#include <stdio.h>
int main(void)
{
  printf( "Welcome to Smraza\n");
  printf( "Raspberry Pi blink program\n" );
  printf( "Press Ctrl+C to exit\n" );
  wiringPiSetup() ;
  pinMode (0, OUTPUT) ; 
  for(;;) 
  {
    digitalWrite(0, HIGH) ; delay (500) ;
    digitalWrite(0,  LOW) ; delay (500) ;
  }
}
