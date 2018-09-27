#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <sys/time.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <pthread.h>
#include <vector>
#include "jetsonTX2.h"

#define TIMER struct timeval
TIMER begin, end;
TIMER GLOBAL_TIMER, LOCAL_TIMER, TIMEOUT[10];

//(re)start a timer
void initTimer(TIMER* timer)
{
 gettimeofday(timer, NULL);
}

//get a new timer pointer
TIMER* newTimer()
{
	//create a new timer
	TIMER* output = new TIMER;
	//initialize it
	initTimer(output);
	//return it
	return output;
}

//get the elapsed milliseconds since the timer was (re)started
double millis(TIMER* timer)
{
	//get the timer for duration
	TIMER* end = newTimer();

	//calculate elapsed seconds
	double s = end->tv_sec - timer->tv_sec;
	//calculate elapsed microseconds
	double us = end->tv_usec - timer->tv_usec;
	//convert to milliseconds
	double ms = ((s * 1000) + (us / 1000));

	//free the resources tied to the duration timer
	delete end;
	return ms;
}

void* checkKeyboard(void* args)
{
 int* RUNNING = (int*)args;

 while(*RUNNING)
 {
  char c;
  scanf("%c", &c);
  *RUNNING = c == 'q' || c == '0' ? 0 : 1;
 }

 pthread_exit(NULL);
}

#define TACHOMETER pin13
#define SYS_FS_LOCATION "/sys/class/gpio/gpio397/value"
