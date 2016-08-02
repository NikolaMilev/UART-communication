#include <stdio.h>
#include <string.h>
#include "uartlib.h"



int main(int argc, char **argv)
{
	char *c;
	int read_num, filedes, written_num;
	char *ch = "cao, picka!";

	time_for_one_byte_();

	struct timeval tval_before, tval_after, tval_result;

 	gettimeofday(&tval_before, NULL);

	c = (char*) malloc(MAX_SIZE_) ;
	if(c == NULL)
	{
		printf("malloc error");
		return 1;	
	}

	filedes = open_conf_UART_();
	if(filedes < 0)
	{
		printf("open_conf_UART_ error");
		return 1;
	}

	read_num = read_UART_(filedes, &c, MAX_SIZE_-1);
	
	if(read_num < 0)
	{
		printf("An unsuccessful read!\n");
		return 0;
	}
	c[read_num] = '\0';
	printf("Accepted %d bytes. Arrived message: %s\n", read_num , c);

	printf("-----------------------\n");
	printf("--------Writing--------\n");
	written_num = write_UART_(filedes, ch, strlen(ch));
	if(written_num > 0)
	{
		printf("--- Sent %d bytes! ---", written_num);
	}
	else
	{
		printf("Error writing!");
	}

	printf("--------Reading--------\n");
	read_num = read_UART_(filedes, &c, MAX_SIZE_ - 1);

	if(read_num > 0)
	{
		c[read_num] = '\0';
		printf("Accepted %d bytes. Arrived message: %s\n", read_num, c);
	}


	return 0;
}
