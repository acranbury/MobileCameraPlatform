#include <stdio.h>
#include "image.h"

#define PIXELSTART	55
#define FILENAME	"image00001.ppm"
#define FILE2		"test00002.ppm"
#define FILESTART	"P6\n640 480\n255\n"
#define HEIGHT		480
#define WIDTH		640
#define MAXVALUE	255
#define HALFVALUE	MAXVALUE/2

unsigned char ApplyThreshold(unsigned char pixel[3]);

void AnalyzeImage(){

	FILE * fp, *fp2;
	
	int i, j;
	
	unsigned char pixel[3];
	unsigned char normalized[WIDTH][HEIGHT];
	//char buffer[8];
	float columnSum[WIDTH];
	float difference[WIDTH] = {0};
	float sumDifference = 0;
	float averageDifference = 0;
	float prevSum = 0;
	float divisor = HEIGHT - 1;
	
	// open file in binary mode
	fp = fopen(FILENAME, "rb");
	fp2 = fopen(FILE2, "wb");
	
	fwrite(FILESTART, 1, 15, fp2);
	
	if (fp != NULL && fp2 != NULL)
	{
		// go to the beginning of the pixel data
		fseek(fp, PIXELSTART, SEEK_SET);
		fseek(fp2, PIXELSTART, SEEK_SET);
		
		for(i = 0; i < HEIGHT; i++){
			for(j = 0; j < WIDTH; j++){
				
				// grab a pixel
				fread(pixel, 1, 3, fp);
				normalized[j][i] = ApplyThreshold(pixel);
				fwrite(&normalized[j][i], 1, 1, fp2);
				fwrite(&normalized[j][i], 1, 1, fp2);
				fwrite(&normalized[j][i], 1, 1, fp2);
			}
		}
		
		for(i = 0; i < WIDTH; i++){
			columnSum[i] = 0;
			for(j = 0; j < HEIGHT; j++){
				columnSum[i] += (float)normalized[i][j];
			}
			difference[i] = columnSum[i] - prevSum;
			//printf("Column Sum: %f prevSum: %f Difference: %f \n", columnSum[i], prevSum,  difference[i]);
			prevSum = columnSum[i];
		}
		
		for(i = 1; i < HEIGHT; i++){
			if(difference[i] != HEIGHT * MAXVALUE && difference[i] < 1000 && difference[i] > -1000){
				sumDifference += difference[i];			
			}else
				divisor--;
		}
		averageDifference = sumDifference / divisor;
		
		printf("%f\n", averageDifference);
		
		if(averageDifference < -25)
			printf("Left!");
		else if(averageDifference > 25)
			printf("Right!");
		else
			printf("Square!");

		// close the file
		fclose(fp);
		fclose(fp2);
	}else
		printf("error opening file");
	
	return;
	
}

// turns a colour pixel into b/w
unsigned char ApplyThreshold(unsigned char pixel[3]){
	
	// average the pixel colours
	unsigned int temp = ((unsigned int)pixel[0] + (unsigned int)pixel[1] + (unsigned int)pixel[2]) / 3;
	if(temp > HALFVALUE){
		temp = MAXVALUE;
	}else{
		temp = 0;
	}
	
	return (unsigned char)temp;
}
