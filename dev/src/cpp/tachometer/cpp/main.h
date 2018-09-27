#ifndef __MAIN_H__
#define __MAIN_H__
/**************************************************************/
/**************************************************************/
//	Headers, type & variable defines
/**************************************************************/
/**************************************************************/

//#define HOME_TEST

//STD C headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>

//UNIX C headers
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <sched.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>

//CUSTOM Cxx headers
#include "jetsonTX2.h"
#include "ultrasonic_library.h"

//STD C++ headers
#include <vector>
#include <list>
#include <fstream>
#include <string>
#include <iostream>

//path to possible arduinos on the jetson
#ifdef HOME_TEST
#define ACM0 "/dev/tty.usbmodemFD121"
#define ACM1 "/dev/tty.usbmodemFD121"
#endif
#ifndef HOME_TEST
#define ACM0 "/dev/ttyACM0"
#define ACM1 "/dev/ttyACM1"
#endif

#define DISTANCE_WHEEL_REVOLUTION (4.25 * 3.14)
#define DISTANCE_WHEEL_REVOLUTION_CM (4.25 * 3.14 * 2.54)

#ifdef HOME_TEST
#define MAIN_THROTTLE_FILE "testfile1"
#define MAIN_STEERING_FILE "testfile2"
#endif

#ifndef HOME_TEST
//main-function steering and throttle overrides
#define MAIN_THROTTLE_FILE "../py/teleOp/main_throttle.txt"
#define MAIN_STEERING_FILE "../py/teleOp/main_steering.txt"
#endif

#define NUM_SONARS 4

/*
 0x1 = show tach
 0x2 = show sonar
 0x4 = show arduino
*/
#define DEBUG_TACH 0x1
#define DEBUG_SONAR 0x2
#define DEBUG_ARDUINO 0x4
#define DEBUG_BRAKE 0x8
#define DEBUG_FLAGS (DEBUG_TACH | DEBUG_ARDUINO | DEBUG_BRAKE/* | DEBUG_SONAR*/)
//#define DEBUG_ALL 0xFFFFFFFF

#define GEAR_RATIO 3 //of the spur gear : wheel

//signal to tell the arduino to stop (HIGH = stop)
#define SIG_SYSTEM pin33
//location of the gpio on the jetson
#define SIG_SYSTEM_SYSFS "/sys/class/gpio/gpio389/value"

//sonar definitions
//#define SONARS_ARE_IMPLEMENTED //uncomment this when you want to run the sonars on the jetson

#define MAX_SENSOR_DISTANCE 500
#define ROUNDTRIP_CM 58
#define MAX_SENSOR_DELAY 5800
#define NO_ECHO -1

//#define CAPTAIN pin11
//#define CAPTAIN_SYSFS "/sys/class/gpio/gpio466/value"

//#define SONAR_TRIG_0 pin7
#define SONAR_TRIG_0 pin13
#define SONAR_TRIG_1 pin18
//#define SONAR_TRIG_2 pin29
#define SONAR_TRIG_3 pin21

#define SONAR_ECHO_0 pin15
#define SONAR_ECHO_1 pin22
#define SONAR_ECHO_2 pin31
#define SONAR_ECHO_3 pin19

//#define SONAR_TRIG_0_SYSFS "/sys/class/gpio/gpio396/value"
#define SONAR_TRIG_0_SYSFS "/sys/class/gpio/gpio397/value"
#define SONAR_TRIG_1_SYSFS "/sys/class/gpio/gpio481/value"
//#define SONAR_TRIG_2_SYSFS "/sys/class/gpio/gpio398/value"
#define SONAR_TRIG_3_SYSFS "/sys/class/gpio/gpio428/value"

#define SONAR_ECHO_0_SYSFS "/sys/class/gpio/gpio255/value"
#define SONAR_ECHO_1_SYSFS "/sys/class/gpio/gpio254/value"
#define SONAR_ECHO_2_SYSFS "/sys/class/gpio/gpio298/value"
#define SONAR_ECHO_3_SYSFS "/sys/class/gpio/gpio429/value"

#define SONAR_STOP_DISTANCE 60 //cm
//#define SONAR_STOP_DISTANCES[6] = {}

//location of the steering servo file
#define STEERING_FILE "../py/teleOp/steering.txt"


//signal for the tachometer
//#define TACHOMETER pin31
//#define TACHOMETER pin7
#define TACHOMETER pin13
//location of the gpio on the jetson
//#define TACHOMETER_SYSFS "/sys/class/gpio/gpio298"
//#define TACHOMETER_SYSFS "/sys/class/gpio/gpio396/value"
#define TACHOMETER_SYSFS "/sys/class/gpio/gpio397/value"

//location of the throttle file
#define THROTTLE_FILE "../py/teleOp/throttle.txt"

//TIMER is a type that names a clock or timer
#define TIMER struct timeval

extern bool MAIN_PAUSE_SERIAL_WRITE;
extern bool MAIN_IS_TELEOP_RUNNING;

/**************************************************************/
/**************************************************************/
//	System library
/**************************************************************/
/**************************************************************/

void nano(const char* file);
void exec(const char* exe);

/**************************************************************/
/**************************************************************/
//	GPIO library
/**************************************************************/
/**************************************************************/

//allow the pin to be exported and set the direction
//0 is input 1 is output
//unexports the pin if it hasn't been, to clear values
void initPin(jetsonGPIO pin=SIG_SYSTEM, const char* sysfs=SIG_SYSTEM_SYSFS, int direction = 0);
unsigned readPin(jetsonGPIO pin);
void writePin(jetsonGPIO pin, unsigned value=1);
void closePin(jetsonGPIO pin);

/**************************************************************/
/**************************************************************/
//	Timer library
/**************************************************************/
/**************************************************************/

//reset the timer to 0 for the milliseconds function
void initTimer(TIMER* timer);

//return the amount of milliseconds that have elapsed since initTimer was called for the given timer
double millis(TIMER* timer);

//create a new timer and return it
//be sure to call free(_returned_timer_ptr)
//when the resource is finished being used
TIMER* newTimer();

/**************************************************************/
/**************************************************************/
//	Thread library
/**************************************************************/
/**************************************************************/

//allows concurrent multitasking
class Thread
{
private:
 //pointer to a thread object
 pthread_t* thread;

 //the function to be called
 void* (*callback)(void*);

 //the arguments to be passed to the function
 void* args;

 //whether this thread was already launched
 bool launched;

public:
 //empty constructor
 Thread();

 //second constructor allows a function and arguments to be
 //passed. default arguments are null
 //creates a new thread object
 Thread(void* (*cb)(void*), void* data=NULL);

 //destructor removes the thread object
 ~Thread();

 //allow to resassign which function and/or arguments are called
 //only possible if a thread hasn't been launched yet
 Thread& assign(void* (*cb)(void*), void* data=NULL);

 //launch a thread (can only happen once per thread)
 void launch();

 //wait for the thread to finish
 void await();

 void cancel();

 //delay function to be used by any threads
 static void* delay(void* args);
};

/**************************************************************/
/**************************************************************/
//	Serial library
/**************************************************************/
/**************************************************************/

class Serial
{
public:
 //file descriptor for the arduino port
 int fd, syncronizer;

 //9600 baud default
 speed_t baudrate;

 //name of the port to open
 std::string port;

 //store all received data here
 std::list< std::string > received;

 //terminal settings of the port
 termios port_settings;

 //empty constructor
 Serial();

 //close the port when this object is destroyed (or the code is force-quit)
 virtual ~Serial();

 //open and prepare the port for UART serial read/write commands
 void begin(speed_t baud=B9600);

 //write a byte to the arduino
 void Write(char c);

 //read bytes from the arduino until a newline character is reached
 std::string Read();

 //pop the first string from the queue of strings
 //if none exists, return an empty string
 std::string Next();

 void clear();

 static void* read_serial(void* args);
 static void* write_serial(void* args);
};

/**************************************************************/
/**************************************************************/
//	Keyboard input library
/**************************************************************/
/**************************************************************/
class Keyboard
{
public:
 int syncronizer;
 std::list<char> keys;

 Keyboard();

 bool empty();
 std::string getStr();
 char getKey();
 static void* read_keyboard(void* args);
};

/**************************************************************/
/**************************************************************/
//	Tachometer library
/**************************************************************/
/**************************************************************/
//#define DISTANCE_WHEEL_REVOLUTION_CM (4.41 * 3.14 * 2.54)

class Tachometer
{
public:
 double odometer; //total distance
 double rpm; //last rpm reading
 double avg_rpm; //used to get normalized readings from noisy data
 double running_avg_rpm; //used to store intermediate sums for the average
 unsigned revolutions;
 int syncronizer;
 bool calibrating;
 bool calibrated;

 Tachometer();
 ~Tachometer();

 void calibrate();
 void calibrateToNeutral();
 static void* read_tach(void* args);
};
/**************************************************************/
/**************************************************************/
//	Sonar library
/**************************************************************/
/**************************************************************/
class Sonar
{
public:
 int syncronizer;
 double distance;
 jetsonGPIO echo, trigger;
 const char *echo_sysfs, *trigger_sysfs;

 Sonar(jetsonGPIO ec, const char* ec_sysfs, jetsonGPIO tr, const char* tr_sysfs);
 ~Sonar();

 bool triggerPing();
 double ping();
 double pingMedian(int counts);
 double calcmedian(int counts, std::vector<double>& samples);

 static void* read_echo(void* args);
 static void* read_dist(void* args);
};

class Profile
{
public:
 Profile()
 {
  FILE* fp = fopen("profiler.txt", "w");
  fclose(fp);
 }

 static void Int(int val, std::string name)
 {
  static std::vector<std::string> var_names;
  static std::vector<int> values;

  for(unsigned i = 0; i < var_names.size(); i++)
  {
   if(var_names[i] == name)
   {
    if(values[i] == val) return;
    FILE* fp = fopen("profiler.txt", "a");
    fprintf(fp, "[INT] %s: %d -> %d\n", name.c_str(), values[i], val);
    fclose(fp);
    values[i] = val;
    return;
   }
  }
  var_names.push_back(name);
  values.push_back(val);
  FILE* fp = fopen("profiler.txt", "a");
  fprintf(fp, "[INT] %s: %d\n", name.c_str(), val);
  fclose(fp);
 }
 static void Double(double val, std::string name)
 {
  static std::vector<std::string> var_names;
  static std::vector<double> values;

  for(unsigned i = 0; i < var_names.size(); i++)
  {
   if(var_names[i] == name)
   {
    if(values[i] == val) return;
    FILE* fp = fopen("profiler.txt", "a");
    fprintf(fp, "[DOUBLE] %s: %lf -> %lf\n", name.c_str(), values[i], val);
    fclose(fp);
    values[i] = val;
    return;
   }
  }
  var_names.push_back(name);
  values.push_back(val);
  FILE* fp = fopen("profiler.txt", "a");
  fprintf(fp, "[DOUBLE] %s: %lf\n", name.c_str(), val);
  fclose(fp);
 }
 static void Unsigned(unsigned val, std::string name)
 {
  static std::vector<std::string> var_names;
  static std::vector<unsigned> values;

  for(unsigned i = 0; i < var_names.size(); i++)
  {
   if(var_names[i] == name)
   {
    if(values[i] == val) return;
    FILE* fp = fopen("profiler.txt", "a");
    fprintf(fp, "[UNSIGNED] %s: %u -> %u\n", name.c_str(), values[i], val);
    fclose(fp);
    values[i] = val;
    return;
   }
  }
  var_names.push_back(name);
  values.push_back(val);
  FILE* fp = fopen("profiler.txt", "a");
  fprintf(fp, "[UNSIGNED] %s: %u\n", name.c_str(), val);
  fclose(fp);
 }
 static void Char(char val, std::string name)
 {
  static std::vector<std::string> var_names;
  static std::vector<char> values;

  for(unsigned i = 0; i < var_names.size(); i++)
  {
   if(var_names[i] == name)
   {
    if(values[i] == val) return;
    FILE* fp = fopen("profiler.txt", "a");
    fprintf(fp, "[CHAR] %s: %d -> %d\n", name.c_str(), (int)values[i], (int)val);
    fclose(fp);
    values[i] = val;
    return;
   }
  }
  var_names.push_back(name);
  values.push_back(val);
  FILE* fp = fopen("profiler.txt", "a");
  fprintf(fp, "[CHAR] %s: %d\n", name.c_str(), (int)val);
  fclose(fp);
 }
 static void String(std::string val, std::string name)
 {
  static std::vector<std::string> var_names;
  static std::vector<std::string> values;

  for(unsigned i = 0; i < var_names.size(); i++)
  {
   if(var_names[i] == name)
   {
    if(values[i] == val) return;
    FILE* fp = fopen("profiler.txt", "a");
    fprintf(fp, "[STRING] %s: %s -> %s\n", name.c_str(), values[i].c_str(), val.c_str());
    fclose(fp);
    values[i] = val;
    return;
   }
  }
  var_names.push_back(name);
  values.push_back(val);
  FILE* fp = fopen("profiler.txt", "a");
  fprintf(fp, "[STRING] %s: %s\n", name.c_str(), val.c_str());
  fclose(fp);
 }
 static void Bool(bool val, std::string name)
 {
  static std::vector<std::string> var_names;
  static std::vector<bool> values;

  for(unsigned i = 0; i < var_names.size(); i++)
  {
   if(var_names[i] == name)
   {
    if(values[i] == val) return;
    FILE* fp = fopen("profiler.txt", "a");
    fprintf(fp, "[BOOL] %s: %s -> %s\n", name.c_str(), values[i] ? "TRUE" : "FALSE", val ? "TRUE" : "FALSE");
    fclose(fp);
    values[i] = val;
    return;
   }
  }
  var_names.push_back(name);
  values.push_back(val);
  FILE* fp = fopen("profiler.txt", "a");
  fprintf(fp, "[BOOL] %s: %s\n", name.c_str(), val ? "TRUE" : "FALSE");
  fclose(fp);
 }
 static void File(std::string filename, std::string name)
 {
  static std::vector<std::string> var_names;
  static std::vector<std::string> values;
  for(unsigned i = 0; i < var_names.size(); i++)
  {
   if(var_names[i] == name)
   {
    std::string val = "";
    FILE* fp = fopen(filename.c_str(), "r");
    while(!feof(fp))
    {
     char character = '\0';
     fscanf(fp, "%c", &character);
     val += character;
    }
    val += '\n';
    fclose(fp);

    if(values[i] == val) return;
    fp = fopen("profiler.txt", "a");
    fprintf(fp, "[FILE] %s: now reads: \'%s\' [EOF]\n", name.c_str(), val.c_str());
    fclose(fp);
    values[i] = val;
    return;
   }
  }
  var_names.push_back(name);
  std::string val = "";
  FILE* fp = fopen(filename.c_str(), "r");
  while(!feof(fp))
  {
   char character = '\0';
   fscanf(fp, "%c", &character);
   val += character;
  }
  val += '\n';
  fclose(fp);
  values.push_back(val);
  fp = fopen("profiler.txt", "a");
  fprintf(fp, "[FILE] %s: now reads: \'%s\' [EOF]\n", name.c_str(), val.c_str());
  fclose(fp);
 }
};
#endif
