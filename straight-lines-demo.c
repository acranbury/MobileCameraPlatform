#include <stdio.h>
#include "serial.h"

#define SPINMODIFIER	2.356	// turns degrees into wheel distance

void WaitForDTMF(int tone);
void Drive(int distance, int time);
void Spin(int degrees);

int main(int argc, char *argv[]){
	
	// open and initialize the serial port
	SerialOpen();
	SerialInit();
	
	// Wait at A until hears DTMF 1
	WaitForDTMF(1);
	
	// Travels a straight path for 1.73m in 10 sec
	Drive(173, 10);

	
	// Turns on the spot 90 degrees
	Spin(90);
	
	// Wait at B until hears DTMF 5
	WaitForDTMF(5);
	
	// Travels a straight path for 1 m in 10 sec
	Drive(100, 10);
	
	// Turns on the spot 120 degrees
	Spin(120);
	
	// Wait at C until hears DTMF 9
	WaitForDTMF(9);
	
	// Travels a straight path for 2 m in 10 sec
	Drive(200, 10);
	
	// Turns on the spot 150 degrees and stops
	Spin(150);
	
	// close serial port
	SerialClose();
	
	return(0);
}

// wait for a DTMF tone
void WaitForDTMF(int tone){
}

// drive forwards a distance in some time
void Drive(int distance, int time){
	char buffer[33];
	
	// compute the speed
	float speed = distance/time;
	
	// write the distance command to the serial port
	sprintf(buffer, "dst00%3ddst01%3ddst10%3ddst11%3d", (int)speed, distance, (int)speed, distance);
	SerialWrite((unsigned char *)buffer, 32);
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
