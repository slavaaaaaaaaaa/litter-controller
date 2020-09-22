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

static int emptyingLed =            26;
static int waitingLed =             3;
static int dumpingLed =             0;
static int errorLed =               0;
static int emptyButton =            22;
static int dumpButton =             7;
static int clockwise =              21;
static int counterclockwise =       6;
static int trig =                   2;
static int echo =                   1;
static int lcdWidth =               16;
static int emptyDistance =          45;
static int kittyInsideDistance =    35;
static int falseDistanceThreshold = 400; // if too close to the sensor, it reports ~2300. This is a safe number I guess
static int poopingTime =            120;
static int ccwTurnTime =            55;
static int cwTurnTime =             65;
static int dumpTime =               8; // make this just too short to check sonic sensor clearance later
static int DEBUG =                  0;
static char *VERSION =              "0.1.0";

static char *emptyLcdLine = "                ";
static int lcdHandle;

static bool kittyInside = FALSE;

pthread_mutex_t motorLock = PTHREAD_MUTEX_INITIALIZER;

static const char *const usage[] = {
    "litter-controller [options] [[--] args]",
    "litter-controller [options]",
    NULL,
};

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

// check that we've spun enough to clear the sonic sensor
void alignBox() {
    while(1) {
        float distance = sonic();
        if ((distance > falseDistanceThreshold) || // if the distance is abnormally high, likely due to kitty sniffing the ultrasonic sensor...
            (distance < kittyInsideDistance)) { // or if it's actually too low
            digitalWrite(counterclockwise, LOW);
            delay(1000);
        } else {
            digitalWrite(counterclockwise, HIGH);
            break;
        }
    }
}

void dumpBox(char *source) {
    time_t now;
    time(&now);
    printf("'%s' called to dump box at %s", source, ctime(&now));

    int err = pthread_mutex_trylock(&motorLock);
    if (err == 0) {
        struct tm *time = localtime(&now);
        char buffer[26];
        strftime(buffer, 26, "%a %H:%M", time);
        lcdWrite(1, "Litter dumped", buffer);

        digitalWrite(dumpingLed, HIGH);
        digitalWrite(waitingLed, LOW);

        turnOnRelay(clockwise, dumpTime);
        delay(2000);
        turnOnRelay(counterclockwise, dumpTime);

        alignBox();

        digitalWrite(dumpingLed, LOW);
        pthread_mutex_unlock(&motorLock);
    } else {
        printf("Motor already in use, '%s' trigger to dump ignored due to %d!\n", source, err);
    }
}

void emptyBox(char *source) {
    time_t now;
    time(&now);
    printf("'%s' called to empty box at %s", source, ctime(&now));

    int err = pthread_mutex_trylock(&motorLock);
    if (err == 0) {
        struct tm *time = localtime(&now);
        char buffer[26];
        strftime(buffer, 26, "%a %H:%M", time);

        lcdWrite(1, source, buffer);

        digitalWrite(emptyingLed, HIGH);
        digitalWrite(waitingLed, LOW);

        turnOnRelay(counterclockwise, ccwTurnTime);
        delay(1000);
        turnOnRelay(clockwise, cwTurnTime);
        delay(1000);
        turnOnRelay(counterclockwise, dumpTime);

        alignBox();

        digitalWrite(emptyingLed, LOW);
        pthread_mutex_unlock(&motorLock);
    } else {
        printf("Motor already in use, '%s' trigger to empty ignored due to %d!\n", source, err);
    }
}

void checkButtonState() {
    if (digitalRead(emptyButton) == HIGH)
        emptyBox("Human pushed me");
    else if (digitalRead(dumpButton) == HIGH)
        dumpBox("Human pushed me");
}

void *waitForKitty(void *args) {
    digitalWrite(waitingLed, HIGH);
    kittyInside = TRUE;

    delay(poopingTime * 1000);

    emptyBox("Kitty was here");

    kittyInside = FALSE;
    pthread_exit(NULL);
}

void checkSonicState() {
    time_t now;
    time(&now);
    float distance = sonic();

    if (DEBUG > 0)
        printf("Current distance: %.5f\n", distance);

    if (((distance > falseDistanceThreshold) || // if the distance is abnormally high, likely due to kitty sniffing the ultrasonic sensor...
        (distance < kittyInsideDistance)) && // or if the distance is within limits, ...
            ! kittyInside) { // while there's no known kitty inside
        printf("Distance delta caused to empty box at %s", ctime(&now));

        pthread_t tid;
        pthread_create(&tid, NULL, waitForKitty, NULL);
    }
}

void waitForEvents(void) {
    while(1) {
        checkButtonState();
        checkSonicState();

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
        OPT_INTEGER('e', "emptyingLed",             &emptyingLed,       "LED that glows when emptying"),
        OPT_INTEGER('w', "waitingLed",              &waitingLed,        "LED that glows when waiting for exit"),
        OPT_INTEGER('u', "dumpingLed",              &dumpingLed,        "LED that glows when dumping litter"),
        OPT_INTEGER('r', "errorLed",                &errorLed,          "LED that glows on error"),
        OPT_INTEGER('E', "emptyButton",             &emptyButton,       "button to initiate emptying"),
        OPT_INTEGER('D', "dumpButton",              &dumpButton,        "button to initiate litter dumping"),
        OPT_INTEGER('c', "clockwiseRelay",          &clockwise,         "clockwise relay pin"),
        OPT_INTEGER('C', "cclockwiseRelay",         &counterclockwise,  "counterclockwise relay pin"),
        OPT_INTEGER('t', "trig",                    &trig,              "ultrasonic sensor's trig pin"),
        OPT_INTEGER('h', "echo",                    &echo,              "ultrasonic sensor's echo pin"),
        OPT_GROUP("Other options"),
        OPT_INTEGER('l', "lcdWidth",                &lcdWidth,          "character width of the LCD screen"),
        OPT_INTEGER('i', "emptyDistance",           &emptyDistance,     "distance (in cm) expected when empty"),
        OPT_INTEGER('I', "kittyInsideDistance",     &kittyInsideDistance, "distance (in cm) to trigger emptying"),
        OPT_INTEGER('f', "falseDistanceThreshold",  &falseDistanceThreshold, "distance (in cm) that is too high to be true"),
        OPT_INTEGER('p', "poopingTime",             &poopingTime,       "time (in seconds) to wait before emptying"),
        OPT_INTEGER('d', "debug",                   &DEBUG,             "whether to use debugging timing values"),
        OPT_END(),
    };

    struct argparse argparse;
    argparse_init(&argparse, options, usage, 0);
    argparse_describe(&argparse, "\nReplacement litter box controller.", "\nReplacement litter box controller expecting an RPI 3B+.");
    argc = argparse_parse(&argparse, argc, argv);

    if (DEBUG > 1) {
        printf("Running in debug mode with lowered times!\n");
        poopingTime = 5;
        ccwTurnTime = 5;
        cwTurnTime =  6;
        dumpTime =    1;
    }

    wiringPiSetup();
    lcdHandle = lcdSetup();
    setPins();

    lcdWrite(1, "litter control", VERSION);

    alignBox();

    waitForEvents();

    return 1;
}
