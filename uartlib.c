#include <errno.h>
#include "uartlib.h"

#include <stdio.h>
#include <sys/time.h>

unsigned int BAUD_ = B38400 ;
unsigned int NUM_BITS_ = CS8 ;
char *UART_PATH_ = "/dev/ttyAMA0" ;
unsigned int MAX_SIZE_ = 256 ;
unsigned int OPEN_FLAG_ = O_RDWR ;
time_t TIMEOUT_SEC_ = 5 ;
suseconds_t TIMEOUT_USEC_ = 0 ;

//This needs to be finely tuned
suseconds_t TIMEOUT_BYTE_ = 5000;


int open_conf_UART_()
{
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

	//Setting the options
	options.c_cflag |= BAUD_ | NUM_BITS_ | CLOCAL | CREAD ;
	options.c_cflag &= ~(HUPCL | CSTOPB | PARENB);

	options.c_iflag &= ~(INPCK | ISTRIP | IGNBRK | BRKINT | IGNCR | ICRNL | INLCR | IXOFF | IXON | IXANY | IMAXBEL);
	options.c_oflag &= ~(OPOST | ONLCR | OLCUC | OCRNL | ONOCR | ONLRET | OFILL | FFDLY);
	
	options.c_lflag &= ~(ICANON | ECHO | ISIG | IEXTEN | NOFLSH | TOSTOP);

	//I want the uart to wait 1/10 of a second between bytes at most
	options.c_cc[VTIME] = 10;
	options.c_cc[VMIN] = 0;



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
	int indicator;
	int buffer_length;
	char *tmp_dest;

	fd_set set;
	struct timeval timeout, init_timeout;

	struct timeval tval_before, tval_result;

	// indicator = tcflush(uart_filestream, TCIFLUSH);
	// if(indicator < 0)
	// {	
	// 	// Unable to flush
	// 	return -1;
	// }

	FD_ZERO(&set);
	FD_SET(uart_filestream, &set);

	timeout.tv_sec = 0;
	timeout.tv_usec = TIMEOUT_BYTE_;

	init_timeout.tv_sec = TIMEOUT_SEC_ ;
	init_timeout.tv_usec = TIMEOUT_USEC_ ;

	indicator = select(uart_filestream + 1, &set, NULL, NULL, &init_timeout);
	if(indicator < 0)
	{
		return -1;
	}
	else if(indicator == 0)
	{	//Timeout
		return -2;
	}
	gettimeofday(&tval_before, NULL);
	
	buffer_length = 0 ;
	tmp_dest = dest ;
	while(buffer_length < max_len)
	{
		indicator = read(uart_filestream, (void*)tmp_dest, max_len - buffer_length);
		if(indicator < 0)
		{
			if(errno == EAGAIN || errno == EINTR)
			{
				continue;
			}

			return -1;
		}
		else if(indicator == 0)
		{
			break;
		}

		buffer_length += indicator ;
		tmp_dest += indicator;

		FD_ZERO(&set);
		FD_SET(uart_filestream, &set);

		indicator = select(uart_filestream+1, &set, NULL, NULL, &timeout);

		if(indicator < 0)
		{
			return -1;
		}
		else if(indicator == 0)
		{

			gettimeofday(&tval_result, NULL);

			printf("Duration of operation: %lu sec, %lu usec\n", tval_result.tv_sec - tval_before.tv_sec, tval_result.tv_usec - tval_before.tv_usec);
			return buffer_length;
		}

		timeout.tv_sec = 0;
		timeout.tv_usec = TIMEOUT_BYTE_;
	}
	return buffer_length;
	// Both branches of the if statement above have return, so this will not be reached
}


int write_UART_(int uart_filestream, char *src, unsigned int len)
{
	//Variable section
	int indicator, sel_ind;
	fd_set set;
	struct timeval timeout;
	////

	// Initializing set (for timeout)
	FD_ZERO(&set);
	FD_SET(uart_filestream, &set);

	timeout.tv_sec = TIMEOUT_SEC_;
	timeout.tv_usec = TIMEOUT_USEC_;

	// indicator = tcflush(uart_filestream, TCOFLUSH);
	// if(indicator < 0)
	// {	
	// 	// Unable to flush
	// 	return -1;
	// }

	// select waits for the uart_filestream to be ready for reading
	sel_ind = select(uart_filestream + 1, NULL, &set, NULL, &timeout);
	if(sel_ind == -1)
	{
		// select error has occurred
		return -1;
	}
	else if(sel_ind == 0)
	{
		// A timeout occurred, returning -2 as an indicator.
		return -2;
	}

	//Trying to write to the filestream
	indicator = write(uart_filestream, src, len) ;

	if (indicator < 0)
	{
		// An error occured while writing
		return -1;
	}
	else
	{
		int ret_tmp = indicator;
		//Waiting for the output buffer to be sent 
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