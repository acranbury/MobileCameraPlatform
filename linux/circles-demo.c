#include <stdio.h>
#include "serial.h"
#include "goertzel.h"


#define MAXSPEED		337
#define SPINMODIFIER	0.2356	// converts degrees into wheel distance
#define BIGCIRCLE		0
#define SMALLCIRCLE		1
#define PIVOT180		81
#define D1				4580		// distance inner wheel travels on small circle
#define D2				6280		// distance outer wheel for small circle, inner wheel for large circle
#define D3				7980		// distance outer wheel travels on large circle

#define SPEEDSMALLRIGHT	DISTSMALLRIGHT / 90
#define SPEEDSMALLLEFT	DISTSMALLLEFT / 90
#define DISTSMALLRIGHT	4580
#define DISTSMALLLEFT	6280

#define SPEEDLARGERIGHT	DISTLARGERIGHT / 60
#define SPEEDLARGELEFT	DISTLARGELEFT / 60
#define DISTLARGERIGHT	7980
#define DISTLARGELEFT	6280

//int WaitForDTMF(char tone);
void TravelCircularPath(int size);
void Pivot180();

int main(int argc, char *argv[]){

	// open and initialize the serial port
	SerialOpen();
	SerialInit();
	/*
	// Wait at point A until hears DTMF D
	if(!WaitonDTMF('D')){
		printf("No tone detected\n");
		return(1);
	}

	// Travels circular path (A, B, C) full circle in 1 min
	TravelCircularPath(BIGCIRCLE);
	
	sleep(70);
*/
	// Waits at point C until hears DTMF *
	if(!WaitonDTMF('*')){
		printf("No tone detected\n");
		return(1);
	}

	// Pivots 180 degrees to point D
	Pivot180();
	
	sleep(5);
/*
	// Waits at point D until hears DTMF A
	if(!WaitonDTMF('A')){
		printf("No tone detected\n");
		return(1);
	}

	// Travels circular path (D, E, F) full circle in 90 seconds and stops
	TravelCircularPath(SMALLCIRCLE);
	
	sleep(100);
*/
	// close serial port
	SerialClose();

	return(0);
}

// wait for a DTMF tone
//void WaitForDTMF(char tone){
//}

// travel a circular path
void TravelCircularPath(int size){
	int distanceRight, distanceLeft;
	int speedRight, speedLeft;
	char buffer[17];

	switch(size){
		case BIGCIRCLE:
			distanceRight = DISTLARGELEFT;
			distanceLeft = DISTLARGERIGHT;
			
			speedRight = SPEEDLARGELEFT;
			speedLeft = SPEEDLARGERIGHT;
			break;
		case SMALLCIRCLE:
			distanceRight = DISTSMALLRIGHT;
			distanceLeft = DISTSMALLLEFT;
			
			speedRight = SPEEDSMALLRIGHT;
			speedLeft = SPEEDSMALLLEFT;
			break;
		default:
			break;
	}

	

	sprintf(buffer, "dst00%3ddst01%3d", (int)speedLeft, distanceLeft/10);
	SerialWrite((unsigned char *)buffer, 16);
	printf("%s\n", buffer);
	sprintf(buffer, "dst10%3ddst11%3d", (int)speedRight, distanceRight/10);
	SerialWrite((unsigned char *)buffer, 16);
	printf("%s\n", buffer);
}

// pivot on the spot 180 degrees
void Pivot180(){
	char buffer[17];
	
	int distance = PIVOT180;
	
	int speed = MAXSPEED;
	
	// write the command to the serial port
	sprintf(buffer, "dst10%3ddst11%3d", speed, distance);
	SerialWrite((unsigned char *)buffer, 16);
	printf("%s\n", buffer);
}
