/*
 * Author: SMRAZA KEEN
 * Date:2016/6/29
 * IDE :wiringPi
 * Address:www.amazon.com/shops/smraza
 * Function: relay_test
 * Compiling : gcc -Wall -o relay relay.c -lwiringPi
 * 
 */
#include <wiringPi.h>
#include <stdio.h>
#define relay 0
char key;
int main(void)
{
  printf( "Welcome to Smraza\n");
  printf( "Raspberry relay_test program\n" );
  printf( "Press Ctrl+C to exit\n" );
  wiringPiSetup();
  pinMode (relay, OUTPUT) ; 
  while(1)
  {
	printf( "Input 0 1 \n");
	key=getchar();
	if(key=='1')
	{
		digitalWrite(relay,HIGH);
	}
	else if(key=='0')
	{
		digitalWrite(relay,LOW);
	}
  }
}
