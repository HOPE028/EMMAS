#include <wiringPi.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#define BUTTON 5

int special = 0;

unsigned short int held_BUTTON(unsigned short int button, unsigned short int holdTime) {
    unsigned short int sample;
    unsigned short int sampleCount = holdTime/25;
    unsigned short int delayInterval = holdTime/40;

    for(sample = 0; sample<sampleCount; sample++) {
        if (!digitalRead(button)) {
            break;
        }
        delay(delayInterval);
    }

    return sample == sampleCount ? 1 : 0;
}

void goBUTTON() {
 if (held_BUTTON(BUTTON, 500)) {
  special++;
 }
}

void print() {
 for (;;) {
  printf("%d\n", special);
  delay(200);
 }
}

void *callback(void *args) {
    print();
    return 0;
}

void runThread() {
 static pthread_t threadID;

 pthread_create(&threadID, NULL, callback, NULL);
}


int main() {
 signal(SIGINT, cleanUp);
	signal(SIGTERM, cleanUp);
	signal(SIGHUP, cleanUp);

 wiringPiSetupGpio(); 

 pinMode(BUTTON, INPUT);

 pullUpDnControl(BUTTON, PUD_UP);

 wiringPiISR(BUTTON, INT_EDGE_FALLING, goBUTTON);

 runThread();

 pause(); 


 return 0;
}