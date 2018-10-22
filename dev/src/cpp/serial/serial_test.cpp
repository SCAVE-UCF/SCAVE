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
#include "tachometer.h"
#include "serial.h"
#include <sched.h>

using namespace std;

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


int main(int argc, char *argv[]) {
	cout << "Please press the button! ESC key quits the program" << endl;
	cout << "Starting serial test" << endl;
	int min_throttle = 1400, max_throttle = 1750;
	int min_steering = 35, max_steering = 130;
	int fd = open_port(ACM1);
	fd = configure_port(fd);
	
	int throttle = 1600, steering = 90;
	write(fd, &steering, 1);
	write(fd, &throttle, 1);
	
	int throttle_temp = 1600, steering_temp = 90;

	while(1)    
	{
		readFiles(&throttle, &steering);
		cout << "Stearing  " << steering <<endl;
		cout << "Throttle  " << throttle <<endl;
			if (steering_temp != steering)
			{
				//if (steering < min_steering) steering = min_steering;
				//else if (steering > max_steering) steering = max_steering;
				int neg_steering = -steering;
				write(fd, &neg_steering, 1);
				 cout << "Stearing  " << steering <<endl;
				steering_temp = steering;
		}
		
		if (throttle_temp != throttle)
		{
			//if (throttle < min_throttle) throttle = min_throttle;
			//else if (throttle > max_throttle) throttle = max_throttle;
			int norm_throttle = (int)((throttle-1000)/10);
			write(fd, &norm_throttle, 1);
			cout << "Throttle  " << throttle <<endl;
			throttle_temp = throttle;
		}
	  
		usleep(1000);
	  
		

	}
		
	close(fd);
  	cout << "Test Serial finished." << endl;

	return 0;
}


