#include <wiringPi.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#define BUTTON 17
#define SWITCH 20
#define VALVE_1 13
#define VALVE_2 19

int direction = 1; // -1 => left, 1 => right
int power = -1; //-1 => no power, 1 => power

void cleanUp(int signo) {
	pullUpDnControl(BUTTON, PUD_DOWN);
	pullUpDnControl(SWITCH, PUD_DOWN);
	digitalWrite(VALVE_1, LOW);
	digitalWrite(VALVE_2, LOW);
	pinMode(VALVE_1, INPUT);
	pinMode(VALVE_2, INPUT);
	exit(0);
}

unsigned short int isPressed_BUTTON() {
	static struct timespec lastCall;
	struct timespec thisCall;
	float timeDiff;
	
	clock_gettime(CLOCK_REALTIME, &thisCall);
	timeDiff = (thisCall.tv_sec + thisCall.tv_nsec/1E9 - lastCall.tv_sec - lastCall.tv_nsec/1E9)*5;
	lastCall = thisCall;
	
	return timeDiff > 1 ? 1 : 0;
	
}

unsigned short int held(unsigned short int button, unsigned short int holdTime) {
    unsigned short int sample;
    unsigned short int sampleCount = holdTime/25;
    unsigned short int delayInterval = holdTime/40;

    for(sample=0; sample<sampleCount; sample++) {
        if (!digitalRead(button)) {
            break;
        }
        delay(delayInterval);
    }

    return sample == sampleCount ? 1 : 0;
}

unsigned short int held_SWITCH(unsigned short int button, unsigned short int holdTime) {
    unsigned short int sample;
    unsigned short int sampleCount = holdTime/25;
    unsigned short int delayInterval = holdTime/40;

    for(sample=0; sample<sampleCount; sample++) {
		if (direction == 1) {
			if (!digitalRead(button)) {
				break;
			}
			delay(delayInterval);
        }
        else {
			if (digitalRead(button)) {
				break;
			}
			delay(delayInterval);
        }
    }

    return sample == sampleCount ? 1 : 0;
}

unsigned short int isPressed_SWITCH() {
	static struct timespec lastCall;
	struct timespec thisCall;
	float timeDiff;
	
	clock_gettime(CLOCK_REALTIME, &thisCall);
	timeDiff = (thisCall.tv_sec + thisCall.tv_nsec/1E9 - lastCall.tv_sec - lastCall.tv_nsec/1E9)*1;
	lastCall = thisCall;
	
	return timeDiff > 1.5 ? 1 : 0;
	
}
void goButton() {
	if (held(BUTTON, 500)) {
		power = power * -1;
		if (power == 1) {
			if (direction == -1) {
				digitalWrite(VALVE_1, HIGH);
			}
			else {
				digitalWrite(VALVE_2, HIGH);
			}
		} 
		else {
			digitalWrite(VALVE_1, LOW);
			digitalWrite(VALVE_2, LOW);
		}
	}
}

void goSwitch() {
	if (held_SWITCH(SWITCH, 1000)) {
		direction = direction * -1;
		
		digitalWrite(VALVE_1, LOW);
		digitalWrite(VALVE_2, LOW);
		
		if (power == 1) {
			if (direction == -1) {
				digitalWrite(VALVE_1, HIGH);
			}
			else {
				digitalWrite(VALVE_2, HIGH);
			}
		}
	}
}

int main(void) {
	signal(SIGINT, cleanUp);
	signal(SIGTERM, cleanUp);
	signal(SIGHUP, cleanUp);
	
	wiringPiSetupGpio();
	
	pinMode(BUTTON, INPUT);
	pinMode(SWITCH, INPUT);
	pinMode(VALVE_1, OUTPUT);
	pinMode(VALVE_2, OUTPUT);
	digitalWrite(VALVE_1, LOW);
	digitalWrite(VALVE_2, LOW);
	
	pullUpDnControl(BUTTON, PUD_UP);
	pullUpDnControl(SWITCH, PUD_UP);
	
	wiringPiISR(BUTTON, INT_EDGE_RISING, goButton);
	wiringPiISR(SWITCH, INT_EDGE_FALLING, goSwitch);
	
	pause(); 
	
	return 0;

}
