#ifndef UART_LIB_
#define UART_LIB_

#include <stdlib.h>		//Errors, etc
#include <unistd.h>		//Used for UART
#include <fcntl.h>		//Used for UART
#include <termios.h>	//Used for UART
#include <sys/types.h>	//These includes are for timeout
#include <sys/select.h>	//Used for select(), etc

//Some values used by default, left for the user to change if needed

//Used to set up the baud rate
unsigned int BAUD_ ;

//Used to indicate number of bits in one backage 
unsigned int NUM_BITS_  ;

//Path to the UART device
char *UART_PATH_ ;

//Flag for opening the device
unsigned int OPEN_FLAG_ ;

//Timeout for answer from the other side
time_t TIMEOUT_SEC_ ;
suseconds_t TIMEOUT_USEC_ ;

//Time interval between two bursts of data inside the package
suseconds_t TIMEOUT_BYTE_ ;


/*
 * Opens, configures and then returns the file descriptor of the newly opened UART filestream. 
 * Arguments : none
 * Uses : open(), tcgetattr(), tcflush(), tcsetattr() and close().
 * Returns : A file descriptor for the newly opened filestream, -1 otherwise. Any errno that is set 
 * by the above functions is unaltered.
 */
int open_conf_UART_(void) ;

/*
 * Reads a maximum of max_len bytes from the UART port whose file descriptor is uart_filestream into the dest buffer.
 * The dest variable is assumed to be allocated to a size equal or larger than max_len.
 * This function does not append the terminating null byte at the end of the buffer or in any way alter the bytes read from the port.
 * Arguments : uart_filestream - file descriptor of the UART port.
 * dest - the address of the string (char**) where the data should be put;
 * max_len - a maximum number of characters read into dest 
 * Uses : select() and read() 
 * Returns : length of the read buffer, -1 if an error has occurred or -2, if timeout has occurred.
 */
int read_UART_(int uart_filestream, char* dest, int max_len) ;

/*
 * Writes a maximum of len bytes from src buffer to the UART port whose file descirptor is uart_filestream
 * Arguments : uart_filestream - the UART file descriptor; 
 * src - source buffer, contains the bytes we want to send
 * len - a maximum number of bytes that is to be written to the UART port
 * Uses : select(), write(), tcdrain()
 * Returns : length of the read buffer, -1 if an error has occurred or -2, if timeout has occurred.
 */
int write_UART_(int uart_filestream, char *src, unsigned int len) ;

#endif