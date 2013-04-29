/*
 * 	Program: goertzel.c
 * 
 * 	Author: Mark Mahony		April 29, 2013
 */

/*************************INCLUDES******************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include "goertzel.h"

/*	DetectDTMF
 * 
 * 	Input: sample data, desired frequency and number of samples
 * 
 *  Output: Power of the desired frequency in the data.
 * 
 * 	from www.netwerkt.wordpress.com/2011/08/25/goertzel-filter/
*/

double DetectDTMF(int samples[], double freq, int N) {
    double s_prev = 0.0;
    double s_prev2 = 0.0;    
    double coeff,normalizedfreq,power,s;
    int i;
    normalizedfreq = freq / SAMPLERATE;
	printf ("PI: %d Norm_Freq %d\n", sizeof(M_PI), sizeof(normalizedfreq));
    //coeff = 2 * cos((double)(M_PI) * (double)normalizedfreq );
    for (i=0; i<N; i++) {
        s = samples[i] + coeff * s_prev - s_prev2;
        s_prev2 = s_prev;
        s_prev = s;
    }
    power = s_prev2*s_prev2+s_prev*s_prev-coeff*s_prev*s_prev2;
    return power;
}

/*	WaitonDTMF
 * 
 * 	Input: tone to look for.
 * 
 *  Output: Indication of success of failure.
*/
int WaitonDTMF (char tone){
	int found = 0;
	int sample_data [NUMSAMPLES];
	time_t StartTime;

	

	StartTime = time(NULL);
	
	while ((!found) && ((StartTime + ((time_t)TIMEOUT)) < time(NULL))){
		GetData(sample_data);
		printf ("power of 697 HZ: %f\n", DetectDTMF(sample_data, 697, NUMSAMPLES));
		printf ("power of 770 HZ: %f\n", DetectDTMF(sample_data, 770, NUMSAMPLES));
		printf ("power of 852 HZ: %f\n", DetectDTMF(sample_data, 852, NUMSAMPLES));
		printf ("power of 941 HZ: %f\n", DetectDTMF(sample_data, 941, NUMSAMPLES));
		printf ("power of 1209 HZ: %f\n", DetectDTMF(sample_data, 1209, NUMSAMPLES));
		printf ("power of 1336 HZ: %f\n", DetectDTMF(sample_data, 1336, NUMSAMPLES));
		printf ("power of 1336 HZ: %f\n", DetectDTMF(sample_data, 1477, NUMSAMPLES));
		printf ("power of 1633 HZ: %f\n", DetectDTMF(sample_data, 1633, NUMSAMPLES));
	}
	
	if (found)
		return 1;
	else 
		return 0;
}

/*	WaitonDTMF
 * 
 * 	Input: array to fill
*/

void GetData (int * data){
	FILE * fp;
	int count;
	
	system(WEBCAMDTMF);

	if ((fp = fopen (AUDIOFILE, "rb")) == NULL)
	    printf ("ERROR: Could not open audio file\n");
	else {
		fseek(fp, DATASTART, SEEK_SET);		// Seek to beginning of audio data.
		count = fread (data, sizeof(int), NUMSAMPLES, fp);
		printf ("%d bytes were read, %d were supposed to be read\n", count, NUMSAMPLES);
	}	
}