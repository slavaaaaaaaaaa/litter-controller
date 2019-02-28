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

#include <argparse.h>

#define AF_BASE 64
#define AF_RS (AF_BASE + 0)
#define AF_RW (AF_BASE + 1)
#define AF_E (AF_BASE + 2)
#define AF_LED (AF_BASE + 3)
#define AF_DB4 (AF_BASE + 4)
#define AF_DB5 (AF_BASE + 5)
#define AF_DB6 (AF_BASE + 6)
#define AF_DB7 (AF_BASE + 7)

static int emptyingLed =        27;
static int waitingLed =         6;
static int dumpingLed =         26;
static int errorLed =           0;
static int emptyButton =        23;
static int dumpButton =         22;
static int clockwise =          28;
static int counterclockwise =   29;
static int trig =               24;
static int echo =               25;
static int lcdWidth =           16;
static int emptyDistance =      35;
static int deltaDistance =      15;
static int poopingTime =        180;
static int DEBUG =              1;

static char *emptyLcdLine = "                ";
static int lcdHandle;


static const char *const usage[] = {
    "litter-controller [options] [[--] args]",
    "litter-controller [options]",
    NULL,
};

void debug(char *args) {
    if (DEBUG)
        printf("DEBUG: %s\n", args);
}

float sonic(void) {
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
        if (strlen(messageTop) > lcdWidth) {
            printf("%s is too long; TODO scrolling", messageTop);
        } else {
            lcdPosition(lcdHandle, 0, 0);
            lcdPuts(lcdHandle, emptyLcdLine);
            lcdPosition(lcdHandle, 0, 0);
            lcdPuts(lcdHandle, messageTop);
        }
    }

    if (strcmp(messageBottom, "")) {
        if (strlen(messageTop) > lcdWidth) {
            printf("%s is too long; TODO scrolling", messageBottom);
        } else {
            lcdPosition(lcdHandle, 0, 1);
            lcdPuts(lcdHandle, emptyLcdLine);
            lcdPosition(lcdHandle, 0, 1);
            lcdPuts(lcdHandle, messageBottom);
        }
    }

    return 0;
}

void turnOnRelay(int direction, int seconds) {
    digitalWrite(direction, LOW);
    delay((seconds * 1000));
    digitalWrite(direction, HIGH);
}

void dumpBox(char *source) {
    time_t now;
    time(&now);
    struct tm *time = localtime(&now);
    char buffer[26];
    strftime(buffer, 26, "%a %H:%M:%S", time);
    lcdWrite(1, "Last dumped:", buffer);

    digitalWrite(dumpingLed, HIGH);

    turnOnRelay(clockwise, 10);
    turnOnRelay(counterclockwise, 10);

    digitalWrite(dumpingLed, LOW);
}

void emptyBox(char *source) {
    time_t now;
    time(&now);
    struct tm *time = localtime(&now);
    char buffer[26];
    strftime(buffer, 26, "%a %H:%M:%S", time);
    lcdWrite(1, "Last empty:", buffer);

    if (digitalRead(emptyButton) == HIGH) {
        printf("Button called to EMPTY box at %s", ctime(&now));
        emptyBox("button");
    } else if (digitalRead(dumpButton) == HIGH) {
        printf("Button called to DUMP box at %s", ctime(&now));
        dumpBox("button");
    }
}

    turnOnRelay(counterclockwise, 3000);
    turnOnRelay(clockwise, 4000);
    turnOnRelay(counterclockwise, 1000);

    digitalWrite(emptyingLed, LOW);
}

void waitForEvents(void) {
    time_t now;

    float previousDistance = sonic();

    while(1) {
        time(&now);

        if (digitalRead(emptyButton) == HIGH) {
            printf("Button called to EMPTY box at %s", ctime(&now));
            emptyBox("button");
        } else if (digitalRead(dumpButton) == HIGH) {
            printf("Button called to DUMP box at %s", ctime(&now));
            dumpBox("button");
        }

        float newDistance = sonic();
        printf("Current distance: %.5f\n", newDistance);

        if ((emptyDistance - newDistance) > 5) {
            printf("Distance delta caused to EMPTY box at %s", ctime(&now));
            printf("\tPrevious distance:  %.5f\n", previousDistance);
            printf("\tNew distance:       %.5f\n", newDistance);
            printf("\tDistance delta:     %.5f\n", (newDistance - previousDistance));

            digitalWrite(waitingLed, HIGH);
            delay(poopingTime);
            digitalWrite(waitingLed, LOW);

            emptyBox("distance");
            previousDistance = newDistance;
        }

        delay(100);
    }
}

void setPins() {
    pinMode(emptyingLed,        OUTPUT);
    pinMode(waitingLed,         OUTPUT);
    pinMode(dumpingLed,         OUTPUT);
    pinMode(errorLed,           OUTPUT);

    pinMode(emptyButton,        INPUT);
    pinMode(dumpButton,         INPUT);

    pinMode(clockwise,          OUTPUT);
    pinMode(counterclockwise,   OUTPUT);

    pinMode(echo,               INPUT);
    pinMode(trig,               OUTPUT);

    digitalWrite(emptyingLed,       LOW);
    digitalWrite(waitingLed,        LOW);
    digitalWrite(dumpingLed,        LOW);
    digitalWrite(errorLed,          LOW);

    digitalWrite(clockwise,         HIGH);
    digitalWrite(counterclockwise,  HIGH);
}

int main(int argc, const char **argv) {
    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_GROUP("GPIO pins"),
        OPT_INTEGER('e', "emptyingLed",     &emptyingLed,       "LED that glows when emptying"),
        OPT_INTEGER('w', "waitingLed",      &waitingLed,        "LED that glows when waiting for exit"),
        OPT_INTEGER('d', "dumpingLed",      &dumpingLed,        "LED that glows when dumping litter"),
        OPT_INTEGER('r', "errorLed",        &errorLed,          "LED that glows on error"),
        OPT_INTEGER('E', "emptyButton",     &emptyButton,       "button to initiate emptying"),
        OPT_INTEGER('D', "dumpButton",      &dumpButton,        "button to initiate litter dumping"),
        OPT_INTEGER('c', "clockwiseRelay",  &clockwise,         "clockwise relay pin"),
        OPT_INTEGER('C', "cclockwiseRelay", &counterclockwise,  "counterclockwise relay pin"),
        OPT_INTEGER('t', "trig",            &trig,              "ultrasonic sensor's trig pin"),
        OPT_INTEGER('h', "echo",            &echo,              "ultrasonic sensor's echo pin"),
        OPT_GROUP("Other options"),
        OPT_INTEGER('l', "lcdWidth",        &lcdWidth,          "character width of the LCD screen"),
        OPT_INTEGER('i', "emptyDistance",   &emptyDistance,     "distance (in cm) expected when empty"),
        OPT_INTEGER('I', "deltaDistance",   &deltaDistance,     "distance (in cm) delta to trigger emptying"),
        OPT_INTEGER('p', "poopingTime",     &poopingTime,       "time (in seconds) to wait before emptying"),
        OPT_INTEGER('g', "debug",           &DEBUG,             "whether to show debugging output"),
        OPT_END(),
    };

    struct argparse argparse;
    argparse_init(&argparse, options, usage, 0);
    argparse_describe(&argparse, "\nReplacement litter box controller.", "\nReplacement litter box controller expecting an RPI 3B+.");
    argc = argparse_parse(&argparse, argc, argv);

    wiringPiSetup();
    lcdHandle = lcdSetup();
    setPins();

    lcdWrite(1, "henlo", "");

    waitForEvents();

    return 0;
}
