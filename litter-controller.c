#include <wiringPi.h>
#include <pcf8574.h>
#include <lcd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>

#include <pthread.h>
#include <signal.h>
#include <string.h>
#include <stdbool.h>

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
#define trig 24
#define echo 25

static char *emptyLcdLine = "                ";
static int lcdHandle;

void *blinkLedForever(void *args) {
    int time = (int) args;
    printf("Blinking at interval %d\n", time);

    while(1) {
        digitalWrite(led, HIGH);
        delay(time);
        digitalWrite(led, LOW);
        delay(time);
    }
}

pthread_t blinkLed(int time) {
    pinMode(led, OUTPUT);

    pthread_t tid;
    pthread_create(&tid, NULL, blinkLedForever, time);

    return tid;
}

float sonic(void) {
    pinMode(echo, INPUT);
    pinMode(trig, OUTPUT);
    digitalWrite(trig, LOW);
    delay(30);

    digitalWrite(trig, HIGH);
    delayMicroseconds(20);
    digitalWrite(trig, LOW);

    while(digitalRead(echo) == LOW);
    long start = micros();

    while(digitalRead(echo) == HIGH);
    long travel = micros() - start;

    return (float) travel / 58;
}

int lcdSetup(void) {
    pcf8574Setup(AF_BASE, 0x27);

    lcdHandle = lcdInit (2, 16, 4, AF_RS, AF_E, AF_DB4,AF_DB5,AF_DB6,AF_DB7, 0,0,0,0);

    if (lcdHandle < 0) {
        fprintf (stderr, "lcdInit failed\n");
        exit (EXIT_FAILURE);
    }

    for (int i=0; i<8; i++)
        pinMode(AF_BASE+i, OUTPUT);

    digitalWrite(AF_LED, HIGH); // backlight
    digitalWrite(AF_RW, LOW); // Set the R/Wall to a low level, LCD for the write state

    return lcdHandle;
}

int lcdWrite(bool clear, char *messageTop, char *messageBottom) {
    if (clear)
        lcdClear(lcdHandle);

    if (strcmp(messageTop, "")) {
        if (strlen(messageTop) > 16) {
            printf("%s is too long; TODO scrolling", messageTop);
        } else {
            lcdPosition(lcdHandle, 0, 0);
            lcdPuts(lcdHandle, emptyLcdLine);
            lcdPosition(lcdHandle, 0, 0);
            lcdPuts(lcdHandle, messageTop);
        }
    }

    if (strcmp(messageBottom, "")) {
        lcdPosition(lcdHandle, 0, 1);
        lcdPuts(lcdHandle, emptyLcdLine);
        lcdPosition(lcdHandle, 0, 1);
        lcdPuts(lcdHandle, messageBottom);
    }

    return 0;
}

int waitForButton(void) {
    pinMode(button, INPUT);

    while(1) {
        if (digitalRead(button)==HIGH) {
            digitalWrite(led, HIGH);
            return 0;
        }

        printf("Distance: %.6f\n", sonic());

        delay(100);
    }
}

int main(void) {
    wiringPiSetup();
    lcdHandle = lcdSetup();
    pthread_t blinkingLedTid = blinkLed(300);

    lcdWrite(1, "hello", "pizza");
    delay(1000);

    waitForButton();

    pthread_cancel(blinkingLedTid);

    return 0;
}
