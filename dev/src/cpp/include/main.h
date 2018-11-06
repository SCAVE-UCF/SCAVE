
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
#include <sched.h>
//#include <thread>
//#include <chrono>
#include <list>

#include "serial.h"



using namespace std;

#define THROTTLE_FILE "../../python/teleOp/local/throttle.txt"
#define STEERING_FILE "../../python/teleOp/local/steering.txt"

#define MIN_THROTTLE 1050
#define MAX_THROTTLE 1750
#define MIN_STEERING 35
#define MAX_STEERING 130



void readFiles(int* throttle, int* steering);
