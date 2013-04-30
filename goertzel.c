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

char IdentifyDTMF (void){
	int found = 0;
	int sample_data [NUMSAMPLES];
	int StartTime;
	char found_tone = 0;

	printf ("waiting...\n");	
	StartTime = (int)time(NULL);		// Timer start
	
	while ((!found) && ((StartTime + TIMEOUT) > (int)(time(NULL)))) { // While a DTMF is not found and the search has not timed out.
		GetData(sample_data);	// Get sample data.
		// Search for DTMF tones.
		//printf ("Power of 637  %f\n", (DetectDTMF(sample_data, 697, NUMSAMPLES) / (double)(100 * NUMSAMPLES / 2)) - POWERTHRESHOLD);
		//printf ("Power of 770  %f\n", (DetectDTMF(sample_data, 770, NUMSAMPLES) / (double)(100 * NUMSAMPLES / 2)) - POWERTHRESHOLD);
		//printf ("Power of 852  %f\n", (DetectDTMF(sample_data, 852, NUMSAMPLES) / (double)(100 * NUMSAMPLES / 2)) - POWERTHRESHOLD);
		//printf ("Power of 941  %f\n", (DetectDTMF(sample_data, 941, NUMSAMPLES) / (double)(100 * NUMSAMPLES / 2)) - POWERTHRESHOLD);
		//printf ("Power of 1209 %f\n", (DetectDTMF(sample_data, 1209, NUMSAMPLES) / (double)(100 * NUMSAMPLES / 2)) - POWERTHRESHOLD);
		//printf ("Power of 1336 %f\n", (DetectDTMF(sample_data, 1336, NUMSAMPLES) / (double)(100 * NUMSAMPLES / 2)) - POWERTHRESHOLD);
		//printf ("Power of 1477 %f\n", (DetectDTMF(sample_data, 1477, NUMSAMPLES) / (double)(100 * NUMSAMPLES / 2)) - POWERTHRESHOLD);
		//printf ("Power of 1633 %f\n", (DetectDTMF(sample_data, 1633, NUMSAMPLES) / (double)(100 * NUMSAMPLES / 2)) - POWERTHRESHOLD);
		if ((DetectDTMF(sample_data, 697, NUMSAMPLES) / (double)(100 * NUMSAMPLES / 2)) > POWERTHRESHOLD){
			printf ("Got 697\n");
			if ((DetectDTMF(sample_data, 1209, NUMSAMPLES) / (double)(100 * NUMSAMPLES / 2)) > POWERTHRESHOLD){
				found = 1;
				found_tone = '1';
				
			}
			else if ((DetectDTMF(sample_data, 1336, NUMSAMPLES) / (double)(100 * NUMSAMPLES / 2)) > POWERTHRESHOLD){
				found = 1;			
				found_tone = '2';
			}
			else if ((DetectDTMF(sample_data, 1477, NUMSAMPLES) / (double)(100 * NUMSAMPLES / 2)) > POWERTHRESHOLD){
				found = 1;					
				found_tone = '3';
			}
			else if ((DetectDTMF(sample_data, 1633, NUMSAMPLES) / (double)(100 * NUMSAMPLES / 2)) > POWERTHRESHOLD){
				found = 1;					
				found_tone = 'A';
			}
		}			
		else if ((DetectDTMF(sample_data, 770, NUMSAMPLES) / (double)(100 * NUMSAMPLES / 2)) > POWERTHRESHOLD){
			printf ("Got 770\n");
			if ((DetectDTMF(sample_data, 1209, NUMSAMPLES) / (double)(100 * NUMSAMPLES / 2)) > POWERTHRESHOLD){
				found = 1;	
				found_tone = '4';
			}
			else if ((DetectDTMF(sample_data, 1336, NUMSAMPLES) / (double)(100 * NUMSAMPLES / 2)) > POWERTHRESHOLD){
				found = 1;		
				found_tone = '5';
			}
			else if ((DetectDTMF(sample_data, 1477, NUMSAMPLES) / (double)(100 * NUMSAMPLES / 2)) > POWERTHRESHOLD){
				found = 1;		
				found_tone = '6';
			}
			else if ((DetectDTMF(sample_data, 1633, NUMSAMPLES) / (double)(100 * NUMSAMPLES / 2)) > POWERTHRESHOLD){
				found = 1;	
				found_tone = 'B';
			}
		}
		else if ((DetectDTMF(sample_data, 852, NUMSAMPLES) / (double)(100 * NUMSAMPLES / 2)) > POWERTHRESHOLD){
			printf ("Got 852\n");
			if ((DetectDTMF(sample_data, 1209, NUMSAMPLES) / (double)(100 * NUMSAMPLES / 2)) > POWERTHRESHOLD){
				found = 1;		
				found_tone = '7';
			}
			else if ((DetectDTMF(sample_data, 1336, NUMSAMPLES) / (double)(100 * NUMSAMPLES / 2)) > POWERTHRESHOLD){
				found = 1;		
				found_tone = '8';
			}
			else if ((DetectDTMF(sample_data, 1477, NUMSAMPLES) / (double)(100 * NUMSAMPLES / 2)) > POWERTHRESHOLD){
				found = 1;		
				found_tone = '9';
			}
			else if ((DetectDTMF(sample_data, 1633, NUMSAMPLES) / (double)(100 * NUMSAMPLES / 2)) > POWERTHRESHOLD){
				found = 1;	
				found_tone = 'C';
			}
		}
		else if ((DetectDTMF(sample_data, 941, NUMSAMPLES) / (double)(100 * NUMSAMPLES / 2)) > POWERTHRESHOLD){
			printf ("Got 941\n");
			if ((DetectDTMF(sample_data, 1209, NUMSAMPLES) / (double)(100 * NUMSAMPLES / 2)) > POWERTHRESHOLD){
				found = 1;	
				found_tone = '*';
			}
			else if ((DetectDTMF(sample_data, 1336, NUMSAMPLES) / (double)(100 * NUMSAMPLES / 2)) > POWERTHRESHOLD){
				found = 1;	
				found_tone = '0';
			}
			else if ((DetectDTMF(sample_data, 1477, NUMSAMPLES) / (double)(100 * NUMSAMPLES / 2)) > POWERTHRESHOLD){
				found = 1;	
				found_tone = '#';
			}
			else if ((DetectDTMF(sample_data, 1633, NUMSAMPLES) / (double)(100 * NUMSAMPLES / 2)) > POWERTHRESHOLD){
				found = 1;	
				found_tone = 'D';
			}
		}
		/*if (found_tone != tone){
			found = 0;
			found_tone = 0;
		}*/
	}	
	
	if ((found) )//&& (found_tone == tone))
	{
		printf ("SUCCESS!!!\n");
		return found_tone;
	}
	else {		
		printf ("FAIL\n");
		return 0;
	}
}

/*
int main(int argc, char **argv){
	printf("Waiting for D\n");
	WaitonDTMF('D');
	printf("Waiting for *\n");
	WaitonDTMF('*');
	printf("Waiting for 1\n");
	WaitonDTMF('1');
	printf("Waiting for 5\n");
	WaitonDTMF('5');
	printf("Waiting for 9\n");
	WaitonDTMF('9');
	printf("Waiting for 0\n");
	WaitonDTMF('0');
	printf("Waiting for 2\n");
	WaitonDTMF('2');
	printf("Waiting for 3\n");
	WaitonDTMF('3');
	printf("Waiting for 4\n");
	WaitonDTMF('4');
	printf("Waiting for 6\n");
	WaitonDTMF('6');
	printf("Waiting for 7\n");
	WaitonDTMF('7');
	printf("Waiting for 8\n");
	WaitonDTMF('8');
	printf("Waiting for A\n");
	WaitonDTMF('A');
	printf("Waiting for B\n");
	WaitonDTMF('B');
	printf("Waiting for C\n");
	WaitonDTMF('C');
	printf("Waiting for #\n");
	WaitonDTMF('#');
	printf ("done\n");
	return(0);
}*/

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
    coeff = 2 * cosf(2 * M_PI * normalizedfreq);
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
	int StartTime;
	char found_tone = 0;

	printf ("waiting...\n");	
	StartTime = (int)time(NULL);		// Timer start
	
	while ((!found) && ((StartTime + TIMEOUT) > (int)(time(NULL)))) { // While a DTMF is not found and the search has not timed out.
		GetData(sample_data);	// Get sample data.
		// Search for DTMF tones.
		//printf ("Power of 637  %f\n", (DetectDTMF(sample_data, 697, NUMSAMPLES) / (double)(100 * NUMSAMPLES / 2)) - POWERTHRESHOLD);
		//printf ("Power of 770  %f\n", (DetectDTMF(sample_data, 770, NUMSAMPLES) / (double)(100 * NUMSAMPLES / 2)) - POWERTHRESHOLD);
		//printf ("Power of 852  %f\n", (DetectDTMF(sample_data, 852, NUMSAMPLES) / (double)(100 * NUMSAMPLES / 2)) - POWERTHRESHOLD);
		//printf ("Power of 941  %f\n", (DetectDTMF(sample_data, 941, NUMSAMPLES) / (double)(100 * NUMSAMPLES / 2)) - POWERTHRESHOLD);
		//printf ("Power of 1209 %f\n", (DetectDTMF(sample_data, 1209, NUMSAMPLES) / (double)(100 * NUMSAMPLES / 2)) - POWERTHRESHOLD);
		//printf ("Power of 1336 %f\n", (DetectDTMF(sample_data, 1336, NUMSAMPLES) / (double)(100 * NUMSAMPLES / 2)) - POWERTHRESHOLD);
		//printf ("Power of 1477 %f\n", (DetectDTMF(sample_data, 1477, NUMSAMPLES) / (double)(100 * NUMSAMPLES / 2)) - POWERTHRESHOLD);
		//printf ("Power of 1633 %f\n", (DetectDTMF(sample_data, 1633, NUMSAMPLES) / (double)(100 * NUMSAMPLES / 2)) - POWERTHRESHOLD);
		if ((DetectDTMF(sample_data, 697, NUMSAMPLES) / (double)(100 * NUMSAMPLES / 2)) > POWERTHRESHOLD){
			printf ("Got 697\n");
			if ((DetectDTMF(sample_data, 1209, NUMSAMPLES) / (double)(100 * NUMSAMPLES / 2)) > POWERTHRESHOLD){
				found = 1;
				found_tone = '1';
				
			}
			else if ((DetectDTMF(sample_data, 1336, NUMSAMPLES) / (double)(100 * NUMSAMPLES / 2)) > POWERTHRESHOLD){
				found = 1;			
				found_tone = '2';
			}
			else if ((DetectDTMF(sample_data, 1477, NUMSAMPLES) / (double)(100 * NUMSAMPLES / 2)) > POWERTHRESHOLD){
				found = 1;					
				found_tone = '3';
			}
			else if ((DetectDTMF(sample_data, 1633, NUMSAMPLES) / (double)(100 * NUMSAMPLES / 2)) > POWERTHRESHOLD){
				found = 1;					
				found_tone = 'A';
			}
		}			
		else if ((DetectDTMF(sample_data, 770, NUMSAMPLES) / (double)(100 * NUMSAMPLES / 2)) > POWERTHRESHOLD){
			printf ("Got 770\n");
			if ((DetectDTMF(sample_data, 1209, NUMSAMPLES) / (double)(100 * NUMSAMPLES / 2)) > POWERTHRESHOLD){
				found = 1;	
				found_tone = '4';
			}
			else if ((DetectDTMF(sample_data, 1336, NUMSAMPLES) / (double)(100 * NUMSAMPLES / 2)) > POWERTHRESHOLD){
				found = 1;		
				found_tone = '5';
			}
			else if ((DetectDTMF(sample_data, 1477, NUMSAMPLES) / (double)(100 * NUMSAMPLES / 2)) > POWERTHRESHOLD){
				found = 1;		
				found_tone = '6';
			}
			else if ((DetectDTMF(sample_data, 1633, NUMSAMPLES) / (double)(100 * NUMSAMPLES / 2)) > POWERTHRESHOLD){
				found = 1;	
				found_tone = 'B';
			}
		}
		else if ((DetectDTMF(sample_data, 852, NUMSAMPLES) / (double)(100 * NUMSAMPLES / 2)) > POWERTHRESHOLD){
			printf ("Got 852\n");
			if ((DetectDTMF(sample_data, 1209, NUMSAMPLES) / (double)(100 * NUMSAMPLES / 2)) > POWERTHRESHOLD){
				found = 1;		
				found_tone = '7';
			}
			else if ((DetectDTMF(sample_data, 1336, NUMSAMPLES) / (double)(100 * NUMSAMPLES / 2)) > POWERTHRESHOLD){
				found = 1;		
				found_tone = '8';
			}
			else if ((DetectDTMF(sample_data, 1477, NUMSAMPLES) / (double)(100 * NUMSAMPLES / 2)) > POWERTHRESHOLD){
				found = 1;		
				found_tone = '9';
			}
			else if ((DetectDTMF(sample_data, 1633, NUMSAMPLES) / (double)(100 * NUMSAMPLES / 2)) > POWERTHRESHOLD){
				found = 1;	
				found_tone = 'C';
			}
		}
		else if ((DetectDTMF(sample_data, 941, NUMSAMPLES) / (double)(100 * NUMSAMPLES / 2)) > POWERTHRESHOLD){
			printf ("Got 941\n");
			if ((DetectDTMF(sample_data, 1209, NUMSAMPLES) / (double)(100 * NUMSAMPLES / 2)) > POWERTHRESHOLD){
				found = 1;	
				found_tone = '*';
			}
			else if ((DetectDTMF(sample_data, 1336, NUMSAMPLES) / (double)(100 * NUMSAMPLES / 2)) > POWERTHRESHOLD){
				found = 1;	
				found_tone = '0';
			}
			else if ((DetectDTMF(sample_data, 1477, NUMSAMPLES) / (double)(100 * NUMSAMPLES / 2)) > POWERTHRESHOLD){
				found = 1;	
				found_tone = '#';
			}
			else if ((DetectDTMF(sample_data, 1633, NUMSAMPLES) / (double)(100 * NUMSAMPLES / 2)) > POWERTHRESHOLD){
				found = 1;	
				found_tone = 'D';
			}
		}
		if (found_tone != tone){
			found = 0;
			found_tone = 0;
		}
	}	
	
	if ((found) && (found_tone == tone)){
		printf ("SUCCESS!!!\n");
		return 1;
	}
	else {		
		printf ("FAIL\n");
		return 0;
	}
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
