/*
 * Author: SMRAZA KEEN
 * Date:2016/6/29
 * IDE :wiringPi
 * Address:www.amazon.com/shops/smraza
 * Function: HC sr04 test
 * Compiling : gcc -Wall -o HCsr04 HCsr04.c -lwiringPi
 * 
 */
#include <wiringPi.h>
#include <stdio.h>
#include <sys/time.h>

#define Trig    24
#define Echo    25

void ultraInit(void)
{
	pinMode(Echo, INPUT);
	pinMode(Trig, OUTPUT);
}

float disMeasure(void)
{
	struct timeval tv1;
	struct timeval tv2;
	long start, stop;
    	float dis;

	digitalWrite(Trig, LOW);
	delayMicroseconds(2);

	digitalWrite(Trig, HIGH);
	delayMicroseconds(10);  
	digitalWrite(Trig, LOW);
	while(!(digitalRead(Echo) == 1));
	gettimeofday(&tv1, NULL);  

	while(!(digitalRead(Echo) == 0));
	gettimeofday(&tv2, NULL);           

	start = tv1.tv_sec * 1000000 + tv1.tv_usec;
	stop  = tv2.tv_sec * 1000000 + tv2.tv_usec;

	dis = (float)(stop - start) / 1000000 * 34000 / 2;

	return dis;
}

int main(void)
{
	float dis;

	if(wiringPiSetup() == -1){ //when initialize wiring failed,print messageto screen
		printf("setup wiringPi failed !");
		return 1; 
	}

	ultraInit();
	 printf( "Welcome to Smraza\n");
     printf( "Raspberry HC sr04 test program\n" );
	 printf( "Press Ctrl+C to exit\n" );
	while(1){
		dis = disMeasure();
		printf("distance = %0.2f cm\n",dis);
		delay(1000);
	}

	return 0;
}

