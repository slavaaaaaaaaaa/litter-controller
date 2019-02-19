/*
 * Author: SMRAZA KEEN
 * Date:2016/9/8
 * IDE :wiringPi
 * Address:www.amazon.com/shops/smraza
 * Function: Temperature and humidity monitoring experiment
 * Compiling :gcc -Wall -o LCD1602_DHT11 LCD1602_DHT11.c -lwiringPi -lwiringPiDev
 * Connection:
 * DHT11
 * '+'		5V0
 * out		GPIO7
 * '-'		GND
 * LCD1602 with IIC:
 * GND		GND
 * VCC		5V0
 * SDA		SDA1
 * SCL		SCL1
 */
#include <wiringPi.h>
#include <pcf8574.h>
#include <lcd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MAXTIMINGS  85
#define DHTPIN      7  

//PCF8574 Start I/O address
// PCF8754 64+8
#define AF_BASE 64
#define AF_RS (AF_BASE + 0)
#define AF_RW (AF_BASE + 1)
#define AF_E (AF_BASE + 2)
#define AF_LED (AF_BASE + 3)
#define AF_DB4 (AF_BASE + 4)
#define AF_DB5 (AF_BASE + 5)
#define AF_DB6 (AF_BASE + 6)
#define AF_DB7 (AF_BASE + 7)
// Global lcd handle:
static int lcdHandle;

int dht11_dat[5] = { 0, 0, 0, 0, 0 };

void read_dht11_dat();
int main(void)
{
int i;
wiringPiSetup();        //Initialise WiringPi
printf( "Welcome to Smraza\n");
printf( "Temperature and humidity monitoring experiment\n" );
printf( "Press Ctrl+C to exit\n" );
pcf8574Setup(AF_BASE,0x3F);
lcdHandle = lcdInit (2, 16, 4, AF_RS, AF_E, AF_DB4,AF_DB5,AF_DB6,AF_DB7, 0,0,0,0) ;
if (lcdHandle < 0)
{
fprintf (stderr, "lcdInit failed\n") ;
exit (EXIT_FAILURE) ;
}
for(i=0;i<8;i++)
pinMode(AF_BASE+i,OUTPUT); 	//Will expand the IO port as the output mode
digitalWrite(AF_LED,1); 	//Open back light
digitalWrite(AF_RW,0); 		//Set the R/Wall to a low level, LCD for the write state
lcdClear(lcdHandle); 		//Clear display
while ( 1 )
    {
        read_dht11_dat();
		delay(1000);
    }
}

void read_dht11_dat()
{
    uint8_t laststate   = HIGH;
    uint8_t counter     = 0;
    uint8_t j       = 0, i;

    dht11_dat[0] = dht11_dat[1] = dht11_dat[2] = dht11_dat[3] = dht11_dat[4] = 0;

    /* pull pin down for 18 milliseconds */
    pinMode( DHTPIN, OUTPUT );
    digitalWrite( DHTPIN, LOW );
    delay( 18 );
    /* then pull it up for 40 microseconds */
    digitalWrite( DHTPIN, HIGH );
    delayMicroseconds( 40 );
    /* prepare to read the pin */
    pinMode( DHTPIN, INPUT );

    /* detect change and read data */
    for ( i = 0; i < MAXTIMINGS; i++ )
    {
        counter = 0;
        while ( digitalRead( DHTPIN ) == laststate )
        {
            counter++;
            delayMicroseconds( 1 );
            if ( counter == 255 )
            {
                break;
            }
        }
        laststate = digitalRead( DHTPIN );

        if ( counter == 255 )
            break;

        /* ignore first 3 transitions */
        if ( (i >= 4) && (i % 2 == 0) )
        {
            /* shove each bit into the storage bytes */
            dht11_dat[j / 8] <<= 1;
            if ( counter > 16 )
                dht11_dat[j / 8] |= 1;
            j++;
        }
    }
    /*
     * check we read 40 bits (8bit x 5 ) + verify checksum in the last byte
     * print it out if data is good
     */
    if ( (j >= 40) &&(dht11_dat[4] == ( (dht11_dat[0] + dht11_dat[1] + dht11_dat[2] + dht11_dat[3]) & 0xFF) ) )
    {
        lcdPosition (lcdHandle, 0, 0);
		lcdPrintf(lcdHandle,"Hum=%d.%d%%",dht11_dat[0], dht11_dat[1]);
		lcdPosition (lcdHandle, 0, 1);
		lcdPrintf(lcdHandle,"Tem=%d.%d%%",dht11_dat[2], dht11_dat[3]);
		lcdPosition (lcdHandle, 10, 1);
		lcdPuts(lcdHandle, "Smraza");
    }
}
