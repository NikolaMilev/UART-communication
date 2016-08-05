#include <errno.h>
#include "uartlib.h"

//Time and print were used for measuring the time between the initial response and the end of the message,
//approximating the time needed to receive a message
//#include <stdio.h>
//#include <sys/time.h>

unsigned int BAUD_ = B38400 ;
unsigned int NUM_BITS_ = CS8 ;
char *UART_PATH_ = "/dev/ttyAMA0" ;
unsigned int OPEN_FLAG_ = O_RDWR ;
time_t TIMEOUT_SEC_ = 2 ;
suseconds_t TIMEOUT_USEC_ = 0 ;

// This needs to be finely tuned
suseconds_t TIMEOUT_BYTE_ = 5000;


int open_conf_UART_()
{
	// Variable section
	int indicator;
	int uart_filestream ;
	struct termios options ;

	// Opening the port in a read/write mode
	uart_filestream = open(UART_PATH_, OPEN_FLAG_ | O_NOCTTY | O_NONBLOCK);
	if (uart_filestream < 0)
	{
		// Unable to open the serial port, so produce an error and halt
		return -1;
	}

	// Configuring the options for UART

	// Flushing the file stream (the input and the output area)
	indicator = tcflush(uart_filestream, TCIOFLUSH);
	if(indicator < 0)
	{	
		// Unable to flush
		close(uart_filestream);
		return -1;
	}
	
	// Retrieve the options and modify them. 
	indicator = tcgetattr(uart_filestream, &options);
	if(indicator < 0)
	{	
		// Unable to get the attributes
		close(uart_filestream);
		return -1;
	}

	// Setting the options; still going to see if I am to keep this or the next two lines below the comments
	// options.c_cflag |= BAUD_ | NUM_BITS_ | CLOCAL | CREAD ;
	// options.c_cflag &= ~(HUPCL | CSTOPB | PARENB);

	// options.c_iflag &= ~(INPCK | ISTRIP | IGNBRK | BRKINT | IGNCR | ICRNL | INLCR | IXOFF | IXON | IXANY | IMAXBEL);
	// options.c_oflag &= ~(OPOST | ONLCR | OLCUC | OCRNL | ONOCR | ONLRET | OFILL | FFDLY);
	
	// options.c_lflag &= ~(ICANON | ECHO | ISIG | IEXTEN | NOFLSH | TOSTOP);

	// //I want the uart to wait 1/10 of a second between bytes at most
	// options.c_cc[VTIME] = 10;
	// options.c_cc[VMIN] = 0;

	cfmakeraw(&options);
	options.c_cflag |= BAUD_ | NUM_BITS_ | CREAD;


	// Setting the options for the file stream. 
	indicator = tcsetattr(uart_filestream, TCSANOW, &options);
	if(indicator < 0)
	{	
		// Unable to set the attributes
		close(uart_filestream);
		return -1;
	}
	return uart_filestream;
}

int read_UART_(int uart_filestream, char* dest, int max_len)
{
	// Variable section
	int indicator;
	int buffer_length;
	char *tmp_dest;
	fd_set set;
	struct timeval timeout, init_timeout;

	// struct timeval tval_before, tval_result;

	// Reseting the set and inserting the uart_filestream in it
	FD_ZERO(&set);
	FD_SET(uart_filestream, &set);

	// Setting the time for initial contact
	init_timeout.tv_sec = TIMEOUT_SEC_ ;
	init_timeout.tv_usec = TIMEOUT_USEC_ ;

	// Waiting for the first contact. If this times out, we assume no contact.
	indicator = select(uart_filestream + 1, &set, NULL, NULL, &init_timeout);
	if(indicator < 0)
	{
		return -1;
	}
	else if(indicator == 0)
	{	// Timeout has occurred
		return -2;
	}
	//gettimeofday(&tval_before, NULL);
	
	// This section means that there is something to be read in the file descriptor
	buffer_length = 0 ;
	tmp_dest = dest ;

	// The first select is redundant but it is easier to loop this way.
	while(buffer_length < max_len)
	{
		// select changes the timeval structure so it is reset here
		timeout.tv_sec = 0;
		timeout.tv_usec = TIMEOUT_BYTE_;

		// Reinitialize the sets for reading
		FD_ZERO(&set);
		FD_SET(uart_filestream, &set);

		// Wait for the file descriptor to be available or for timeout
		indicator = select(uart_filestream+1, &set, NULL, NULL, &timeout);

		if(indicator < 0)
		{	
			if(errno == EINTR)
			{
				// Try again
				continue;
			}

			// This indicates an error
			return -1;
		}
		else if(indicator == 0)
		{
			// This indicates a timeout; We assume that the transmission is over once first timeout is reached

		//	gettimeofday(&tval_result, NULL);

		//	printf("Duration of operation: %lu sec, %lu usec\n", tval_result.tv_sec - tval_before.tv_sec, tval_result.tv_usec - tval_before.tv_usec);
			return buffer_length;
		}

		// There's been a select that didn't time out before this read
		indicator = read(uart_filestream, (void*)tmp_dest, max_len - buffer_length);
		if(indicator < 0)
		{
			if(errno == EINTR)
			{
				// If the call was interrupted, try again
				continue;
			}

			// If it was any other condition, the read is corrupt.
			return -1;
		}

		else if(indicator == 0)
		{
			// If, somehow, EOF was reached
			break;
		}

		// Change the necessary values
		buffer_length += indicator ;
		tmp_dest += indicator; 

	}
	// Both branches of the if statement above have return, so this will not be reached
	// but a warning is generated 
	return buffer_length;
}


int write_UART_(int uart_filestream, char *src, unsigned int len)
{
	//Variable section
	int indicator, sel_ind;
	fd_set set;
	struct timeval timeout;

	while(1)
	{
		// Initializing set (for timeout)
		FD_ZERO(&set);
		FD_SET(uart_filestream, &set);

		// For writing, same timeout is being used
		timeout.tv_sec = TIMEOUT_SEC_;
		timeout.tv_usec = TIMEOUT_USEC_;

		// select waits for the uart_filestream to be ready for writing
		sel_ind = select(uart_filestream + 1, NULL, &set, NULL, &timeout);
		if(sel_ind == -1)
		{
			if(errno == EINTR)
			{
				// If the call was interrupted, try again
				continue;
			}

			// select error has occurred
			return -1;
		}
		else if(sel_ind == 0)
		{
			// A timeout occurred, returning -2 as an indicator.
			return -2;
		}
		else
		{
			break;
		}
	}

	// Trying to write to the filestream
	indicator = write(uart_filestream, src, len) ;

	if (indicator < 0)
	{
		// An error occured while writing
		return -1;
	}
	else
	{
		int ret_tmp = indicator;
		// Waiting for the output buffer to be sent 
		indicator = tcdrain(uart_filestream);
		if(indicator < 0)
		{
			return -1;
		}
		else
		{
			return ret_tmp;
		}
	}	
	// Both branches of the if statement above have return, so this will not be reached
}

void flush_buffer_UART_(int uart_filestream)
{
	char c;
	while(read(uart_filestream, &c, 1) > 0);
}