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
#include <pthread.h>
#include "serial.h"
#include "image.h"

#include <linux/joystick.h>

#define NAME_LENGTH 	128

#define HOMETILT		90
#define HOMEPAN			90
#define DELTATILT		1
#define DELTAPAN		1
#define MAXTILTUP		115
#define MAXTILTDOWN		0
#define MAXPANLEFT		0
#define MAXPANRIGHT		180
#define CAMTHRESHOLD	3000
#define TURNTHRESHOLD	5
#define TRIGTHRESHOLD	-32000
#define CMDDELAYLO		50000000
#define CMDDELAYHI		10000000

#define MONITORTIMEOUT	1000000000

// Speed conversion defines
#define SPEEDMNUM		100
#define	SPEEDMDEN		65534

#define BUFSIZE			8

// strings
#define GAMEPAD			"/dev/input/js0"
#define WEBCAMIMAGE		"vlc-wrapper -I dummy v4l2:///dev/video1 --video-filter scene --no-audio --scene-path ~/test --scene-prefix image --scene-format ppm vlc://quit --run-time=1"
#define WEBCAMAUDIO		"arecord -f cd -d 5 audio.wav"

// make sure controller is set to XInput not DirectInput
// Gamepad buttons
#define BUTTONA			0
#define BUTTONB			1
#define BUTTONX			2
#define BUTTONY			3
#define BUTTONLB		4
#define BUTTONRB		5
#define	BUTTONBACK		6
#define BUTTONSTART		7
#define BUTTONLOGI		8
#define BUTTONLHAT		9
#define BUTTONRHAT		10

// Gamepad axes
#define	AXISLTLR		0
#define AXISLTUD		1
#define AXISLTRIG		2
#define AXISRTLR		3
#define AXISRTUD		4
#define AXISRTRIG		5
#define AXISDPADLR		6
#define AXISDPADUD		7

void CaptureAudio(void);
void CaptureImage(void);

void *MonitorPlatform(void *arg);

int main (int argc, char **argv)
{
	
	int *axis;
	int *button;
	struct js_event js;
	int fd;
	int tilt = HOMETILT, pan = HOMEPAN;
	int buttonAPressed = 0, buttonBPressed = 0, buttonXPressed = 0, buttonYPressed = 0, buttonRHATPressed = 0,
		buttonLBPressed = 0, buttonRBPressed = 0, buttonSTARTPressed = 0, buttonBACKPressed = 0;
		
	int aborted = 0;
	
	struct timespec prevTime, currTime;
	clockid_t clock;
	
	int cmdDelay = CMDDELAYHI;
	int leftMotor, rightMotor, speed, oldLeft = 0, oldRight = 0;
	
	pthread_t monitorThread;
	pthread_attr_t attr;
	
	unsigned char axes = 2;
	unsigned char buttons = 2;
	int version = 0x000800;
	char name[NAME_LENGTH] = "Unknown";
	
	char buffer[BUFSIZE] = {'0','1','2','3','4','5','6','7'};

	// open the gamepad port
	if ((fd = open(GAMEPAD, O_RDONLY)) < 0) {
		perror("Failed opening gamepad port!");
		exit(1);
	}

	// get info from gamepad driver
	ioctl(fd, JSIOCGVERSION, &version);
	ioctl(fd, JSIOCGAXES, &axes);
	ioctl(fd, JSIOCGBUTTONS, &buttons);
	ioctl(fd, JSIOCGNAME(NAME_LENGTH), name);

	printf("Joystick (%s) has %d axes and %d buttons. Driver version is %d.%d.%d.\n",
		name, axes, buttons, version >> 16, (version >> 8) & 0xff, version & 0xff);

	// allocate memory for structures
	axis = calloc(axes, sizeof(int));
	button = calloc(buttons, sizeof(char));

	// open and initialize the serial port
	SerialOpen();
	SerialInit();

	// open the gamepad port in nonblocking mode
	fcntl(fd, F_SETFL, O_NONBLOCK);

	printf("Running ... (interrupt to exit)\n");
	
	// initialize and set the thread attributes
 	pthread_attr_init( &attr );
 	pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_JOINABLE );
 	pthread_attr_setscope( &attr, PTHREAD_SCOPE_SYSTEM );
 	
 	if(pthread_create(&monitorThread, &attr, MonitorPlatform, NULL) != 0){
		printf("Error creating monitor thread!\n Exiting...\n");
		exit(11);
	}
	
	// get process clock timer
	clock_getcpuclockid(0, &clock);
	// get current time and set prevTime
	clock_gettime(clock, &prevTime);

	// main loop
	// does a nonblocking read of the gamepad
	// if we get an event, fill out the structures
	// checks the timer
	// if time is right
	// send commands to platform
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
		// didn't get an event, check the error to make sure nothing went wrong
		}else if (errno != EAGAIN) { 
			perror("\njoystick: error reading");
			exit (1);
		}
		
		// get current time
		//clock_getcpuclockid(0, &clock);
		clock_gettime(clock, &currTime);
		
		// if some amount of time has passed, use the current structure
		// to send commands to the camera/platform
		if(!(((unsigned)currTime.tv_nsec - (unsigned)prevTime.tv_nsec) < cmdDelay)){
			
			// do a ping every time we send commands, so we can check to see if 
			// the platform is responding
			//snprintf(buffer, BUFSIZE+1, "png00000");
			//SerialWrite((unsigned char *)buffer, BUFSIZE);
			
// *********************** A Button ************************************
			if(button[BUTTONA]){
				// Take a picture
				if(buttonAPressed == 0){
					printf("Photo Sample");
					CaptureImage();
					buttonAPressed = 1;
				}
			}else
				buttonAPressed = 0;
				
// ************************ B Button ***********************************
			if(button[BUTTONB]){
				// Take an audio sample
				if(buttonBPressed == 0){
					printf("Audio Sample");
					CaptureAudio();
					buttonBPressed = 1;
				}
			}else
				buttonBPressed = 0;
				
// *********************** X Button ************************************
			if(button[BUTTONX]){
				// Take an audio sample
				if(buttonXPressed == 0){
					snprintf(buffer, BUFSIZE+1, "png00000");
					SerialWrite((unsigned char *)buffer,BUFSIZE);
					tilt = HOMETILT;
					pan = HOMEPAN;
					buttonXPressed = 1;
				}
			}else
				buttonXPressed = 0;
				
// ********************* Y Button **************************************
			if(button[BUTTONY]){
				// Take an audio sample
				if(buttonYPressed == 0){
					snprintf(buffer, BUFSIZE+1, "hom00000");
					SerialWrite((unsigned char *)buffer,BUFSIZE);
					buttonYPressed = 1;
					tilt = HOMETILT;
					pan = HOMEPAN;
				}
			}else
				buttonYPressed = 0;
				
// ****************** Left Button **************************************
			if(button[BUTTONLB]){
				// toggle command delay
				if(buttonLBPressed == 0){
					if(cmdDelay == CMDDELAYHI)
						cmdDelay = CMDDELAYLO;
					else
						cmdDelay = CMDDELAYHI;
					buttonLBPressed = 1;
				}
			}else
				buttonLBPressed = 0;
				
// ********************** Right Button *********************************
			if(button[BUTTONRB]){
				// toggle command delay
				if(buttonRBPressed == 0){
					snprintf(buffer, BUFSIZE+1, "cal00000");
					SerialWrite((unsigned char *)buffer,BUFSIZE);
					buttonRBPressed = 1;
					tilt = HOMETILT;
					pan = HOMEPAN;
				}
			}else
				buttonRBPressed = 0;
				
// ********************** Right Hat ************************************
			if(button[BUTTONRHAT]){
				// toggle laser sight
				if(buttonRHATPressed == 0){
					snprintf(buffer, BUFSIZE+1, "aim00000");
					SerialWrite((unsigned char *)buffer, BUFSIZE);
					buttonRHATPressed = 1;
				}
			}else
				buttonRHATPressed = 0;
				
// ********************** Start Button *********************************
			if(button[BUTTONSTART]){
				// only resume if aborted first
				if(aborted)
					// toggle command delay
					if(buttonSTARTPressed == 0){
						snprintf(buffer, BUFSIZE+1, "res00000");
						SerialWrite((unsigned char *)buffer,BUFSIZE);
						tilt = HOMETILT;
						pan = HOMEPAN;
						aborted = 0;
						buttonSTARTPressed = 1;
					}
			}else
				buttonSTARTPressed = 0;
				
// ********************** Back Button *********************************
			if(button[BUTTONBACK]){
				// toggle command delay
				if(buttonBACKPressed == 0){
					snprintf(buffer, BUFSIZE+1, "abt00000");
					SerialWrite((unsigned char *)buffer,BUFSIZE);
					aborted = 1;
					buttonBACKPressed = 1;
				}
			}else
				buttonBACKPressed = 0;

// ********************* Right Thumbstick ******************************
			if(axis[AXISRTLR] > CAMTHRESHOLD){
				// pan left
				pan += DELTAPAN;
				if(pan > MAXPANRIGHT)
					pan = MAXPANRIGHT;
				// pan camera
				snprintf(buffer, BUFSIZE+1, "pan1 %3d", pan);
				SerialWrite((unsigned char *)buffer,BUFSIZE);
			}else if(axis[AXISRTLR] < -CAMTHRESHOLD){
				// pan right
				pan -= DELTAPAN;
				if(pan < MAXPANLEFT)
					pan = MAXPANLEFT;
				// pan camera
				snprintf(buffer, BUFSIZE+1, "pan1 %3d", pan);
				SerialWrite((unsigned char *)buffer,BUFSIZE);
			}
			
			if(axis[AXISRTUD] > CAMTHRESHOLD){
				// tilt down
				tilt -= DELTATILT;
				if(tilt < MAXTILTDOWN)
					tilt = MAXTILTDOWN;
				// tilt camera
				snprintf(buffer, BUFSIZE+1, "tlt1 %3d", tilt);
				SerialWrite((unsigned char *)buffer,BUFSIZE);
			}else if(axis[AXISRTUD] < -CAMTHRESHOLD){
				// tilt up
				tilt += DELTATILT;
				if(tilt > MAXTILTUP)
					tilt = MAXTILTUP;
				// tilt camera
				snprintf(buffer, BUFSIZE+1, "tlt1 %3d", tilt);
				SerialWrite((unsigned char *)buffer,BUFSIZE);
			}
			
// ************************ Speed Updates ******************************
			// convert trigger values into speed
			speed = (SPEEDMNUM * (axis[AXISRTRIG] - axis[AXISLTRIG])) / SPEEDMDEN;
			
			// turn left
			if(axis[AXISLTLR] < -TURNTHRESHOLD){
				leftMotor = speed * (100 + ((SPEEDMNUM * axis[AXISLTLR]) / 32767)) / 100;
				rightMotor = speed;
			// turn right
			}else if(axis[AXISLTLR] > TURNTHRESHOLD){
				rightMotor = speed * (100 - ((SPEEDMNUM * axis[AXISLTLR]) / 32767)) / 100;
				leftMotor = speed;
			// go straight
			}else{
				rightMotor = speed;
				leftMotor = speed;
			}
			
			// check D-Pad for spin
			// spin left
			if(axis[AXISDPADLR] < 0){
				leftMotor = -speed;
				rightMotor = speed;
			// spin right
			}else if(axis[AXISDPADLR] > 0){
				leftMotor = speed;
				rightMotor = -speed;
			}
				
			// only send drive commands if the motor speed has changed
			if(leftMotor != oldLeft){
				// send commands to drive both motors
				// left motor
				snprintf(buffer, BUFSIZE+1, "mov0%4d", leftMotor);
				SerialWrite((unsigned char *)buffer,BUFSIZE);
				oldLeft = leftMotor;
				//printf("%s\n", buffer);
			}
			if(rightMotor != oldRight){
				// right motor
				snprintf(buffer, BUFSIZE+1, "mov1%4d", rightMotor);
				SerialWrite((unsigned char *)buffer,BUFSIZE);
				oldRight = rightMotor;
				//printf("%s\n", buffer);
			}

			// get the current time and put it in prevTime
			clock_gettime(clock, &prevTime);
		}
			
		fflush(stdout);
	}
}

// calls "arecord -f cd -d 5 audio.wav"
void CaptureAudio(void){
	system(WEBCAMAUDIO);
}

// calls "vlc -I dummy v4l2:///dev/video1 --video-filter scene --no-audio --scene-path ~/test --scene-prefix image --scene-format bmp vlc://quit --run-time=1"
void CaptureImage(void){
	system(WEBCAMIMAGE);
	AnalyzeImage();
}

// monitors the serial connection for platform response
void * MonitorPlatform(void *arg){
	unsigned char monitorBuffer[8] = {0};
	struct timespec monitorPrevTime, monitorCurrTime;
	int numBytes = 0;
	clockid_t monitorClock;
	
	// get process clock timer
	clock_getcpuclockid(0, &monitorClock);
	// get current time and set prevTime
	clock_gettime(monitorClock, &monitorPrevTime);
	
	
	while(1){
		numBytes = SerialRead(monitorBuffer, 8);
		
		if(numBytes > 0){
			printf("%s\n", monitorBuffer);
		
			// get the current time
			clock_gettime(monitorClock, &monitorCurrTime);
		
			if(((unsigned)monitorCurrTime.tv_nsec - (unsigned)monitorPrevTime.tv_nsec) > MONITORTIMEOUT){
				printf("Large delay between pings detected - Platform error!\n");
			}
		}
	}
	
	pthread_exit(NULL);
}
