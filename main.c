#include <stdio.h>
#include <string.h>
#include "uartlib.h"
#include <sys/time.h>

unsigned int MAX_BUFFER_SIZE_ = 257 ;

int main(int argc, char **argv)
{
	char *c;
	int i,read_num, filedes, written_num;
	char ch[MAX_BUFFER_SIZE_]; 
	struct timeval tval_before, tval_result;

	for(i=0; i<256; i++)
	{
		ch[i]=i;
	}

	c = (char*) malloc(MAX_BUFFER_SIZE_) ;
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
	read_num = read_UART_(filedes, c, MAX_BUFFER_SIZE_-1);
	
	if(read_num < 0)
	{
		printf("An unsuccessful read!\n");
		return 0;
	}
	c[read_num] = '\0';
	printf("Accepted %d bytes. Arrived message: %s\n", read_num , c);

	printf("----------------------------------------\n");
	printf("-----------------Writing----------------\n");
	written_num = write_UART_(filedes, ch, 256) ;

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
	read_num = read_UART_(filedes, c, MAX_BUFFER_SIZE_-1);
	gettimeofday(&tval_result, NULL);

	//printf("Duration of operation: %lu sec, %lu usec\n", tval_result.tv_sec - tval_before.tv_sec, tval_result.tv_usec - tval_before.tv_usec);
	if(read_num < 0)
	{
		printf("An unsuccessful read!\n");
		return 0;
	}
	c[read_num] = '\0';
	printf("Accepted %d bytes. Arrived message: %s\n", read_num , c);
	for(i=0; i<256; i++)
	{
		if(ch[i] != c[i])
		{
			printf("ch and c differ on position %d\n", i);
		}
	}


	return 0;
}
