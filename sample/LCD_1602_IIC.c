/*
 * Author: SMRAZA KEEN
 * Date:2016/9/8
 * IDE :wiringPi
 * Address:www.amazon.com/shops/smraza
 * Function: LCD1602 with IIC test
 * Compiling :gcc -Wall -o LCD_1602_IIC LCD_1602_IIC.c -lwiringPi -lwiringPiDev
 */
#include <wiringPi.h>
#include <pcf8574.h>
#include <lcd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

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

static int lcdHandle;

int main(void) {
	wiringPiSetup();
	pcf8574Setup(AF_BASE, 0x27);
	lcdHandle = lcdInit (2, 16, 4, AF_RS, AF_E, AF_DB4,AF_DB5,AF_DB6,AF_DB7, 0,0,0,0);

	if (lcdHandle < 0) {
	        fprintf (stderr, "lcdInit failed\n");
	        exit (EXIT_FAILURE);
	}

	for(int i=0;i<8;i++)
                pinMode(AF_BASE+i, OUTPUT); // Expand the IO port as the output mode

	digitalWrite(AF_LED, 1); // Open back light
	digitalWrite(AF_RW, 0); // Set the R/Wall to a low level, LCD for the write state
	lcdClear(lcdHandle); // Clear display
	lcdPosition(lcdHandle, 0, 0); // Position cursor on the first line in the first column
	lcdPuts(lcdHandle, "hi hello"); // Print the text on the LCD at the current cursor postion
	lcdPosition(lcdHandle, 6, 1);
	lcdPuts(lcdHandle, "pizza");

	return 0;
}
