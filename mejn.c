#include <stdio.h>
#include <string.h>
#include "uartlib.h"
#include <sys/time.h>



int main(int argc, char **argv)
{
	char *c;
	int read_num, filedes, written_num;
	char *ch = "long message1long message2long message3long message4long message5long message6long message7long message8long message9long message10";

	struct timeval tval_before, tval_result;

	c = (char*) malloc(MAX_SIZE_) ;
	if(c == NULL)
	{
		printf("malloc error");
		return 1;	
	}
	printf("------------------Opening-----------------\n");
	filedes = open_conf_UART_();
	if(filedes < 0)
	{
		printf("open_conf_UART_ error");
		return 1;
	}
	printf("------------------Reading-----------------\n");
	read_num = read_UART_(filedes, c, MAX_SIZE_-1);
	
	if(read_num < 0)
	{
		printf("An unsuccessful read!\n");
		return 0;
	}
	c[read_num] = '\0';
	printf("Accepted %d bytes. Arrived message: %s\n", read_num , c);

	printf("----------------------------------------\n");
	printf("-----------------Writing----------------\n");
	written_num = write_UART_(filedes, ch, strlen(ch)) ;

	if(written_num == -1)
	{
		printf("Writing error!\n");
		return 0;
	}
	else if (written_num == -2)
	{
		printf("Timeout writing!\n");
		return 0;
	}
	printf("Written %d bytes!\n", written_num);


	printf("------------------Reading-----------------\n");
	gettimeofday(&tval_before, NULL);
	read_num = read_UART_(filedes, c, MAX_SIZE_-1);
	gettimeofday(&tval_result, NULL);

	//printf("Duration of operation: %lu sec, %lu usec\n", tval_result.tv_sec - tval_before.tv_sec, tval_result.tv_usec - tval_before.tv_usec);
	if(read_num < 0)
	{
		printf("An unsuccessful read!\n");
		return 0;
	}
	c[read_num] = '\0';
	printf("Accepted %d bytes. Arrived message: %s\n", read_num , c);


	return 0;
}
