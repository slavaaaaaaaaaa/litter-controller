/*
 * Author: SMRAZA KEEN
 * Date:2016/9/9
 * IDE :wiringPi
 * Address:www.amazon.com/shops/smraza
 * Function: Stepper button test
 * Compiling : gcc -Wall -o button button.c -lwiringPi
 * 
 */
#include <wiringPi.h>
#include <stdio.h>
#define button 0
int val;
int main(void)
{
  printf( "Welcome to Smraza\n");
  printf( "Raspberry button_test program\n" );
  printf( "Press Ctrl+C to exit\n" );
  wiringPiSetup();
  pinMode (button, INPUT) ; 
  while(1)
  {
	val=digitalRead(button);
	if(val==HIGH)
	{  
            delay(100);   
            if(val==HIGH)
		printf( "button on\n");
	}
	else 
	{
		printf( "button off\n");
	}
	delay(200);
  }
}
