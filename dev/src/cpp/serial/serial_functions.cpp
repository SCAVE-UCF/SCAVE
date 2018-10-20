#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include "jetsonTX2.h"
#include "serial.h"
#include <sched.h>




int open_port(const char* port)
{
	int fd;
	fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY);

	if (fd == -1)
	{
		printf("open_port: Unable to open %s.\n", port);
	}
	else {
		fcntl(fd, F_SETFL, 0);
		printf("port is open.\n");
	}
	return fd;
}

int configure_port(int fd)
{
	struct termios port_settings;
	cfsetispeed(&port_settings, B9600);
	cfsetospeed(&port_settings, B9600);

	port_settings.c_cflag &= ~PARENB;
	port_settings.c_cflag &= ~CSTOPB;
	port_settings.c_cflag &= ~CSIZE;
	port_settings.c_cflag |= CS8;

	tcsetattr(fd, TCSANOW, &port_settings);
	return fd;
}


void readFiles(int* throttle, int* steering)
{
	FILE* t_f = fopen("/home/nvidia/Documents/dev/src/py/teleOp/throttle.txt", "r");
	FILE* s_f = fopen("/home/nvidia/Documents/dev/src/py/teleOp/steering.txt", "r");

	fscanf(t_f, "%d", throttle);
	fscanf(s_f, "%d", steering);
	fclose(t_f);
	fclose(s_f);
}


int serial_read()
{
	int fd = open_port(ACM0);
	fd = configure_port(fd);
	int byte;
	read(fd, &byte, 1);
	close(fd);
	return byte;
}

void* serial_write(int byte)
{
	int fd = open_port(ACM0);
	fd = configure_port(fd);
    write(fd, &byte, 1);
	close(fd);
	return NULL;
}