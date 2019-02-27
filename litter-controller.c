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

#define emptyingLed 27
#define waitingLed 6
#define dumpingLed 26
#define errorLed 0

#define emptyButton 23
#define dumpButton 22

#define clockwise 28
#define counterclockwise 29

#define trig 24
#define echo 25

#define emptyDistance 20
#define poopingTime 60000

static char *emptyLcdLine = "                ";
static int lcdHandle;

float sonic(void) {
    pinMode(echo, INPUT);
    pinMode(trig, OUTPUT);
    digitalWrite(trig, LOW);
    delay(30);

    digitalWrite(trig, HIGH);
    delayMicroseconds(20);
    digitalWrite(trig, LOW);

    while (digitalRead(echo) == LOW);
    long start = micros();

    while (digitalRead(echo) == HIGH);
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

void turnOnRelay(int direction, int time) {
    printf("Turning on relay %d for %dms\n", direction, time);
    pinMode(direction, OUTPUT);

    digitalWrite(direction, LOW);
    delay(time);
    digitalWrite(direction, HIGH);
}

void dumpBox(char *source) {
    time_t now;
    time(&now);
    struct tm *time = localtime(&now);
    char buffer[26];
    strftime(buffer, 26, "%a %H:%M:%S", time);
    lcdWrite(1, "Last dump:", buffer);

    digitalWrite(dumpingLed, HIGH);

    turnOnRelay(clockwise, 1000);
    turnOnRelay(counterclockwise, 1000);

    digitalWrite(dumpingLed, LOW);
}

void emptyBox(char *source) {
    time_t now;
    time(&now);
    struct tm *time = localtime(&now);
    char buffer[26];
    strftime(buffer, 26, "%a %H:%M:%S", time);
    lcdWrite(1, "Last empty:", buffer);

    digitalWrite(emptyingLed, HIGH);

    turnOnRelay(counterclockwise, 3000);
    turnOnRelay(clockwise, 4000);
    turnOnRelay(counterclockwise, 1000);

    digitalWrite(emptyingLed, LOW);
}

void waitForEvents(void) {
    pinMode(emptyButton, INPUT);
    pinMode(dumpButton, INPUT);

    time_t now;

    float previousDistance = sonic();

    while(1) {
        time(&now);

        if (digitalRead(emptyButton) == HIGH) {
            printf("Button called to EMPTY box at %s\n", ctime(&now));
            emptyBox("button");
        } else if (digitalRead(dumpButton) == HIGH) {
            printf("Button called to DUMP box at %s\n", ctime(&now));
            dumpBox("button");
        }

        float newDistance = sonic();
        printf("Current distance: %.5f\n", newDistance);

        if ((emptyDistance - newDistance) > 5) {
            printf("Distance delta caused to EMPTY box at %s\n", ctime(&now));
            printf("Previous distance:  %.5f\n", previousDistance);
            printf("New distance:       %.5f\n", newDistance);
            printf("Distance delta:     %.5f\n", (newDistance-previousDistance));

            digitalWrite(waitingLed, HIGH);
            delay(poopingTime);
            digitalWrite(waitingLed, LOW);

            emptyBox("distance");
            previousDistance = newDistance;
        }

        delay(100);
    }
}

int main(void) {
    wiringPiSetup();
    lcdHandle = lcdSetup();

    lcdWrite(1, "hello", "pizza");

    waitForEvents();

    return 0;
}
