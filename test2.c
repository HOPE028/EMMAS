#include <wiringPi.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#define BUTTON 17
#define VALVE 5

void cleanUp(int signo) {
	pullUpDnControl(BUTTON, PUD_DOWN);
	digitalWrite(VALVE, LOW);
	pinMode(VALVE, INPUT);
	exit(0);
}

unsigned short int isPressed(unsigned short int button) {
	static struct timespec lastCall;
	struct timespec thisCall;
	float timeDiff;
	
	clock_gettime(CLOCK_REALTIME, &thisCall);
	timeDiff = (thisCall.tv_sec + thisCall.tv_nsec/1E9 - lastCall.tv_sec - lastCall.tv_nsec/1E9)*5;
	lastCall = thisCall;
	
	return timeDiff > 1 ? 1 : 0;
	
}

void goButton() {
	if (isPressed(BUTTON)) {
		digitalWrite(VALVE, HIGH);
	
		delay(3000);
		digitalWrite(VALVE, LOW);
	}
}

int main(void) {
	signal(SIGINT, cleanUp);
	signal(SIGTERM, cleanUp);
	signal(SIGHUP, cleanUp);
	
	wiringPiSetupGpio();
	
	pinMode(BUTTON, INPUT);
	pinMode(VALVE, OUTPUT);
	digitalWrite(VALVE, LOW);
	
	pullUpDnControl(BUTTON, PUD_UP);
	
	wiringPiISR(BUTTON, INT_EDGE_FALLING, goButton);
	
	pause(); 
	
	return 0;
}

