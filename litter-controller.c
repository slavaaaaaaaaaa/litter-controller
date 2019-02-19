#include <wiringPi.h>
#include <pcf8574.h>
#include <lcd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <pthread.h>
#include <signal.h>

#define AF_BASE 64
#define AF_RS (AF_BASE + 0)
#define AF_RW (AF_BASE + 1)
#define AF_E (AF_BASE + 2)
#define AF_LED (AF_BASE + 3)
#define AF_DB4 (AF_BASE + 4)
#define AF_DB5 (AF_BASE + 5)
#define AF_DB6 (AF_BASE + 6)
#define AF_DB7 (AF_BASE + 7)

#define led 0
#define button 1

void *blinkLedForever(void *args) {
    int time = (int) args;

    while(1) {
        digitalWrite(led, HIGH);
        delay(time);
        digitalWrite(led, LOW);
        delay(time);
    }
}

int blinkLed(int time) {
    pthread_t tid;
    pthread_create(&tid, NULL, blinkLedForever, (void *) time);

    return tid;
}

int main(void) {
    wiringPiSetup();
    pcf8574Setup(AF_BASE, 0x27);

    int lcdHandle = lcdInit (2, 16, 4, AF_RS, AF_E, AF_DB4,AF_DB5,AF_DB6,AF_DB7, 0,0,0,0);

    if (lcdHandle < 0) {
        fprintf (stderr, "lcdInit failed\n");
        exit (EXIT_FAILURE);
    }

    for(int i=0;i<8;i++)
        pinMode(AF_BASE+i, OUTPUT); // Expand the IO port as the output mode

    digitalWrite(AF_LED, 1); // Open back light
    digitalWrite(AF_RW, 0); // Set the R/Wall to a low level, LCD for the write state
    lcdClear(lcdHandle);

    lcdPosition(lcdHandle, 0, 0);
    lcdPuts(lcdHandle, "hi anna");
    lcdPosition(lcdHandle, 0, 1);
    lcdPuts(lcdHandle, "cat is on oven!");

    pinMode(button, INPUT);
    pinMode(led, OUTPUT);

    int blinkingLed = blinkLed(300);

    while(1) {
        if (digitalRead(button)==HIGH) {
            digitalWrite(led, HIGH);
            pthread_kill(blinkingLed, 15);
        }

        delay(100);
    }

    return 0;
}
