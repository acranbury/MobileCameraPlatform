/*
 * 	Program: DTMF.c
 * 
 * 	Author: Mark Mahony		April 25, 2013
 * 
 *  Revision: Add IdentifyDTMF, September 20, 2013. Mark Mahony
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "tmwtypes.h"
#include "7708k.h"
#include "14778k.h"
#include "1209.h"
#include "1336.h"
#include "1633.h"
#include "697.h"
#include "852.h"
#include "941.h"
#include "dtmf.h"
#include "time.h"


/*************************MACROS*******************************/
#define FILTER(toneval)	(tone ## toneval)
#define FILTERSIZE(toneval) (toneL ## toneval)
#define WINDOW_SIZE(filtersize) (SAMPLECHUNK - 1 + (filtersize))

/*
 * 	Purpose: Check audio data for a tone. 
 * 
 * 	Output: Tone found.
 */

char IdentifyDTMF(){
	float sample_data[SAMPLECHUNK]; // Audio sample data
	float * output;		// Output of convolution
	int found = 0;		// Flag determining if  a DTMF tone has been found.
	int tone1 = 0, tone2 = 0; // Second tone.
	int samplecount = 0;	// Number of sample_data(s) that have been analyzed.
	int fWindowsize[NUMTONES] = {WINDOW_SIZE(FILTERSIZE(697)),
								WINDOW_SIZE(FILTERSIZE(770)),
								WINDOW_SIZE(FILTERSIZE(852)),
								WINDOW_SIZE(FILTERSIZE(941)),
								WINDOW_SIZE(FILTERSIZE(1209)),
								WINDOW_SIZE(FILTERSIZE(1336)),
								WINDOW_SIZE(FILTERSIZE(1477)),
								WINDOW_SIZE(FILTERSIZE(1633)),};		
	int fFiltersize[NUMTONES] = {(int)FILTERSIZE(697),
								(int)FILTERSIZE(770),
								(int)FILTERSIZE(852),
								(int)FILTERSIZE(941),
								(int)FILTERSIZE(1209),
								(int)FILTERSIZE(1336),
								(int)FILTERSIZE(1477),
								(int)FILTERSIZE(1633),};		// Number of taps on filter.
	real64_T * fFilter[NUMTONES] = {(real64_T *)(FILTER(697)),
									(real64_T *)(FILTER(770)),
									(real64_T *)(FILTER(852)),
									(real64_T *)(FILTER(941)),
									(real64_T *)(FILTER(1209)),
									(real64_T *)(FILTER(1336)),
									(real64_T *)(FILTER(1477)),
									(real64_T *)(FILTER(1633)),};	// Filter coefficients.
	// Smallest amount of filter taps will make it so that output is large enough
	// for all output files.
	int windowsize = fWindowsize[7];	// size 7 is tone 1633, with 74 taps.
	int filtersize;
	real64_T * filter;	
	FILE * fp = NULL;	// Pointer to audio data file.
	int i;				// loop counter.

	short tones[NUMTONES] = {697, 770, 852, 941, 1209, 1336, 1477, 1633};

	// Allocate space for the output result.
	if (!(output = (float*)malloc(sizeof(float)*windowsize)))	// create array for output values.
	{
		printf ("Memory allocation failed");						// check for errors.
		exit(0);
	}

	// Start polling for tones. 
	while ((!tone2) && (samplecount < TIMEOUT)){ // Keep waiting	
		if (fp != NULL)		// If  the previous data was unsuccessful, close it.
			fclose(fp);
		system(WEBCAMDTMF);
		samplecount++;
		if (!(fp = fopen(DTMFRECORDING, "rb")))		// Open sample data and check if it was opened sucessfully.
			printf ("Error opening file\n");

		tone1 = 0; 
		tone2 = 0;
		
		while ((!tone2) && (GetDataDATA (fp, sample_data) == 0)) {		// Continue 
			// Compare the audio data against each filter.
			for (i = 0; i < NUMTONES; i++){
				// Set filter parameters
				windowsize = fWindowsize[i];
				filter = fFilter[i];
				filtersize = fFiltersize[i];
				// Clear old data
				memset(output, 0, sizeof(float) * fWindowsize[7]);  // Write whole array to zeroes.
				// Do the convolution, applying the filter.
				Convolve(sample_data, output, filter, filtersize, windowsize);
				// Analyze the convolved data for a DTMF tone.
				found = AnalyzeOneTone(output, windowsize);
				if (found != 0){
					tone1 = tones[i];
					found = 0;
				}
				else if (tone1 != 0 && found != 0){
					tone2 = tones[i];
				}
				/*for (i = 0; i < SAMPLECHUNK; i++){
					printf ("%f\n", sample_data[i]); 
				}*/
			}
		}	// until end of file or the tone is found.
	}	// until the tone is found.

	free(output);
	if (fp != NULL){
		fclose (fp);
	}
	
	switch (tone1){
		case 697:
			switch (tone2){
				case 1209:
					return '1';
					break;
				case 1336:
					return '2';
					break;
				case 1477:
					return '3';
					break;
				case 1633:
					return 'A';
					break;
				default:
					return '0';
			}
		break;
		case 770:
			switch (tone2){
				case 1209:
					return '4';
					break;
				case 1336:
					return '5';
					break;
				case 1477:
					return '6';
					break;
				case 1633:
					return 'B';
					break;
				default:
					return '0';
			}
		break;
		case 852:
			switch (tone2){
				case 1209:
					return '7';
					break;
				case 1336:
					return '8';
					break;
				case 1477:
					return '9';
					break;
				case 1633:
					return 'C';
					break;
				default:
					return '0';
			}
		break;
		case 941:
			switch (tone2){
				case 1209:
					return '*';
					break;
				case 1336:
					return '0';
					break;
				case 1477:
					return '#';
					break;
				case 1633:
					return 'D';
					break;
				default:
					return '0';
			}
		break;
		case 1209:
			switch (tone2){
				case 697:
					return '1';
					break;
				case 770:
					return '4';
					break;
				case 852:
					return '7';
					break;
				case 941:
					return '*';
					break;
				default:
					return '0';
			}
		break;	
		case 1336:
			switch (tone2){
				case 697:
					return '2';
					break;
				case 770:
					return '5';
					break;
				case 852:
					return '8';
					break;
				case 941:
					return '0';
					break;
				default:
					return '0';
			}
		break;
		case 1477:
			switch (tone2){
				case 697:
					return '3';
					break;
				case 770:
					return '6';
					break;
				case 852:
					return '9';
					break;
				case 941:
					return '#';
					break;
				default:
					return '0';
			}
		break;
		case 1633:
			switch (tone2){
				case 697:
					return 'A';
					break;
				case 770:
					return 'B';
					break;
				case 852:
					return 'C';
					break;
				case 941:
					return 'D';
					break;
				default:
					return '0';
			}
		break;
	default:
		return '0';
	}
}

/*
 * 	Purpose: Wait until the passed DTMF tone is heard. 
 * 
 * 	Input: Desired tone.
 */
int WaitonDTMF (char tone){
	float sample_data[SAMPLECHUNK];
	float * output1, * output2;
	char found = 0;
	int windowsize1, windowsize2;
	int filter1size, filter2size;
	int samplecount = 0;
	int i;
	real64_T * filter1, * filter2;
	FILE * fp = NULL;
	switch (tone){
		case '0':
			windowsize1 = WINDOW_SIZE(FILTERSIZE(1336));
			windowsize2 = WINDOW_SIZE(FILTERSIZE(941));
			filter1 = (real64_T *)(FILTER(1336));
			filter2 = (real64_T *)(FILTER(941));
		    filter1size = (int)FILTERSIZE(1336);
			filter2size = (int)FILTERSIZE(941);
			break;
		case '1':
			windowsize1 = WINDOW_SIZE(FILTERSIZE(1209));
			windowsize2 = WINDOW_SIZE(FILTERSIZE(697));
			filter1 = (real64_T *)(FILTER(1209));
			filter2 = (real64_T *)(FILTER(697));
		    filter1size = (int)FILTERSIZE(1209);
			filter2size = (int)FILTERSIZE(697);
			break;
		case '2':
			windowsize1 = WINDOW_SIZE(FILTERSIZE(1336));
			windowsize2 = WINDOW_SIZE(FILTERSIZE(697));
			filter1 = (real64_T *)(FILTER(1336));
			filter2 = (real64_T *)(FILTER(697));
		    filter1size = (int)FILTERSIZE(1336);
			filter2size = (int)FILTERSIZE(697);
			break;
		case '3':
			windowsize1 = WINDOW_SIZE(FILTERSIZE(1477));
			windowsize2 = WINDOW_SIZE(FILTERSIZE(697));
			filter1 = (real64_T *)(FILTER(1477));
			filter2 = (real64_T *)(FILTER(697));
		    filter1size = (int)FILTERSIZE(1477);
			filter2size = (int)FILTERSIZE(697);
			break;
		case 'A':
			windowsize1 = WINDOW_SIZE(FILTERSIZE(1633));
			windowsize2 = WINDOW_SIZE(FILTERSIZE(697));
			filter1 = (real64_T *)(FILTER(1633));
			filter2 = (real64_T *)(FILTER(697));
		    filter1size = (int)FILTERSIZE(1633);
			filter2size = (int)FILTERSIZE(697);
			break;
		case '4':
			windowsize1 = WINDOW_SIZE(FILTERSIZE(1209));
			windowsize2 = WINDOW_SIZE(FILTERSIZE(770));
			filter1 = (real64_T *)(FILTER(1209));
			filter2 = (real64_T *)(FILTER(770));
		    filter1size = (int)FILTERSIZE(1209);
			filter2size = (int)FILTERSIZE(770);
			break;
		case '5':
			windowsize1 = WINDOW_SIZE(FILTERSIZE(1336));
			windowsize2 = WINDOW_SIZE(FILTERSIZE(770));			
			filter1 = (real64_T *)(FILTER(1336));
			filter2 = (real64_T *)(FILTER(770));
		    filter1size = (int)FILTERSIZE(1336);
			filter2size = (int)FILTERSIZE(770);
			break;
		case '6':
			windowsize1 = WINDOW_SIZE(FILTERSIZE(1477));
			windowsize2 = WINDOW_SIZE(FILTERSIZE(770));			
			filter1 = (real64_T *)(FILTER(1477));
			filter2 = (real64_T *)(FILTER(770));
		    filter1size = (int)FILTERSIZE(1477);
			filter2size = (int)FILTERSIZE(770);
			break;
		case 'B':
			windowsize1 = WINDOW_SIZE(FILTERSIZE(1633));
			windowsize2 = WINDOW_SIZE(FILTERSIZE(770));		
			filter1 = (real64_T *)(FILTER(1633));
			filter2 = (real64_T *)(FILTER(770));
		    filter1size = (int)FILTERSIZE(1633);
			filter2size = (int)FILTERSIZE(770);
			break;
		case '7':
			windowsize1 = WINDOW_SIZE(FILTERSIZE(1209));
			windowsize2 = WINDOW_SIZE(FILTERSIZE(852));	
			filter1 = (real64_T *)(FILTER(1209));
			filter2 = (real64_T *)(FILTER(852));
		    filter1size = (int)FILTERSIZE(1209);
			filter2size = (int)FILTERSIZE(852);
			break;
		case '8':
			windowsize1 = WINDOW_SIZE(FILTERSIZE(1336));
			windowsize2 = WINDOW_SIZE(FILTERSIZE(852));
			filter1 = (real64_T *)(FILTER(1336));
			filter2 = (real64_T *)(FILTER(852));
		    filter1size = (int)FILTERSIZE(1336);
			filter2size = (int)FILTERSIZE(852);
			break;
		case '9':
			windowsize1 = WINDOW_SIZE(FILTERSIZE(1477));
			windowsize2 = WINDOW_SIZE(FILTERSIZE(852));
			filter1 = (real64_T *)(FILTER(1477));
			filter2 = (real64_T *)(FILTER(852));
		    filter1size = (int)FILTERSIZE(1477);
			filter2size = (int)FILTERSIZE(852);
			break;
		case 'C':
			windowsize1 = WINDOW_SIZE(FILTERSIZE(1633));
			windowsize2 = WINDOW_SIZE(FILTERSIZE(852));
			filter1 = (real64_T *)(FILTER(1633));
			filter2 = (real64_T *)(FILTER(852));
		    filter1size = (int)FILTERSIZE(1633);
			filter2size = (int)FILTERSIZE(852);
			break;
		case '*':
			windowsize1 = WINDOW_SIZE(FILTERSIZE(1209));
			windowsize2 = WINDOW_SIZE(FILTERSIZE(941));
			filter1 = (real64_T *)(FILTER(1209));
			filter2 = (real64_T *)(FILTER(941));
		    filter1size = (int)FILTERSIZE(1209);
			filter2size = (int)FILTERSIZE(941);
			break;
		case '#':
			windowsize1 = WINDOW_SIZE(FILTERSIZE(1477));
			windowsize2 = WINDOW_SIZE(FILTERSIZE(941));
			filter1 = (real64_T *)(FILTER(1477));
			filter2 = (real64_T *)(FILTER(941));
		    filter1size = (int)FILTERSIZE(1477);
			filter2size = (int)FILTERSIZE(941);
			break;
		case 'D':
			windowsize1 = WINDOW_SIZE(FILTERSIZE(1633));
			windowsize2 = WINDOW_SIZE(FILTERSIZE(941));
			filter1 = (real64_T *)(FILTER(1633));
			filter2 = (real64_T *)(FILTER(941));
		    filter1size = (int)FILTERSIZE(1633);
			filter2size = (int)FILTERSIZE(941);
			break;
		default :
			windowsize1 = 0;
			windowsize2 = 0;
			filter1 = NULL;
			filter2 = NULL;
		    filter1size = 0;
			filter2size = 0;
			break;
	}
	// Grab sample data.

	
	// Allocate space for the output result.
	if (!(output1 = (float*)malloc(sizeof(float)*windowsize1)))	// create array for output values.
	{
		printf ("Memory allocation failed");						// check for errors.
		exit(0);
	}

	if (!(output2 = (float*)malloc(sizeof(float)*windowsize2)))	// create array for output values.
	{
		printf ("Memory allocation failed");						// check for errors.
		exit(0);
	}
	
	while ((!found) && (samplecount < TIMEOUT)){ // Keep waiting	
		if (fp != NULL)		// If  the previous data was unsuccessful, close it.
			fclose(fp);
		system(WEBCAMDTMF);
		samplecount++;
		if (!(fp = fopen(DTMFRECORDING, "rb")))		// Open sample data and check if it was opened sucessfully.
			printf ("Error opening file\n");
		//fseek(fp, 0, SEEK_SET);				// Seek to beginning of data (sample).
		//fseek(fp, DATASTART, SEEK_SET);	// Seek to beginning of data (.wav).
		while ((!found) && (GetDataDATA (fp, sample_data) == 0)) {		// Continue 
			//fseek (fp, (long)(sizeof (float)*SAMPLECHUNK), SEEK_CUR);
			memset(output1, 0, sizeof(float) * windowsize1);
			memset(output2, 0, sizeof(float) * windowsize2);
			Convolve(sample_data, output1, filter1, filter1size, windowsize1);
			Convolve(sample_data, output2, filter2, filter2size, windowsize2);
			found = AnalyzeData(output1, output2, windowsize1, windowsize2);
			for (i = 0; i < SAMPLECHUNK; i++){
				printf ("%f\n", sample_data[i]); 
			}
		}	// until end of file or the tone is found.
	}	// until the tone is found.
	if (samplecount >= TIMEOUT)
		return 0;
	else
		return 1;
}

/*****************************GetDataASCII********************************
*	Purpose: Read data from a .sample file to obtain a DTMF tone sample.
*	
*	Input: pointer to input array.
*
*	Output: pointer to the array of floats.
*
*************************************************************************/
int GetDataASCII (FILE * fp, float data [SAMPLECHUNK])
{
	char chardata [LINE_SIZE];						// array to hold char data before conversion to float.
	int j = 0;									// array index for float data.
	
	for	(j = 0; j < SAMPLECHUNK; j++)			// read until all samples filled.
	{
		fgets(chardata, LINE_SIZE, fp);
		if (strcmp("\r\n", chardata))				// Only add a character if it is not a newline.
		{	
			data[j] = atof (chardata);				// convert the current string read to a float number and store it
			if ((j % 10) == 0)
				printf (" %f\n ",data[j]);				// Make sure data is working. 
		}
		else if (feof(fp))
			break;
		else 
			j--;
	}
	if ((fgetc(fp) == EOF))
		return -1;
	else
		return 0;
}

/*****************************GetDataDATA*********************************
*	Purpose: Read data from a .wav file to obtain a DTMF tone sample.
*	
*	Input: pointer to input array.
*
*	Output: pointer to the array of floats.
*
*************************************************************************/
int GetDataDATA (FILE * fp, float data [SAMPLECHUNK])
{									// array index for float data.
	int bytes;
	
	bytes = fread(data, sizeof(float), SAMPLECHUNK, fp);

	if (feof(fp))
		printf ("End of input file was reached while reading! %d floats read", bytes);
	
	if ((fgetc(fp) == EOF))
		return -1;
	else
		return 0;
}

/*****************************Convolve************************************
*
*	Purpose: Convolve the input file with the filter.
* 	
*	Input:	Input stream from stdin, window size, filter choice.
*
*	Output: Filter output.
*	
*************************************************************************/
void Convolve (float input[SAMPLECHUNK], float * output, real64_T * filter, int filtersize, int windowsize){
	int i,j;	
	real64_T filtertaps [filtersize];				// Max filter size.

	//printf ("input [0] %f\n", input[0]);
	//printf ("input [1] %f\n", input[1]);
	
	if (!((filtersize == 0) || (filter == NULL))){
		for (i = 0; i < filtersize; i++)			// copy const filter data.
			filtertaps [i] = filter[i];
		for (i = 0; i < windowsize; i++)		// For each response.
		{
			if (i > 0)
				//printf("%f\n", output[i - 1]);
			for (j = 0; j < SAMPLECHUNK; j++) {
				// Multiply all taps by the input and add.
				output[i] += ((j <  filtersize) ? filtertaps[j] : 0)  * (((i - j) < SAMPLECHUNK) && ((i - j) > 0) ? input[i - j] : 0);
				
			}
		}
	}
}

/*****************************AnalyzeData**********************************
*
*	Purpose: Determine if the two tones needed for the dtmf values are present.
* 	
*	Input:	Result of convolution with FIR filter.
*
*	Output: 0 if dtmf not present, 1 if dtmf present.
*	
*************************************************************************/
int AnalyzeData (float * result1, float * result2, int size1, int size2){
	// Find the average of each result.
	int i;
	float average1 = 0, average2 = 0;

	// Get rid of the starting 10% and ending 10%. (Only use steady state).
	for (i = (size1/10); i < ((size1 * 90)/100); i++){
		average1 += fabs(result1[i]);
		//printf ("%f\n", fabs(result1[i]));
	}
	average1 /= (size1 - (2 * (size1/10)));
	
	for (i = (size2/10); i < ((size2 * 90)/100); i++){
		average2 += fabs(result2[i]);
		//printf ("%f\n", fabs(result2[i]));
	}
	average2 /= (size2 - (2 * (size2/10)));

	printf("Average1: %.10f Average2: %.10f\n", average1, average2);
	if ((average1 > PRESENCE_THRESHOLD) && (average2 > PRESENCE_THRESHOLD))
		return 1;
	else
		return 0;
}

/*****************************AnalyzeOneTone******************************
*
*	Purpose: Determine if the selected tone is present in the provided data.
* 	
*	Input:	Result of convolution with FIR filter.
*
*	Output: 0 if dtmf not present, 1 if dtmf present.
*	
*************************************************************************/
int AnalyzeOneTone (float * result, int size){
	// Find the average of each result.
	int i;
	float average = 0;

	// Get rid of the starting 10% and ending 10%. (Only use steady state).
	for (i = (size/10); i < ((size * 90)/100); i++){
		average += fabs(result[i]);
		//printf ("%f\n", fabs(result1[i]));
	}
	average /= (size - (2 * (size/10)));
	
	

	//printf("Average: %.10f \n", average);
	if (average > PRESENCE_THRESHOLD)
		return 1;
	else
		return 0;
}