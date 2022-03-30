#include <wiringPi.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#define VALVE_1 23
#define VALVE_2 21
#define VALVE_3 16
#define VALVE_4 19


void cleanUp(int signo) {
	//GPIO clean up:

	//Valves to low
	digitalWrite(VALVE_1, LOW);
	digitalWrite(VALVE_2, LOW);
	digitalWrite(VALVE_3, LOW);
	digitalWrite(VALVE_4, LOW);

	//Valves to input
	pinMode(VALVE_1, INPUT);
	pinMode(VALVE_2, INPUT);
	pinMode(VALVE_3, INPUT);
	pinMode(VALVE_4, INPUT);

	exit(0);
}

int main() {
	//If Turned off
	signal(SIGINT, cleanUp);
	signal(SIGTERM, cleanUp);
	signal(SIGHUP, cleanUp);

	wiringPiSetupGpio();

	pinMode(VALVE_1, OUTPUT);
	pinMode(VALVE_2, OUTPUT);
	pinMode(VALVE_3, OUTPUT);
	pinMode(VALVE_4, OUTPUT);

	digitalWrite(VALVE_1, LOW);
	digitalWrite(VALVE_2, LOW);
	digitalWrite(VALVE_3, LOW);
	digitalWrite(VALVE_4, LOW);

	for (;;) {
		digitalWrite(VALVE_1, HIGH);
		delay(500);
		digitalWrite(VALVE_1, LOW);


		digitalWrite(VALVE_2, HIGH);
		delay(500);
		digitalWrite(VALVE_2, LOW);


		digitalWrite(VALVE_3, HIGH);
		delay(500);
		digitalWrite(VALVE_3, LOW);


		digitalWrite(VALVE_4, HIGH);
		delay(500);
		digitalWrite(VALVE_4, LOW);
	}

	return 0;
}

