#include <stdio.h>	// Standard input/output definitions
#include <string.h>	// String function definitions
#include <unistd.h>	// UNIX standard function definitions
#include <fcntl.h>	// File control definitions
#include <errno.h>
#include <termios.h>	// POSIX terminal control definitions

#define SERIALPORT	"/dev/ttyUSB0"

int fd; 		// File descriptor for the port
struct termios options;	// structure of terminal options

// initialize the serial port
void SerialInit(void){
	if(fd > 0){
		printf("Setting serial port %s to 38400 baud, 8N1, no flow control\n", SERIALPORT);
		// get current options for the port
		tcgetattr(fd, &options);
		
		// set baud rates to 9600
		cfsetispeed(&options, B38400);
		cfsetospeed(&options, B38400);
		
		// set 8N1
		options.c_cflag &= ~PARENB;
		options.c_cflag &= ~CSTOPB;
		options.c_cflag &= ~CSIZE;
		options.c_cflag |= CS8;

		// disable hardware flow control
		options.c_cflag &= ~CRTSCTS;

		// disable software flow control
		options.c_cflag &= ~(IXON | IXOFF | IXANY);

		// set up raw input (we don't want it to take out CR's or LF's)
		options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

		// set up raw output
		options.c_oflag &= OPOST;
		
		// enable receiver and set local mode
		options.c_cflag |= (CLOCAL | CREAD);
		
		// set new options for the port 
		//TCSANOW means changes happen now, without waiting for data to complete
		tcsetattr(fd, TCSANOW, &options);
	}
	else
		printf("SerialInit - bad file descriptor\n");
}

// open the serial port
// returns file descriptor
int SerialOpen(void){
	
	// open the port
	// O_RDWR = read/write
	// O_NOCTTY = not controlling
	// O_NDELAY = don't wait for other port to open
	if((fd = open(SERIALPORT, (O_RDWR | O_NOCTTY | O_NDELAY))) == -1){
		// could not open the port
		printf("Could not open serial communications - %s Error number: %d\n", SERIALPORT,errno);
	}
	else{
		fcntl(fd,F_SETFL, 0);
		printf("Opened serial port %s\n", SERIALPORT);
	}

	return(fd);
}

// close the serial port
void SerialClose(void){
	if(close(fd) < 0)
		// error closing the serial port
		printf("Error closing serial port\n");
}

// write to the serial port
// returns the number of bytes written
int SerialWrite(unsigned char * buffer, int numBytes){
	int bytesWritten = 0;
	// sanity check on fd
	if(fd > 0)
		// write the buffer to the serial port
		if((bytesWritten = write(fd, buffer, numBytes)) < 0)
			// didn't write anything to the port
			printf("Serial write of %d bytes failed - Error number: %d\n", numBytes, errno);

	// return number of bytes written
	return(bytesWritten);
}

// read a number of bytes from the serial port
// returns number of bytes read
int SerialRead(unsigned char * buffer, int numBytes){
	int bytesRead = 0;
	// sanity check on fd
	if(fd > 0)
		if((bytesRead = read(fd, buffer, numBytes)) < 0)
			// read error
			printf("Serial read of %d bytes failed\n", numBytes);
	
	// return number of bytes read
	return(bytesRead);
}
