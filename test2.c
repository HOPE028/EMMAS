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

#define VALVE_LEFT_PWM 23
#define VALVE_RIGHT_PWM 19
#define VALVE_LEFT 5
#define VALVE_RIGHT 21

#define SWITCH_POWER 9
#define SWITCH_EQUAL 9

//METHODS

int min(int a, int b);


//Global Variables: 

//-1 => false, 1 => true
int POWER = -1;
int EQUAL = -1;

int LEVEL_LEFT = 0;
int LEVEL_RIGHT = 0;
int LEVEL_MIN = 0;

int delayLeft[2][2] = { 
 {0, 100}, //PWM => delay on, delay off
 {0, 100}  //Exhaust valves => delay on, delay off
};

int delayRight[2][2] = { 
 {0, 100}, //PWM => delay on, delay off
 {0, 100}  //Exhaust valves => delay on, delay off
};

static pthread_t thread_VALVE_RIGHT_PWM;
static pthread_t thread_VALVE_LEFT_PWM;
static pthread_t thread_VALVE_RIGHT;
static pthread_t thread_VALVE_LEFT;
static pthread_t thread_logToConsole;


//Functions

void cleanUp(int signo) {
	//GPIO clean up:

	//Buttons
	pullUpDnControl(BUTTON_LEFT_UP, PUD_DOWN);
	pullUpDnControl(BUTTON_LEFT_DOWN, PUD_DOWN);
	pullUpDnControl(BUTTON_RIGHT_UP, PUD_DOWN);
	pullUpDnControl(BUTTON_RIGHT_DOWN, PUD_DOWN);

	//Valves to low
	digitalWrite(VALVE_LEFT_PWM, LOW);
	digitalWrite(VALVE_RIGHT_PWM, LOW);
	digitalWrite(VALVE_LEFT, LOW);
	digitalWrite(VALVE_RIGHT, LOW);

	//Valves to input
	pinMode(VALVE_LEFT_PWM, INPUT);
	pinMode(VALVE_RIGHT_PWM, INPUT);
	pinMode(VALVE_LEFT, INPUT);
	pinMode(VALVE_RIGHT, INPUT);

	exit(0);
}


//Making sure click was intentional 

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

unsigned short int held_SWITCH(unsigned short int current_switch, unsigned short int holdTime, unsigned short int currentState) {
 unsigned short int sample;
 unsigned short int sampleCount = holdTime/25;
 unsigned short int delayInterval = holdTime/40;

 for(sample=0; sample<sampleCount; sample++) {
		if (currentState == 1) {
			if (!digitalRead(current_switch)) {
					break;
			}
		}
  else {
				if (digitalRead(current_switch)) {
					break;
				}
 	 }
			delay(delayInterval);
 	}
 	return sample == sampleCount ? 1 : 0;
}

//SWITCHES react

void goSWITCH_POWER() {
	if (held_SWITCH(SWITCH_POWER, 1000, POWER)) {
		POWER = POWER * -1;
	}
}

void goSWITCH_EQUAL() {
	if (held_SWITCH(SWITCH_EQUAL, 1000, EQUAL)) {
		EQUAL = EQUAL * -1;

		if (EQUAL == 1) {
			LEVEL_MIN = min(LEVEL_LEFT, LEVEL_RIGHT);
			LEVEL_RIGHT = LEVEL_MIN;
			LEVEL_LEFT = LEVEL_MIN;
		}
	}
}

//BUTTONS react

void goBUTTON_LEFT_UP() {
	if (held_BUTTON(BUTTON_LEFT_UP, 500)) {
		if (LEVEL_LEFT < 8) {
			if (EQUAL == 1) {
			LEVEL_LEFT++;
			LEVEL_RIGHT++;
			LEVEL_MIN++;
			} 
			else {
				LEVEL_LEFT++;
			}
			changeDelay();
		}
	}
}

void goBUTTON_LEFT_DOWN() {
	if (held_BUTTON(BUTTON_LEFT_DOWN, 500)) {
		if (LEVEL_LEFT > 0) {
			if (EQUAL == 1) {
				LEVEL_LEFT--;
				LEVEL_RIGHT--;
				LEVEL_MIN--;
			} 
			else {
				LEVEL_LEFT--;
			}
			changeDelay();
		}
	}
}

void goBUTTON_RIGHT_UP() {
	if (held_BUTTON(BUTTON_RIGHT_UP, 500)) {
		if (LEVEL_RIGHT < 8) {
			if (EQUAL == 1) {
				LEVEL_LEFT++;
				LEVEL_RIGHT++;
				LEVEL_MIN++;
			} 
			else {
				LEVEL_RIGHT++;
			}
			changeDelay();
		}
	}
}

void goBUTTON_RIGHT_DOWN() {
	if (held_BUTTON(BUTTON_RIGHT_DOWN, 500)) {
		if (LEVEL_RIGHT > 0) {
			if (EQUAL == 1) {
				LEVEL_LEFT--;
				LEVEL_RIGHT--;
				LEVEL_MIN--;
			} 
			else {
				LEVEL_RIGHT--;
			}
			changeDelay();
		}
	}
}


//Control over the valves

void changeDelay() {
	if (EQUAL == 1) {
		//update delay for left and right using one value
		if (LEVEL_MIN == 0) {
			//LEFT
			delayLeft[0][0] = 0;
			delayLeft[0][1] = 100;
			delayLeft[1][0] = 0;
			delayLeft[1][1] = 100;

			//RIGHT
			delayRight[0][0] = 0;
			delayRight[0][1] = 100;
			delayRight[1][0] = 0;
			delayRight[1][1] = 100;
		}
		else if (LEVEL_MIN == 1) {
			//LEFT
			delayLeft[0][0] = 0;
			delayLeft[0][1] = 100;
			delayLeft[1][0] = 25;
			delayLeft[1][1] = 75;

			//RIGHT
			delayRight[0][0] = 0;
			delayRight[0][1] = 100;
			delayRight[1][0] = 25;
			delayRight[1][1] = 75;
		}
		else if (LEVEL_MIN == 2) {
			//LEFT
			delayLeft[0][0] = 0;
			delayLeft[0][1] = 100;
			delayLeft[1][0] = 50;
			delayLeft[1][1] = 50;

			//RIGHT
			delayRight[0][0] = 0;
			delayRight[0][1] = 100;
			delayRight[1][0] = 50;
			delayRight[1][1] = 50;
		}
		else if (LEVEL_MIN == 3) {
			//LEFT
			delayLeft[0][0] = 0;
			delayLeft[0][1] = 100;
			delayLeft[1][0] = 75;
			delayLeft[1][1] = 25;

			//RIGHT
			delayRight[0][0] = 0;
			delayRight[0][1] = 100;
			delayRight[1][0] = 75;
			delayRight[1][1] = 25;
		}
		else if (LEVEL_MIN == 4) {
			//LEFT
			delayLeft[0][0] = 0;
			delayLeft[0][1] = 100;
			delayLeft[1][0] = 100;
			delayLeft[1][1] = 0;

			//RIGHT
			delayRight[0][0] = 0;
			delayRight[0][1] = 100;
			delayRight[1][0] = 100;
			delayRight[1][1] = 0;
		}
		else if (LEVEL_MIN == 5) {
			//LEFT
			delayLeft[0][0] = 25;
			delayLeft[0][1] = 75;
			delayLeft[1][0] = 100;
			delayLeft[1][1] = 0;

			//RIGHT
			delayRight[0][0] = 25;
			delayRight[0][1] = 75;
			delayRight[1][0] = 100;
			delayRight[1][1] = 0;
		}
		else if (LEVEL_MIN == 6) {
			//LEFT
			delayLeft[0][0] = 50;
			delayLeft[0][1] = 50;
			delayLeft[1][0] = 100;
			delayLeft[1][1] = 0;

			//RIGHT
			delayRight[0][0] = 50;
			delayRight[0][1] = 50;
			delayRight[1][0] = 100;
			delayRight[1][1] = 0;
		}
		else if (LEVEL_MIN == 7) {
			//LEFT
			delayLeft[0][0] = 75;
			delayLeft[0][1] = 25;
			delayLeft[1][0] = 100;
			delayLeft[1][1] = 0;

			//RIGHT
			delayRight[0][0] = 75;
			delayRight[0][1] = 25;
			delayRight[1][0] = 100;
			delayRight[1][1] = 0;
		}
		else {
			//LEFT
			delayLeft[0][0] = 100;
			delayLeft[0][1] = 0;
			delayLeft[1][0] = 100;
			delayLeft[1][1] = 0;

			//RIGHT
			delayRight[0][0] = 100;
			delayRight[0][1] = 0;
			delayRight[1][0] = 100;
			delayRight[1][1] = 0;
		}
	} 

	else {
		//LEFT CODE
		if (LEVEL_LEFT == 0) {

			delayLeft[0][0] = 0;
			delayLeft[0][1] = 100;
			delayLeft[1][0] = 0;
			delayLeft[1][1] = 100;
		}
		else if (LEVEL_LEFT == 1) {
			delayLeft[0][0] = 0;
			delayLeft[0][1] = 100;
			delayLeft[1][0] = 25;
			delayLeft[1][1] = 75;
		}
		else if (LEVEL_LEFT == 2) {
			delayLeft[0][0] = 0;
			delayLeft[0][1] = 100;
			delayLeft[1][0] = 50;
			delayLeft[1][1] = 50;
		}
		else if (LEVEL_LEFT == 3) {
			delayLeft[0][0] = 0;
			delayLeft[0][1] = 100;
			delayLeft[1][0] = 75;
			delayLeft[1][1] = 25;
		}
		else if (LEVEL_LEFT == 4) {
			delayLeft[0][0] = 0;
			delayLeft[0][1] = 100;
			delayLeft[1][0] = 100;
			delayLeft[1][1] = 0;
		}
		else if (LEVEL_LEFT == 5) {
			delayLeft[0][0] = 25;
			delayLeft[0][1] = 75;
			delayLeft[1][0] = 100;
			delayLeft[1][1] = 0;
		}
		else if (LEVEL_LEFT == 6) {
			delayLeft[0][0] = 50;
			delayLeft[0][1] = 50;
			delayLeft[1][0] = 100;
			delayLeft[1][1] = 0;
		}
		else if (LEVEL_LEFT == 7) {
			delayLeft[0][0] = 75;
			delayLeft[0][1] = 25;
			delayLeft[1][0] = 100;
			delayLeft[1][1] = 0;
		}
		else {
			delayLeft[0][0] = 100;
			delayLeft[0][1] = 0;
			delayLeft[1][0] = 100;
			delayLeft[1][1] = 0;
		}


		//RIGHT CODE
		if (LEVEL_RIGHT == 0) {
			delayRight[0][0] = 0;
			delayRight[0][1] = 100;
			delayRight[1][0] = 0;
			delayRight[1][1] = 100;
		}
		else if (LEVEL_RIGHT == 1) {
			delayRight[0][0] = 0;
			delayRight[0][1] = 100;
			delayRight[1][0] = 25;
			delayRight[1][1] = 75;
		}
		else if (LEVEL_RIGHT == 2) {
			delayRight[0][0] = 0;
			delayRight[0][1] = 100;
			delayRight[1][0] = 50;
			delayRight[1][1] = 50;
		}
		else if (LEVEL_RIGHT == 3) {
			delayRight[0][0] = 0;
			delayRight[0][1] = 100;
			delayRight[1][0] = 75;
			delayRight[1][1] = 25;
		}
		else if (LEVEL_RIGHT == 4) {
			delayRight[0][0] = 0;
			delayRight[0][1] = 100;
			delayRight[1][0] = 100;
			delayRight[1][1] = 0;
		}
		else if (LEVEL_RIGHT == 5) {
			delayRight[0][0] = 25;
			delayRight[0][1] = 75;
			delayRight[1][0] = 100;
			delayRight[1][1] = 0;
		}
		else if (LEVEL_RIGHT == 6) {
			delayRight[0][0] = 50;
			delayRight[0][1] = 50;
			delayRight[1][0] = 100;
			delayRight[1][1] = 0;
		}
		else if (LEVEL_RIGHT == 7) {
			delayRight[0][0] = 75;
			delayRight[0][1] = 25;
			delayRight[1][0] = 100;
			delayRight[1][1] = 0;
		}
		else {
			delayRight[0][0] = 100;
			delayRight[0][1] = 0;
			delayRight[1][0] = 100;
			delayRight[1][1] = 0;
		}
	}
}


//MOVE VALVE RIGHT PWM
void move_VALVE_RIGHT_PWM() {
	for (;;) {
		if (POWER == 1) {
			digitalWrite(VALVE_RIGHT_PWM, HIGH);
			delay(delayRight[0][0]);
			digitalWrite(VALVE_RIGHT_PWM, LOW);
			delay(delayRight[0][1]);
		}
		else {
			digitalWrite(VALVE_RIGHT_PWM, LOW);
			delay(200);
		}
	}
}

void *func_VALVE_RIGHT_PWM(void *args) {
    move_VALVE_RIGHT_PWM();
    return 0;
}

//MOVE VALVE LEFT PWM
void move_VALVE_LEFT_PWM() {
	for (;;) {
		if (POWER == 1) {
			digitalWrite(VALVE_LEFT_PWM, HIGH);
			delay(delayLeft[0][0]);
			digitalWrite(VALVE_LEFT_PWM, LOW);
			delay(delayLeft[0][1]);
		}
		else {
			digitalWrite(VALVE_LEFT_PWM, LOW);
			delay(200);
		}
	}
}

void *func_VALVE_LEFT_PWM(void *args) {
    move_VALVE_LEFT_PWM();
    return 0;
}


//MOVE VALVE RIGHT 
void move_VALVE_RIGHT() {
	for (;;) {
		if (POWER == 1) {
			digitalWrite(VALVE_RIGHT, HIGH);
			delay(delayRight[1][0]);
			digitalWrite(VALVE_RIGHT, LOW);
			delay(delayRight[1][1]);
		}
		else {
			digitalWrite(VALVE_RIGHT, LOW);
			delay(200);
		}
	}
}

void *func_VALVE_RIGHT(void *args) {
    move_VALVE_RIGHT();
    return 0;
}

//MOVE VALVE LEFT 
void move_VALVE_LEFT() {
	for (;;) {
		if (POWER == 1) {
			digitalWrite(VALVE_LEFT, HIGH);
			delay(delayLeft[1][0]);
			digitalWrite(VALVE_LEFT, LOW);
			delay(delayLeft[1][1]);
		}
		else {
			digitalWrite(VALVE_LEFT, LOW);
			delay(200);
		}
	}
}

void *func_VALVE_LEFT(void *args) {
    move_VALVE_LEFT();
    return 0;
}

void logToConsole() {
	for (;;) {
		printf("POWER: %d, C.O.B: %d, LEFT POWER: %d, RIGHT POWER %d", POWER, EQUAL, LEVEL_LEFT, LEVEL_RIGHT);
		delay(1000);
	}
}

void *func_logToConsole(void *args) {
	logToConsole();
	return 0;
}

void createThread() {
	pthread_create(&thread_VALVE_RIGHT_PWM, NULL, func_VALVE_RIGHT_PWM, NULL);
	pthread_create(&thread_VALVE_LEFT_PWM, NULL, func_VALVE_LEFT_PWM, NULL);
	pthread_create(&thread_VALVE_RIGHT, NULL, func_VALVE_RIGHT, NULL);
	pthread_create(&thread_VALVE_LEFT, NULL, func_VALVE_RIGHT, NULL);
	pthread_create(&thread_logToConsole, NULL, func_logToConsole, NULL);
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

	pinMode(VALVE_LEFT_PWM, OUTPUT);
	pinMode(VALVE_RIGHT_PWM, OUTPUT);
	pinMode(VALVE_LEFT, OUTPUT);
	pinMode(VALVE_RIGHT, OUTPUT);

	digitalWrite(VALVE_LEFT_PWM, LOW);
	digitalWrite(VALVE_RIGHT_PWM, LOW);
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

	createThread();
	
	pause(); 
	
	return 0;
}


int min (int a, int b) {
	return a < b ? a : b;
}
