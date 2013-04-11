
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <linux/joystick.h>

#define NAME_LENGTH 128

#define HOMETILT	0
#define HOMEPAN		0
#define DELTATILT	1
#define DELTAPAN	1

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

int main (int argc, char **argv)
{
	int fd;
	int tilt = HOMETILT, pan = HOMEPAN;
	int tiltPressed = 0, panPressed = 0;
	unsigned char axes = 2;
	unsigned char buttons = 2;
	int version = 0x000800;
	char name[NAME_LENGTH] = "Unknown";

	if (argc < 2 || argc > 3 || !strcmp("--help", argv[1])) {
		puts("");
		puts("Usage: joystick [<mode>] <device>");
		puts("");
		puts("Modes:");
		puts("  --normal           One-line mode showing immediate status");
		puts("  --event            Prints events as they come in");
		puts("");
		exit(1);
	}
	if ((fd = open(argv[argc - 1], O_RDONLY)) < 0) {
		perror("joystick");
		exit(1);
	}

	ioctl(fd, JSIOCGVERSION, &version);
	ioctl(fd, JSIOCGAXES, &axes);
	ioctl(fd, JSIOCGBUTTONS, &buttons);
	ioctl(fd, JSIOCGNAME(NAME_LENGTH), name);

	printf("Joystick (%s) has %d axes and %d buttons. Driver version is %d.%d.%d.\n",
		name, axes, buttons, version >> 16, (version >> 8) & 0xff, version & 0xff);
	printf("Testing ... (interrupt to exit)\n");

/*
 * Event interface, single line readout.
 */

	if (argc == 2 || !strcmp("--normal", argv[1])) {

		int *axis;
		int *button;
		int i;
		struct js_event js;

		axis = calloc(axes, sizeof(int));
		button = calloc(buttons, sizeof(char));

		while (1) {
			if (read(fd, &js, sizeof(struct js_event)) != sizeof(struct js_event)) {
				perror("\njstest: error reading");
				exit (1);
			}
			
			switch(js.type & ~JS_EVENT_INIT) {
			case JS_EVENT_BUTTON:
				button[js.number] = js.value;
				break;
			case JS_EVENT_AXIS:
				axis[js.number] = js.value;
				break;
			}

			printf("\r");

			if (axes) {
				printf("Axes: ");
				for (i = 0; i < axes; i++)
					printf("%2d:%6d ", i, axis[i]);
			}

			if (buttons) {
				printf("Buttons: ");
				for (i = 0; i < buttons; i++)
					printf("%2d:%s ", i, button[i] ? "on " : "off");
			}

			// Button A pressed
			if(button[BUTTONA]){
				// take a picture
				// TakePhotoSample();
				printf("Photo Sample\n");
			}
			
			// Button B pressed
			if(button[BUTTONB]){
				// take audio sample
				// TakeAudioSample();
				printf("Audio Sample\n");
			}

			// DPAD up/down pressed
			if(axis[AXISDPADUD]){
				tiltPressed = 1;
			}
			else
				tiltPressed = 0;
			
			if(tiltPressed){
				// send camera tilt command
				if(axis[AXISDPADUD] > 0){
					// tilt down
					tilt -= DELTATILT;
				}
				else{
					// tilt up
					tilt += DELTATILT;
				}
				printf("Tilt: %d", tilt);
			}

			// DPAD left/right pressed
			if(axis[AXISDPADLR]){
				panPressed = 1;
			}else
				panPressed = 0;

			if(panPressed){
				// send camera pan command
				if(axis[AXISDPADLR] > 0){
					// pan left
					pan += DELTAPAN;
				}
				else{
					// pan right
					pan -= DELTAPAN;
				}
				printf("Pan: %d", pan);
			}
			
			fflush(stdout);
		}
	}


/*
 * Reading in nonblocking mode.
 */

	if (!strcmp("--nonblock", argv[1])) {

		int *axis;
		int *button;
		struct js_event js;

		axis = calloc(axes, sizeof(int));
		button = calloc(buttons, sizeof(char));

		fcntl(fd, F_SETFL, O_NONBLOCK);

		while (1) {
			read(fd, &js, sizeof(struct js_event));
			if (errno != EAGAIN) {
				perror("\njstest: error reading");
				exit (1);
			}

			switch(js.type & ~JS_EVENT_INIT) {
				case JS_EVENT_BUTTON:
					button[js.number] = js.value;
					break;
				case JS_EVENT_AXIS:
					axis[js.number] = js.value;
					break;
			}

			// Button A pressed
			if(button[BUTTONA]){
				// take a picture
				// TakePhotoSample();
				printf("Photo Sample\n");
			}
		
			// Button B pressed
			if(button[BUTTONB]){
				// take audio sample
				// TakeAudioSample();
				printf("Audio Sample\n");
			}

			// DPAD up/down pressed
			if(axis[AXISDPADUD])
				tiltPressed = 1;
			else
				tiltPressed = 0;
		
			// DPAD left/right pressed
			if(axis[AXISDPADLR])
				panPressed = 1;
			else
				panPressed = 0;
			
			if(tiltPressed){
				// send camera tilt command
				if(axis[AXISDPADUD] > 0){
					// tilt down
					tilt -= DELTATILT;
				}
				else{
					// tilt up
					tilt += DELTATILT;
				}
				printf("Tilt: %d", tilt);
			}

			if(panPressed){
				// send camera pan command
				if(axis[AXISDPADLR] > 0){
					// pan left
					pan += DELTAPAN;
				}
				else{
					// pan right
					pan -= DELTAPAN;
				}
				printf("Pan: %d", pan);
			}
		
			fflush(stdout);
			}

			usleep(10000);
		}

	printf("joystick: unknown mode: %s\n", argv[1]);
	return -1;
}
