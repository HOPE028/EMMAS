#include <wiringPi.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

//GPIO PORTS

#define BUTTON_LEFT_UP 17
#define BUTTON_LEFT_DOWN 17
#define BUTTON_RIGHT_UP 17
#define BUTTON_RIGHT_DOWN 17

#define VALVE_LEFT_PDWM 5
#define VALVE_RIGHT_PDWM 5
#define VALVE_LEFT 5
#define VALVE_RIGHT 5

#define SWITCH_POWER 9
#define SWITCH_EQUAL 9
#define SWITCH_UP_OR_DOWN 9

//METHODS

int min(int a, int b);


//Global Variables: 

//-1 => false, 1 => true
int POWER = -1;
int EQUAL = -1;
int UP_OR_DOWN = -1;

int LEVEL_LEFT = 0;
int LEVEL_RIGHT = 0;
int LEVEL_MIN = 0;


//Functions

void cleanUp(int signo) {
	//GPIO clean up:

	//Buttons
	pullUpDnControl(BUTTON_LEFT_UP, PUD_DOWN);
	pullUpDnControl(BUTTON_LEFT_DOWN, PUD_DOWN);
	pullUpDnControl(BUTTON_RIGHT_UP, PUD_DOWN);
	pullUpDnControl(BUTTON_RIGHT_DOWN, PUD_DOWN);

	//Valves to low
	digitalWrite(VALVE_LEFT_PDWM, LOW);
	digitalWrite(VALVE_RIGHT_PDWM, LOW);
	digitalWrite(VALVE_LEFT, LOW);
	digitalWrite(VALVE_RIGHT, LOW);

	//Valves to input
	pinMode(VALVE_LEFT_PDWM, INPUT);
	pinMode(VALVE_RIGHT_PDWM, INPUT);
	pinMode(VALVE_LEFT, INPUT);
	pinMode(VALVE_RIGHT, INPUT);

	exit(0);
}


//Making sure click was intentional 

unsigned short int held(unsigned short int button, unsigned short int holdTime) {
    unsigned short int sample;
    unsigned short int sampleCount = holdTime/25;
    unsigned short int delayInterval = holdTime/40;

    for(sample=0; sample<sampleCount; sample++) {
        if (digitalRead(button)) {
            break;
        }
        delay(delayInterval);
    }

    return sample == sampleCount ? 1 : 0;
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

unsigned short int isPressed_SWITCH() {
	static struct timespec lastCall;
	struct timespec thisCall;
	float timeDiff;
	
	clock_gettime(CLOCK_REALTIME, &thisCall);
	timeDiff = (thisCall.tv_sec + thisCall.tv_nsec/1E9 - lastCall.tv_sec - lastCall.tv_nsec/1E9)*1;
	lastCall = thisCall;
	
	return timeDiff > 1.5 ? 1 : 0;
}

//SWITCHES react

void goSWITCH_POWER() {
	if (isPressed_SWITCH()) {
		POWER = POWER * -1;
	}
}

void goSWITCH_EQUAL() {
	if (isPressed_SWITCH()) {
		EQUAL = EQUAL * -1;

		if (EQUAL == 1) {
			LEVEL_MIN = min(LEVEL_LEFT, LEVEL_RIGHT);
			LEVEL_RIGHT = LEVEL_MIN;
			LEVEL_LEFT = LEVEL_MIN;
		}
	}
}

void goSWITCH_UP_OR_DOWN() {
	if (isPressed_SWITCH()) {
		UP_OR_DOWN = UP_OR_DOWN * -1;
	}
}


//BUTTONS react

void goBUTTON_LEFT_UP() {
	if (isPressed_BUTTON(BUTTON_LEFT_UP)) {
		if (EQUAL == 1) {
			LEVEL_LEFT++;
			LEVEL_RIGHT++;
			LEVEL_MIN++;
		} 
		else {
			LEVEL_LEFT++;
		}
	}
}

void goBUTTON_LEFT_DOWN() {
	if (isPressed_BUTTON(BUTTON_LEFT_DOWN)) {
		if (EQUAL == 1) {
			LEVEL_LEFT--;
			LEVEL_RIGHT--;
			LEVEL_MIN--;
		} 
		else {
			LEVEL_LEFT--;
		}
	}
}

void goBUTTON_RIGHT_UP() {
	if (isPressed_BUTTON(BUTTON_RIGHT_UP)) {
		if (EQUAL == 1) {
			LEVEL_LEFT++;
			LEVEL_RIGHT++;
			LEVEL_MIN++;
		} 
		else {
			LEVEL_RIGHT++;
		}
	}
}

void goBUTTON_RIGHT_DOWN() {
	if (isPressed_BUTTON(BUTTON_RIGHT_DOWN)) {
		if (EQUAL == 1) {
			LEVEL_LEFT--;
			LEVEL_RIGHT--;
			LEVEL_MIN--;
		} 
		else {
			LEVEL_RIGHT--;
		}
	}
}


int main(void) {

	//If Turned off
	signal(SIGINT, cleanUp);
	signal(SIGTERM, cleanUp);
	signal(SIGHUP, cleanUp);
	
	wiringPiSetupGpio();
	
	//Setting Up Board
	pinMode(BUTTON_LEFT_UP, INPUT);
	pinMode(BUTTON_RIGHT_UP, INPUT);
	pinMode(BUTTON_LEFT_DOWN, INPUT);
	pinMode(BUTTON_RIGHT_DOWN, INPUT);

	pinMode(VALVE_LEFT_PDWM, OUTPUT);
	pinMode(VALVE_RIGHT_PDWM, OUTPUT);
	pinMode(VALVE_LEFT, OUTPUT);
	pinMode(VALVE_RIGHT, OUTPUT);

	digitalWrite(VALVE_LEFT_PDWM, LOW);
	digitalWrite(VALVE_RIGHT_PDWM, LOW);
	digitalWrite(VALVE_LEFT, LOW);
	digitalWrite(VALVE_RIGHT, LOW);

	pullUpDnControl(BUTTON_LEFT_UP, PUD_UP);
	pullUpDnControl(BUTTON_RIGHT_UP, PUD_UP);
	pullUpDnControl(BUTTON_LEFT_DOWN, PUD_UP);
	pullUpDnControl(BUTTON_RIGHT_DOWN, PUD_UP);

	//BUTTON CLICKED ON
	wiringPiISR(BUTTON_LEFT_UP, INT_EDGE_FALLING, goBUTTON_LEFT_UP);
	wiringPiISR(BUTTON_RIGHT_UP, INT_EDGE_FALLING, goBUTTON_RIGHT_UP);
	wiringPiISR(BUTTON_LEFT_DOWN, INT_EDGE_FALLING, goBUTTON_LEFT_DOWN);
	wiringPiISR(BUTTON_RIGHT_DOWN, INT_EDGE_FALLING, goBUTTON_RIGHT_DOWN);

	//SWTICHES SWITCHED ON
	wiringPiISR(SWITCH_POWER, INT_EDGE_FALLING, goSWITCH_POWER);
	wiringPiISR(SWITCH_EQUAL, INT_EDGE_FALLING, goSWITCH_EQUAL);
	wiringPiISR(SWITCH_UP_OR_DOWN, INT_EDGE_FALLING, goSWITCH_UP_OR_DOWN);
	
	pause(); 
	
	return 0;
}


int min (int a, int b) {
	return a < b ? a : b;
}
