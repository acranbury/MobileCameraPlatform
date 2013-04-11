#ifndef _SERIAL_H
#define _SERIAL_H

//********************** Prototypes ****************************
void SerialInit(void);
int SerialOpen(void);
void SerialClose(void);
int SerialWrite(unsigned char * buffer, int numBytes);
int SerialRead(unsigned char * buffer, int numBytes);

#endif
