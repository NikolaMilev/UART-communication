#ifndef UART_LIB_
#define UART_LIB_

#include <stdlib.h>		//Errors, etc
#include <unistd.h>		//Used for UART
#include <fcntl.h>		//Used for UART
#include <termios.h>	//Used for UART

#include <sys/types.h>	//These includes are for timeout
#include <sys/stat.h>	
#include <fcntl.h>
#include <sys/select.h>	//
#include <sys/ioctl.h>

#define BITS_PER_PACKAGE_ 11
#define WAIT_PROLONGATION_CONSTANT_ 1.1f

//Some values used by default, left for the user to change if needed
unsigned int BAUD_ ;
unsigned int NUM_BITS_  ;
char *UART_PATH_ ;
unsigned int MAX_SIZE_ ;
unsigned int OPEN_FLAG_ ;
time_t TIMEOUT_SEC_ ;
suseconds_t TIMEOUT_USEC_ ;
struct timeval WAIT_CONSTANT_ ;

int open_conf_UART_() ;
int read_UART_(int uart_filestream, char* dest, int max_len) ;
int write_UART_(int uart_filestream, char *src, unsigned int len) ;

#endif