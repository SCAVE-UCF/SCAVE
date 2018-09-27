#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <sys/time.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include "jetsonTX2.h"
#include "tachometer.h"

#define TACHOMETER_SYSFS "/sys/class/gpio/gpio397/value"
//#define TACHOMETER pin7
#define TACHOMETER_IN pin13
static double D_CM = (4.41 * 3.14 * 2.54);

int getkey() {
	int character;
	struct termios orig_term_attr;
	struct termios new_term_attr;

	/* set the terminal to raw mode */
	tcgetattr(fileno(stdin), &orig_term_attr);
	memcpy(&new_term_attr, &orig_term_attr, sizeof(struct termios));
	new_term_attr.c_lflag &= ~(ECHO | ICANON);
	new_term_attr.c_cc[VTIME] = 0;
	new_term_attr.c_cc[VMIN] = 0;
	tcsetattr(fileno(stdin), TCSANOW, &new_term_attr);

	/* read a character from the stdin stream without blocking */
	/*   returns EOF (-1) if no character is available */
	character = fgetc(stdin);

	/* restore the original terminal attributes */
	tcsetattr(fileno(stdin), TCSANOW, &orig_term_attr);

	return character;
}


int main(int argc, char *argv[])
{
//cout << "Please press the button! ESC key quits the program" << endl;
//cout << "Testing the Tachometer" << endl;

 //close the gpio if it's opened, clearing the parameters
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



 //open the file to write
 //FILE* fp = fopen("./files/raw_data.txt", "w");
 FILE* ft = fopen("tach_data.txt","w");

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

 //run until q is pressed
 while(getkey() != 27)
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
   fprintf(ft, "%lf %lf\n", rpm, ms);
   printf("RPM: %lf [%u] / [took %lf s]\n", rpm, counts, ms/1000);
   initTimer(&LOCAL_TIMER);
   counts = 0;
  }
  if(ms > 4000)
  {
   rpm = (((double)counts)/ms)*60000;
   fprintf(ft, "%lf %lf\n", rpm, ms);
   printf("RPM: %lf [%u] / [took %lf s]\n", rpm, counts, ms/1000);
   initTimer(&LOCAL_TIMER);
   counts = 0;
  }
 
 }
 //close all resources
 //fclose(fp);
 fclose(ft);
 gpioUnexport(TACHOMETER_IN);


}

//TO-DO: begin undefining macros...
#undef GETTING_EDGES_OR_TIMING_OUT

