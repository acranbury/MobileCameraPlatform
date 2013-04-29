#include <stdio.h>
#include "serial.h"
#include "dtmf.h"

#define SPINMODIFIER	2.356	// turns degrees into wheel distance

void Drive(int distance, int time);
void Spin(int degrees);

int main(int argc, char *argv[]){

	// open and initialize the serial port
	SerialOpen();
	SerialInit();

	// Wait at A until hears DTMF 1
	//if(!WaitonDTMF('1')){
		//printf("No tone detected\n");
		//return(1);
	//}

	// Travels a straight path for 1.73m in 10 sec
	Drive(1730, 10);

	sleep(10);

	// Turns on the spot 90 degrees
	Spin(45);
	
	sleep(2);

	// Wait at B until hears DTMF 5
	//if(!WaitonDTMF('5')){
		//printf("No tone detected\n");
		//return(1);
	//}

	// Travels a straight path for 1 m in 10 sec
	Drive(1000, 10);

	sleep(10);

	// Turns on the spot 120 degrees
	Spin(60);
	
	sleep(2);

	// Wait at C until hears DTMF 9
	//if(!WaitonDTMF('9')){
		//printf("No tone detected\n");
		//return(1);
	//}

	// Travels a straight path for 2 m in 10 sec
	Drive(2000, 10);
	
	sleep(10);

	// Turns on the spot 150 degrees and stops
	Spin(75);
	
	sleep(2);

	// close serial port
	SerialClose();

	return(0);
}


// drive forwards a distance in some time
void Drive(int distance, int time){
	char buffer[17];

	// compute the speed
	float speed = distance/time;
	if(speed < 204)
		speed = 204;
	distance /= 10;

	// write the distance comman"spn%5d"d to the serial port
	sprintf(buffer, "dst20%03ddst21%03d", (int)speed, distance);
	printf("%s\n", buffer);
	SerialWrite((unsigned char *)buffer, 16);
}

// spin the robot some degrees
void Spin(int degrees){
	char buffer[9];

	// compute the distance the wheels need to travel
	float distance = degrees * SPINMODIFIER;

	// write the spin command to the serial port
	sprintf(buffer, "spn%5d", (int)distance);
	SerialWrite((unsigned char *)buffer, 8);
}
