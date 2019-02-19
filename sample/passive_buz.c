/*
 * Author: SMRAZA KEEN
 * Date:2016/6/29
 * IDE :wiringPi
 * Address:www.amazon.com/shops/smraza
 * Function: Passive buzzer test
 * Compiling : g++ -o passive_buz passive_buz.c -lwiringPi
 * 
 */
#include <wiringPi.h>
#include <stdio.h>
#define buzzer 0
int main(void)
{
  printf( "Welcome to Smraza\n");
  printf( "Raspberry Pi Passive_buzzer test program\n" );
  printf( "Press Ctrl+C to exit\n" );
  wiringPiSetup() ;
  pinMode (buzzer, OUTPUT) ; 
  while(1)
  {
    for(int i=0;i<80;i++) // output a frequency sound 
  { 
    digitalWrite(buzzer,HIGH);// sound 
    delay(1);//delay1ms 
    digitalWrite(buzzer,LOW);//not sound 
    delay(1);//ms delay
   }
    for(int j=0;j<100;j++)// output a frequency sound 
    { 
      digitalWrite(buzzer,HIGH);// sound 
      digitalWrite(buzzer,LOW);//not sound 
      delay(2);//2ms delay
    }
  }
}
