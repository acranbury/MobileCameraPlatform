#ifndef	_goertzel_h
#define _goertzel_h

/*************************DEFINES*******************************/
#define WEBCAMDTMF		"arecord -t raw -f S32_LE -d 1 dtmf.raw"
#define SAMPLERATE		8000
#define DURATION		1			// Duration of recording in seconds.
#define DATASTART		0				// Beginning of audio data in file
#define NUMSAMPLES		DURATION * SAMPLERATE 
#define TIMEOUT			10 			// Timeout before returning, in seconds.
#define AUDIOFILE		"dtmf.raw"
#define POWERTHRESHOLD	5000000000000000			// Threshold indicating presence of a signal.

double DetectDTMF(int samples[], double freq, int N);
int WaitonDTMF (char tone);
void GetData (int * data);
char IdentifyDTMF();

#endif
