#include <stdio.h>
#include "serial.h"
#include "dtmf.h"


#define MAXSPEED		x
#define SPINMODIFIER	2.356	// converts degrees into wheel distance
#define BIGCIRCLE		0
#define SMALLCIRCLE		1
#define D1				4580		// distance inner wheel travels on small circle
#define D2				6280		// distance outer wheel for small circle, inner wheel for large circle
#define D3				7980		// distance outer wheel travels on large circle

int WaitForDTMF(char tone);
void TravelCircularPath(int size);
void Pivot180();

int main(int argc, char *argv[]){

	// open and initialize the serial port
	SerialOpen();
	SerialInit();

	// Wait at point A until hears DTMF D
	if(!WaitForDTMF('D'){
		printf("No tone detected\n");
		exit(1);
	}

	// Travels circular path (A, B, C) full circle in 1 min
	TravelCircularPath(0);

	// Waits at point C until hears DTMF *
	if(!WaitForDTMF('*')){
		printf("No tone detected\n");
		exit(1);
	}

	// Pivots 180 degrees to point D
	Pivot180();

	// Waits at point D until hears DTMF A
	if(!WaitForDTMF('A')){
		printf("No tone detected\n");
		exit(1);
	}

	// Travels circular path (D, E, F) full circle in 90 seconds and stops
	TravelCircularPath(1);

	// close serial port
	SerialClose();

	return(0);
}

// wait for a DTMF tone
void WaitForDTMF(char tone){
}

// travel a circular path
void TravelCircularPath(int size){
	int distanceRight, distanceLeft;
	float speedRight, speedLeft;
	char buffer[17];

	switch(size){
		case BIGCIRCLE:
			distanceRight = D2;
			distanceLeft = D3;
			break;
		case SMALLCIRCLE:
			distanceRight = D2;
			distanceLeft = D1;
			break;
		default:
			break;
	}

	speedRight = distanceRight / 60;
	speedLeft = distanceLeft / 60;

	sprintf(buffer, "dst00%3ddst01%3d", distanceLeft, (int)speedLeft);
	SerialWrite(buffer, 16);
	sprintf(buffer, "dst10%3ddst11%3d", distanceRight, (int)speedRight);
	SerialWrite(buffer, 16);
}

// pivot on the spot 180 degrees
void Pivot180(){
	char buffer[17];
	
	float distance = 180 * SPINMODIFIER * 2;

	int speed = MAXSPEED;

	// write the command to the serial port
	sprintf(buffer, "dst00%3ddst01%3d", (int)distance, speed);
	SerialWrite(buffer, 16);
}
