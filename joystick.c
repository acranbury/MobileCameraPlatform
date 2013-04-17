#include <sys/ioctl.h>
#include <sys/time.h>
#include <time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "serial.h"

#include <linux/joystick.h>

#define NAME_LENGTH 128

#define HOMETILT	90
#define HOMEPAN		90
#define DELTATILT	1
#define DELTAPAN	1
#define MAXTILTUP	120
#define MAXTILTDOWN	0
#define MAXPANLEFT	0
#define MAXPANRIGHT	180

#define BUFSIZE		8

#define GAMEPAD		"/dev/input/js0"

// Gamepad buttons
#define BUTTONA		0
#define BUTTONB		1
#define BUTTONX		2
#define BUTTONY		3
#define BUTTONLB	4
#define BUTTONRB	5
#define	BUTTONBACK	6
#define BUTTONSTART	7
#define BUTTONLOGI	8
#define BUTTONLHAT	9
#define BUTTONRHAT	10

// Gamepad axes
#define	AXISLTLR	0
#define AXISLTUD	1
#define AXISLTRIG	2
#define AXISRTLR	3
#define AXISRTUD	4
#define AXISRTRIG	5
#define AXISDPADLR	6
#define AXISDPADUD	7

// structure for gamepad
struct gamepad{
	int buttonA,buttonB,buttonX,rightThumbUD,rightThumbLR;
}controller;

int main (int argc, char **argv)
{
	
	int *axis;
	int *button;
	struct js_event js;
	int fd;
	int tilt = HOMETILT, pan = HOMEPAN;
	int buttonAPressed = 0, buttonBPressed = 0, buttonXPressed = 0;
	struct timespec *timer;
	unsigned char axes = 2;
	unsigned char buttons = 2;
	long currTime, prevTime;
	int version = 0x000800;
	char name[NAME_LENGTH] = "Unknown";
	
	char buffer[BUFSIZE] = {'0','1','2','3','4','5','6','7'};

	// open the gamepad port
	if ((fd = open(GAMEPAD, O_RDONLY)) < 0) {
		perror("joystick");
		exit(1);
	}

	// get info from gamepad driver
	ioctl(fd, JSIOCGVERSION, &version);
	ioctl(fd, JSIOCGAXES, &axes);
	ioctl(fd, JSIOCGBUTTONS, &buttons);
	ioctl(fd, JSIOCGNAME(NAME_LENGTH), name);

	printf("Joystick (%s) has %d axes and %d buttons. Driver version is %d.%d.%d.\n",
		name, axes, buttons, version >> 16, (version >> 8) & 0xff, version & 0xff);
	printf("Testing ... (interrupt to exit)\n");

	// allocate memory for structures
	axis = calloc(axes, sizeof(int));
	button = calloc(buttons, sizeof(char));
	timer = calloc(1, sizeof(struct timespec));

	// open and initialize the serial port
	SerialOpen();
	SerialInit();
	//SerialWrite(buffer,10);

	// open the gamepad port in nonblocking mode
	fcntl(fd, F_SETFL, O_NONBLOCK);

	// get current time
	clock_gettime(CLOCK_REALTIME, timer);
	currTime = timer->tv_nsec;

	while (1) {
		
		// do a read of the gamepad port
		if(read(fd, &js, sizeof(struct js_event)) == sizeof(struct js_event)){
			
			// get the type of event, fill out button or axis appropriately
			switch(js.type & ~JS_EVENT_INIT) {
			case JS_EVENT_BUTTON:
				button[js.number] = js.value;
				break;
			case JS_EVENT_AXIS:
				axis[js.number] = js.value;
				break;
			}	

			// fill out the structure
			controller.buttonA = button[BUTTONA];
			controller.buttonB = button[BUTTONB];
			controller.buttonX = button[BUTTONX];
			controller.rightThumbLR = axis[AXISRTLR];
			controller.rightThumbUD = axis[AXISRTUD];

		}else
			if (errno != EAGAIN) {
			perror("\njoystick: error reading");
			exit (1);
		}
		
		// get current time
		clock_gettime(CLOCK_REALTIME, timer);
		prevTime = currTime;
		currTime = timer->tv_nsec;
		
		// if some amount of time has passed, use the current structure
		// to send commands to the camera/platform
		if(((unsigned)currTime - (unsigned)prevTime) > 15000){
			if(controller.buttonA){
				// Take a picture
				if(buttonAPressed == 0){
					printf("Photo Sample");
					buttonAPressed = 1;
				}
			}else
				buttonAPressed = 0;
				
			if(controller.buttonB){
				// Take an audio sample
				if(buttonBPressed == 0){
					printf("Audio Sample");
					buttonBPressed = 1;
				}
			}else
				buttonBPressed = 0;
				
			if(controller.buttonX){
				// Take an audio sample
				if(buttonXPressed == 0){
					snprintf(buffer, BUFSIZE+1, "png00000");
					SerialWrite((unsigned char *)buffer,BUFSIZE);
					buttonXPressed = 1;
				}
			}else
				buttonXPressed = 0;

			if(controller.rightThumbLR > 500){
				// pan left
				pan += DELTAPAN;
				if(pan > MAXPANRIGHT)
					pan = MAXPANRIGHT;
				// pan camera
				//SerialWrite("", 8);
				printf("Pan: %d", pan);
				snprintf(buffer, BUFSIZE+1, "pan1 %3d", pan);
				SerialWrite((unsigned char *)buffer,BUFSIZE);
			}
			else if(controller.rightThumbLR < -500){
				// pan right
				pan -= DELTAPAN;
				if(pan < MAXPANLEFT)
					pan = MAXPANLEFT;
				// pan camera
				printf("Pan: %d", pan);
				snprintf(buffer, BUFSIZE+1, "pan1 %3d", pan);
				SerialWrite((unsigned char *)buffer,BUFSIZE);
			}
			
			if(controller.rightThumbUD > 500){
				// tilt down
				tilt -= DELTATILT;
				if(tilt < MAXTILTDOWN)
					tilt = MAXTILTDOWN;
				// tilt camera
				printf("Tilt: %d", tilt);
				snprintf(buffer, BUFSIZE+1, "tlt1 %3d", tilt);
				SerialWrite((unsigned char *)buffer,BUFSIZE);
			}
			else if(controller.rightThumbUD < -500){
				// tilt up
				tilt += DELTATILT;
				if(tilt > MAXTILTUP)
					tilt = MAXTILTUP;
				// tilt camera
				printf("Tilt: %d", tilt);
				snprintf(buffer, BUFSIZE+1, "tlt1 %3d", tilt);
				SerialWrite((unsigned char *)buffer,BUFSIZE);
			}
		}
			
		fflush(stdout);
	}
}
