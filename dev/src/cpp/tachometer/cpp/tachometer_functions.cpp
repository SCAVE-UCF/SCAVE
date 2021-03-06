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
#define TACHOMETER pin13
#define SYS_FS_LOCATION "/sys/class/gpio/gpio397/value"

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

//Read 1 tachometer  value , the code run until 5 counts or 4 seconds , 1 read and return the rpm 
double read_tachometer()
{
if(FILE* pin_exists = fopen(TACHOMETER_SYSFS, "r"))
 {
  fclose(pin_exists);
  gpioUnexport(TACHOMETER_IN);
 }

 //the values of the tachometer, edges[0] = current, edges[1] = last
 unsigned edges[2];

 //reopen the tachometer and set to INPUT
 printf("Initializing tachometer. . .\n");
 gpioExport(TACHOMETER_IN);
 gpioSetDirection(TACHOMETER_IN, 0);
 gpioGetValue(TACHOMETER_IN, &edges[0]);
 

 TIMER d_timer;
 initTimer(&d_timer);

 //keep track of counts when the magnet is sensed
 unsigned counts = 0;
 double rpm = 0;
 double dist = 0;
 double total_dist = 0;
 //start the global timer (for edge values)
 initTimer(&GLOBAL_TIMER);
 //start the local timer (for tachometer values)
 initTimer(&LOCAL_TIMER);

 //run until 5 counts or 4 seconds , 1 read and return the rpm 
 while(ms < 4000)
 {
  //get the last edge
  edges[1] = edges[0];

 //get the current edge
  gpioGetValue(TACHOMETER_IN, &edges[0]);

 //if the edge changed, or timeout, print/record the value
  if(edges[0] != edges[1])
  {
   //fprintf(fp, "%u %lf\n", edges[1], millis(&GLOBAL_TIMER));
   if(edges[1] == 0)
   {
    counts++;
   }
   initTimer(&GLOBAL_TIMER);
  }
  
  double ms = millis(&LOCAL_TIMER);
  if(counts >= 5)
  {
   rpm = (((double)counts)/ms)*60000;
   printf("RPM: %lf [%u] / [took %lf s]\n", rpm, counts, ms/1000);
   //close all resources
   gpioUnexport(TACHOMETER_IN);
   return rpm;

  }

 
 }

rpm = (((double)counts) / ms) * 60000;
fprintf(ft, "%lf %lf\n", rpm, ms);
printf("RPM: %lf [%u] / [took %lf s]\n", rpm, counts, ms / 1000);
initTimer(&LOCAL_TIMER);
counts = 0;
 
 //close all resources
 gpioUnexport(TACHOMETER_IN);

 return rpm;

}

// Run in a continuos mode and write the rmp data in "tach_data.txt"

void run_tachometer()
{
	if (FILE* pin_exists = fopen(TACHOMETER_SYSFS, "r"))
	{
		fclose(pin_exists);
		gpioUnexport(TACHOMETER_IN);
	}

	//the values of the tachometer, edges[0] = current, edges[1] = last
	unsigned edges[2];

	//reopen the tachometer and set to INPUT
	printf("Initializing tachometer. . .\n");
	gpioExport(TACHOMETER_IN);
	gpioSetDirection(TACHOMETER_IN, 0);
	gpioGetValue(TACHOMETER_IN, &edges[0]);


	//open the file to write
	FILE* ft = fopen("tach_data.txt", "w");

	TIMER d_timer;
	initTimer(&d_timer);

	//keep track of counts when the magnet is sensed
	unsigned counts = 0;
	double rpm = 0;
	double dist = 0;
	double total_dist = 0;
	//start the global timer (for edge values)
	initTimer(&GLOBAL_TIMER);
	//start the local timer (for tachometer values)
	initTimer(&LOCAL_TIMER);

	//run 
	while (1)
	{
		//get the last edge
		edges[1] = edges[0];

		//get the current edge
		gpioGetValue(TACHOMETER_IN, &edges[0]);

		//if the edge changed, or timeout, print/record the value
		if (edges[0] != edges[1])
		{
			//fprintf(fp, "%u %lf\n", edges[1], millis(&GLOBAL_TIMER));
			if (edges[1] == 0)
			{
				counts++;
			}
			initTimer(&GLOBAL_TIMER);
		}

		double ms = millis(&LOCAL_TIMER);
		if (counts >= 5)
		{
			rpm = (((double)counts) / ms) * 60000;
			fprintf(ft, "%lf %lf\n", rpm, ms);
			printf("RPM: %lf [%u] / [took %lf s]\n", rpm, counts, ms / 1000);
			initTimer(&LOCAL_TIMER);
			counts = 0;
		}
		if (ms > 4000)
		{
			rpm = (((double)counts) / ms) * 60000;
			fprintf(ft, "%lf %lf\n", rpm, ms);
			printf("RPM: %lf [%u] / [took %lf s]\n", rpm, counts, ms / 1000);
			initTimer(&LOCAL_TIMER);
			counts = 0;
		}
		usleep(1000);
	}
	//close all resources
	//fclose(fp);
	fclose(ft);
	gpioUnexport(TACHOMETER_IN);

}