#ifndef	_dtmf.h
#define _dtmf.h

#include "tmwtypes.h"

/*************************DEFINES*******************************/
// Record for 1 second, with big endian float audio.
#define WEBCAMDTMF		"arecord -f FLOAT_BE -d 1 dtmf.wav"
#define SAMPLERATE		8000
#define LOWESTDTMF		697
#define DTMFRECORDING	"/home/user/DTMF/Debug/src/dtmf_6.samples"
#define DATASTART		44				// Beginning of audio data in file
#define SAMPLECHUNK		511/*5 * (SAMPLERATE/LOWESTDTMF)	*/// Size of each sample chunk of data.
#define LINE_SIZE		16				// Max size of each line to be read.
#define PRESENCE_THRESHOLD	0.5f		// Threshold of determining presence of a tone.
#define TIMEOUT			10				// 30 second timeout.


int GetDataASCII (FILE * fp, float data [SAMPLECHUNK]);
int GetDataDATA (FILE * fp, float data [SAMPLECHUNK]);
int WaitonDTMF (char tone);
void Convolve (float input[SAMPLECHUNK], float * output, real64_T * filter, int filtersize, int windowsize);
int AnalyzeData (float * result1, float * result2, int size1, int size2);

#endif