#ifndef	_goertzel_h
#define _goertzel_h

/*************************DEFINES*******************************/
#define WEBCAMDTMF		"arecord -f S32_LE -d 1 dtmf.wav"
#define SAMPLERATE		44100
#define DURATION		1			// Duration of recording in seconds.
#define DATASTART		44				// Beginning of audio data in file
#define NUMSAMPLES		DURATION * SAMPLERATE 
#define TIMEOUT			6 			// Timeout before returning, in seconds.
#define AUDIOFILE		"dtmf.wav"
#define POWERTHRESHOLD	10			// Threshold indicating presence of a signal.

double DetectDTMF(int samples[], double freq, int N);
int WaitonDTMF (char tone);
void GetData (int * data);

#endif