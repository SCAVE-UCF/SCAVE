#include "main.h"

#define desyncro(rs)\
 rs->syncronizer = 0
Profile profiler;
double sonarDist(Sonar* sonar);
bool _run_once = false;
unsigned ticks = 0;
bool calibrating = false;
bool show_debug = true;
bool isBraking = false;
bool isFinishedBraking = true;
pthread_mutex_t mutex;
TIMER global_timer;
unsigned _term_speed = TERM_SPEED;

//function definitions
double min(double a, double b);
double max(double a, double b);
void goNeutral();
//void brake(Tachometer* tacho);
void brake(unsigned level, double time);
void hard_brake(Tachometer* tach)
{
   TIMER brake_timer;
 //read in initial speed
   unsigned initial_speed = 1600;
   FILE* fp = fopen(MAIN_THROTTLE_FILE, "r");
   fscanf(fp, "%d", &initial_speed);
   fclose(fp);
   Profile::File(MAIN_THROTTLE_FILE, "hard_brake: MAIN_THROTTLE_FILE");
   Profile::Unsigned(initial_speed, "hard_brake: initial_speed");

   bool moving_forward = (initial_speed > 1600 && _term_speed > 1600) ? true : false;
   Profile::Bool(moving_forward, "hard_brake: moving_forward");
   bool moving_reverse = (initial_speed < 1600 && _term_speed > 1600) ? true : false;
   Profile::Bool(moving_reverse, "hard_brake: moving_reverse");

   //if we're commanding it to go in reverse, we still want to be able to stop,
   //so the previous idiom is reversed:
   if(_term_speed < 1600)
   {
    moving_forward = (initial_speed < 1600) ? true : false;
    moving_reverse = (initial_speed > 1600) ? true : false;
   }

   Profile::Double(tach->avg_rpm, "hard_brake: avg_rpm");
   Profile::Bool(moving_forward, "hard_brake: moving_forward");
   Profile::Bool(moving_reverse, "hard_brake: moving_reverse");

   if(/*tach->avg_rpm == 0 ||*/ (!moving_forward && !moving_reverse))
   {
//    if(readPin(CAPTAIN) == 0)
  //  {
//     goNeutral();
 //   } else {
   //  printf("Arduino is in control. do not hard brake\n");
    //}
    Profile::Double(tach->avg_rpm, "hard_brake: avg_rpm");
    Profile::Bool(moving_forward, "hard_brake: moving_forward");
    Profile::Bool(moving_reverse, "hard_brake: moving_reverse");
    Profile::Double(millis(&global_timer), "hard_brake: global time");
    return;
   } //already parked
   fp = fopen("brake_func.txt", "a");
   initTimer(&brake_timer);
/*   for(unsigned speed = moving_forward ? 1200 : 1800;
      ((moving_forward && speed < 1600) || (moving_reverse && speed > 1600)) && millis(&brake_timer) < 7000;
      speed += (moving_forward ? 50 : -50))*/
   static int trial = 0;
   char buffer[256];
   sprintf(buffer, "dist_test%d.txt", trial);
   Profile::Int(trial, "hard_brake: trial");

   trial++;
   FILE* dist = fopen(buffer, "w");
   fprintf(dist, "%u %lf %lf\n", tach->revolutions, tach->rpm, millis(&global_timer));
//   double prev_tach_val = tach->avg_rpm
   while(millis(&brake_timer) < 2000 &&
         tach->rpm > 300)
   {
    fprintf(dist, "%u %lf %lf\n", tach->revolutions, tach->rpm, millis(&global_timer));
    if(DEBUG_FLAGS & DEBUG_BRAKE)
     printf("Brake timer: %lf milliseconds\n", millis(&brake_timer));
    if(3000 < tach->avg_rpm)
    {
     brake(moving_forward ? 1300 : 1750, 75);
   //  brake(moving_forward ? 1550 : 1650, 75);
    }
    if(1000 < tach->avg_rpm && tach->avg_rpm <= 3000)
    {
     brake(moving_forward ? 1000 : 2000, 75);
    // brake(moving_forward ? 1550 : 1650, 75);
    }
    if(tach->avg_rpm > 0 && tach->avg_rpm <= 1000)
    {
     brake(moving_forward ? 1000 : 2000, 75);
    // brake(moving_forward ? 1550 : 1650, 75);
    }
/*
    if(1000 <= initial_speed && initial_speed < 1400)
    {
     //brake softly
     brake(1700, 75);
     fprintf(fp, "%u %lf\n", 1700, time_ms);
     brake(1650, 75);
     fprintf(fp, "%u %lf\n", 1650, millis(&brake_timer));
    }
    else if(1400 <= initial_speed && initial_speed < 1550)
    {
     //brake hard
     brake(1800, 75);
     fprintf(fp, "%u %lf\n", 1800, time_ms);
     brake(1700, 75);
     fprintf(fp, "%u %lf\n", 1700, millis(&brake_timer));
    }
    else if(1550 <= initial_speed && initial_speed < 1650)
    {
     goNeutral();
     fclose(fp);
     _run_once = false;
     return;
    }
    else if(1650 <= initial_speed && initial_speed < 1700)
    {
     //brake hard
     brake(moving_forward ? 1200 : 1800, 75);
     fprintf(fp, "%u %lf %lf\n", moving_forward ? 1200 : 1800, tach->rpm, time_ms);
     brake(moving_forward ? 1500 : 1700, 75);
     fprintf(fp, "%u %lf %lf\n", moving_forward ? 1500 : 1700, tach->rpm, millis(&brake_timer));
/*    }
    else if(1700 <= initial_speed && initial_speed <= 2000)
    {
     //brake softly
     brake(1350, 75);
     fprintf(fp, "%u %lf\n", 1500, time_ms);
     brake(1550, 75);
     fprintf(fp, "%u %lf\n", 1550, millis(&brake_timer));
    }
    else
    {
     //idk, just go to neutral
     goNeutral();
     fclose(fp);
     _run_once = false;
     return;
    }
*/
   }
   fprintf(dist, "%u %lf %lf\n", tach->revolutions, tach->rpm, millis(&global_timer));
   fclose(dist);
   goNeutral();
   printf("Last odometer reading since brake: %lf\nResetting odometer to 0\n", tach->odometer);
   tach->revolutions = 0;
   tach->odometer = 0;
   fprintf(fp, "1600 0 %lf\n", millis(&brake_timer));
   fclose(fp);
   _run_once = false;
}


int main(int argc, char** argv)
{
 FILE* fp = fopen(MAIN_THROTTLE_FILE, "w");
 fprintf(fp, "1600");
 fclose(fp);

 fp = fopen("test_steering.txt", "w");
 fclose(fp);

 fp = fopen("brake_func.txt", "w");
 fclose(fp);

 fp = fopen(MAIN_STEERING_FILE, "w");
 fprintf(fp, "90");
 fclose(fp);
 initTimer(&global_timer);

 Profile::Bool(_run_once, "global: _run_once");
 Profile::Unsigned(ticks, "global: ticks");
 Profile::Bool(calibrating, "global: calibrating");
 Profile::Bool(show_debug, "global: show_debug");

// initPin(CAPTAIN, CAPTAIN_SYSFS, 0);

 initPin(SIG_SYSTEM, SIG_SYSTEM_SYSFS, 1);
 writePin(SIG_SYSTEM, 1);
 usleep(1000);
 writePin(SIG_SYSTEM, 0);
 usleep(1000);
 writePin(SIG_SYSTEM, 1);
 usleep(1000);


 Serial port;
 Tachometer tacho;
 port.begin(B9600);
 Keyboard* kb = new Keyboard;
#ifdef SONARS_ARE_IMPLEMENTED
/* Sonar sonar[NUM_SONARS] = {
  Sonar(SONAR_ECHO_0, SONAR_ECHO_0_SYSFS, SONAR_TRIG_0, SONAR_TRIG_0_SYSFS),
  Sonar(SONAR_ECHO_1, SONAR_ECHO_1_SYSFS, SONAR_TRIG_1, SONAR_TRIG_1_SYSFS),
  Sonar(SONAR_ECHO_2, SONAR_ECHO_2_SYSFS, SONAR_TRIG_2, SONAR_TRIG_2_SYSFS),
  Sonar(SONAR_ECHO_3, SONAR_ECHO_3_SYSFS, SONAR_TRIG_3, SONAR_TRIG_3_SYSFS)
 };*/
#endif

 Thread* accelerometer = new Thread(Tachometer::read_tach, &tacho);
 accelerometer->launch();

 Thread* writer = new Thread(Serial::write_serial, &port);
 writer->launch();

 Thread* reader = new Thread(Serial::read_serial, &port);
 reader->launch();

 Thread* kbr = new Thread(Keyboard::read_keyboard, kb);
 kbr->launch();


 TIMER brake_timer;
 char c = kb->getKey();
 Profile::Char(c, "main: c");

 TIMER print_sonar_timer;
 TIMER check_brake_timer;
 initTimer(&check_brake_timer);
 initTimer(&print_sonar_timer);

 while(kb && c != 'q')
 {
  int camera_lost_line = 0;
  Profile::Int(camera_lost_line, "main: camera_lost_line");

  unsigned cur_speed;
  Profile::Unsigned(cur_speed, "main: cur_speed");

  unsigned interval = 0;
  Profile::Unsigned(interval, "main: interval");

  fp = fopen(MAIN_THROTTLE_FILE, "r");
  fscanf(fp, "%u", &cur_speed);
  fclose(fp);

  if(cur_speed > 1600 && millis(&check_brake_timer) > 100)
  {
   fp = fopen("../py/teleOp/slacker.txt", "r");
   fscanf(fp, "%d", &camera_lost_line);
   fclose(fp);
  }

  if(camera_lost_line > 0)
  {
   
   fp = fopen("../py/teleOp/slacker.txt", "w");
   fprintf(fp, "0");
   fclose(fp);
   printf("Camera commands hard brake\n");
   if(isFinishedBraking)
    isBraking = false;
   hard_brake(&tacho);
  }

#ifdef SONARS_ARE_IMPLEMENTED
  bool reset_timer = false;
  Profile::Bool(reset_timer, "main: reset_timer");

  for(int i = 0; i < NUM_SONARS; i++)
  {
   if(cur_speed == 1600) break;
   if(cur_speed > 1600 && i > 1) break;;
   if(cur_speed < 1600 && i < 2) continue;
  if(millis(&print_sonar_timer) > 500 && (DEBUG_FLAGS & DEBUG_SONAR))
   printf("\n");
   if(millis(&print_sonar_timer) > 500 && (DEBUG_FLAGS & DEBUG_SONAR))
   {
     TIMER sonar_timer;
     initTimer(&sonar_timer);
     fp = fopen("sonar_timer.txt", "a");

     double dist = sonarDist(&sonar[i]);
     Profile::Double(dist, "main: dist");

      printf("SENSOR %d: %lf\n", i, dist);
     fprintf(fp, "%lf %lf\n", dist, millis(&sonar_timer));
     fclose(fp);
//    usleep(10000);
     reset_timer = true;
   }
  }
  if(reset_timer && (DEBUG_FLAGS & DEBUG_SONAR))
  {
   printf("\n");
   initTimer(&print_sonar_timer);
   reset_timer = false;
  }

  static bool sonars_demand_park = false;
  Profile::Bool(sonars_demand_park, "main: sonars_demand_park");

  if(cur_speed > 1600 && tacho.rpm > 0 && tacho.avg_rpm > 0 && millis(&global_timer) > 3000)
  {
   if((sonar[1].distance < SONAR_STOP_DISTANCE && sonar[1].distance > 1) /*||
      (sonar[0].distance < SONAR_STOP_DISTANCE && sonar[0].distance > 1)*/)
   {
//    printf("Sonars are hard-braking\n");
//    hard_brake(&tacho);
//    fp = fopen(MAIN_THROTTLE_FILE, "w");
  //  fprintf(fp, "1000");
   // fclose(fp);
//    hard_brake(&tacho);
    sonars_demand_park = true;
   } else {
    sonars_demand_park = false;
   }
  }
  else if(cur_speed < 1600)
  {
   if((sonar[2].distance < SONAR_STOP_DISTANCE && sonar[2].distance > 1) ||
      (sonar[3].distance < SONAR_STOP_DISTANCE && sonar[3].distance > 1))
   {
//    hard_brake(&tacho);
   // printf("Sonars are hard-braking\n");
   // fp = fopen(MAIN_THROTTLE_FILE, "w");
   // fprintf(fp, "2000");
   // fclose(fp);
    sonars_demand_park = true;
   } else {
    sonars_demand_park = false;
   }
  }
  else
  {
   if(sonars_demand_park)
   {/*
    sonars_demand_park = false;
    fp = fopen(MAIN_THROTTLE_FILE, "w");
    fprintf(fp, "1600");
    fclose(fp);
    writePin(SIG_SYSTEM, 0);*/
   }
   //stay neutral
  }
#endif

/*  if(cur_speed < 1600 || cur_speed > 2000)
   interval = -1;

  if(1600 < cur_speed && cur_speed < 1700)
   interval = 1;

  if(1700 <= cur_speed && cur_speed < 1850)
   interval = 2;

  if(1850 <= cur_speed && cur_speed <= 2000)
   interval = 3;
*/
  char next_char = kb->getKey();
  Profile::Char(next_char, "main: next_char");

  switch(c)
  {
  case 'l':
   fp = fopen(MAIN_STEERING_FILE, "w");
   fprintf(fp, "60");
   fclose(fp);
   break;
  case 'r':
   fp = fopen(MAIN_STEERING_FILE, "w");
   fprintf(fp, "120");
   fclose(fp);
   break;
  case 'd':
   show_debug = !show_debug;
   printf("Debug info %s\n", show_debug ? "ON" : "OFF");
   break;
  case 't':
   if(show_debug)
    printf("Odom: %lf inches\n", tacho.odometer);
   break;
  case 'f':
   if(show_debug)
    printf("\nFlushing...\n");
   while(!port.received.empty())
   {
    std::string next = port.Next();
    if(next.find("idle") == std::string::npos && show_debug)
    {
     printf("%s\t[%4d arduino msgs left]\n", next.c_str(), port.received.size());
    }
   }
   if(show_debug)
    printf("Finished flushing.\n\n");
   break;
  case 'n':
   isBraking = false;
   fp = fopen(MAIN_THROTTLE_FILE, "w");
   fprintf(fp, "1600");
   fclose(fp);
   break;
  case 'b':
//   isBraking = false;
   hard_brake(&tacho);
   fp = fopen(MAIN_STEERING_FILE, "w");
   fprintf(fp, "90");
   fclose(fp);
   break;
  case 'e':
   fp = fopen(MAIN_THROTTLE_FILE, "w");
   fprintf(fp, "1600");
   fclose(fp);
   writePin(SIG_SYSTEM, 1 - readPin(SIG_SYSTEM));
   break;
  case 'c':
   tacho.revolutions = 0; //calibrate the tachometer to 0 revolutions
   tacho.odometer = 0; //calibrate the odometer to 0 cm/inches
   if(show_debug)
    printf("Calibrating tachometer\n");
   fp = fopen(MAIN_THROTTLE_FILE, "w");
   fprintf(fp, "%d", _term_speed);
   fclose(fp);
   tacho.calibrate();
   break;
  case 'x':
   tacho.revolutions = 0;
   tacho.odometer = 0;
   if(show_debug)
    printf("Calibrating tachometer\n");
   fp = fopen(MAIN_THROTTLE_FILE, "w");
   fprintf(fp, "%d", _term_speed);
   fclose(fp);
   tacho.calibrateToNeutral();
   break;
  case 'p':
   break;
  case 's':
//   ticks = 0;
   fp = fopen(MAIN_THROTTLE_FILE, "w");
   fprintf(fp, "%d", _term_speed);
   fclose(fp);

   {
    unsigned Speed;
    Profile::Unsigned(Speed, "main: Speed");
    fp = fopen(MAIN_THROTTLE_FILE, "r");
    fscanf(fp, "%u", &Speed);
    fclose(fp);
    printf("Speed: %u\n", Speed);
   }
   break;
  case '-':
   if(_term_speed > 1050)
   {
    if(_term_speed == 1650)
    {
     _term_speed = 1550;
    }
    else
     _term_speed -= 50;
   }
   break;
  case '=':
   if(_term_speed < 1950)
   {
    if(_term_speed == 1550)
    {
     _term_speed = 1650;
    }
    else
     _term_speed += 50;
   }
   break;
  case 0:
  case '\n':
  case '\r':
   break;
  case 'q':
  case '0':
   next_char = 'q';
   break;
  default:
   std::string next = port.Next();
   while(next.find("idle") != std::string::npos && !port.received.empty())
   {
    next = port.Next();
   }
   if(next.find("idle") == std::string::npos && show_debug)
    printf("%s\n", next.c_str());
   break;
  }

  c = next_char;
 }
 port.syncronizer = 0;
 tacho.syncronizer = 0;
 kb->syncronizer = 0;
 writer->await();
 reader->cancel();
 accelerometer->await();
 printf("Enter any key to quit...\n");
 kbr->await();
 delete kb;

 printf("Exiting main\n");
 pthread_exit(NULL);
} //main

double min(double a, double b)
{
 return a < b ? a : b;
}
double max(double a, double b)
{
 return a > b ? a : b;
}
void goNeutral()
{
  FILE* fp = fopen(MAIN_THROTTLE_FILE, "w");
  fprintf(fp, "1600");
  fclose(fp);
}
void brake(unsigned level, double ms)
{
 FILE* fp = NULL;

 //signal stop pwm
 fp = fopen(MAIN_THROTTLE_FILE, "w");
 fprintf(fp, "%u", level);
 fclose(fp);
 usleep(ms * 1000);

 //signal idle-break pwm
/* fp = fopen(MAIN_THROTTLE_FILE, "w");
 fprintf(fp, "%d", initial_speed > 1600 ? 1500 : (initial_speed < 1600 ? 1700 : 1600));
 fclose(fp);
 usleep(ms * 500);



/* unsigned initial_speed = 1600;
 FILE* fp = fopen(MAIN_THROTTLE_FILE, "r");
 fscanf(fp, "%u", &initial_speed);
 fclose(fp);

 unsigned speed = initial_speed;
 if(initial_speed > 1650)
 {
  switch(method)
  {
  case 0:
   //go full reverse and slow down
   speed = 1000;
   while(speed < 1600 && *rpm > 0)
   {
    FILE* fp = fopen(MAIN_THROTTLE_FILE, "w");
    fprintf(fp, "%u", speed);
    fclose(fp);
    usleep(HARD_BRAKE_MILLIS * 1000.0 / (1600 - speed));
    speed -= 20;
   }
   break;
  case 1:
   //go some amount of reverse based on initial speed
   speed = 1000
   break;
  default:
   goNeutral();
   return;
  }
 } else if(initial_speed < 1550) {
  switch(method)
  {
  case 0:
   //go full reverse and slow down
   break;
  case 1:
   //go some amount of reverse based on initial speed
   break;
  default:
   goNeutral();
   return;
  }
 } else {
  goNeutral();
 }
 goNeutral();*/
}






/***********************************************/
/***********************************************/
//	System library impl'n
/***********************************************/
/***********************************************/

void nano(const char* file)
{
 char buffer[512];
 sprintf( buffer, "nano %s", file );
 system( buffer );
}
void exec(const char* exe)
{
 char buffer[512];
 sprintf( buffer, "./%s", exe );
 system( buffer );
}

/***********************************************/
/***********************************************/
//	GPIO library impl'n
/***********************************************/
/***********************************************/

void initPin(jetsonGPIO pin, const char* sysfs, int direction)
{
 if(FILE* fp = fopen(sysfs, "r"))
 {
  fclose(fp);
  closePin(pin);
 }
 printf("Initializing pin %d\n", pin);
 gpioExport(pin);
 gpioSetDirection(pin, direction);
}
unsigned readPin(jetsonGPIO pin)
{
 unsigned value;
 gpioGetValue(pin, &value);
 return value;
}
void writePin(jetsonGPIO pin, unsigned value)
{
 gpioSetValue(pin, value);
}
void closePin(jetsonGPIO pin)
{
 gpioUnexport(pin);
}

/***********************************************/
/***********************************************/
//	Timer library impl'n
/***********************************************/
/***********************************************/

//(re)start a timer
void initTimer(TIMER* timer)
{
 gettimeofday(timer, NULL);
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

/***********************************************/
/***********************************************/
//	Thread library impl'n
/***********************************************/
/***********************************************/

Thread::Thread():
 thread(NULL),
 callback(Thread::delay),
 args(NULL) {}

Thread::Thread(void* (*cb)(void*), void* data):
 thread(new pthread_t),
 callback(cb),
 args(data) {}

Thread::~Thread()
{
 if(thread)
  delete thread;
}

Thread& Thread::assign(void* (*cb)(void*), void* data)
{
 if(launched) return *this;

 callback = cb;
 args = data;
 return *this;
}
void Thread::cancel()
{
 if(thread)
  pthread_cancel(*thread);
}
void Thread::launch()
{
 if(launched || !thread) return;

 pthread_create(thread, NULL, callback, args);
 launched = true;
}
void Thread::await()
{
 launch();
 if(thread)
  pthread_join(*thread, NULL);
}
void* Thread::delay(void* args)
{
 if(args)
 {
  //sleep for x usecs
  usleep(*(double*)args);
 } else {
  //sleep for 1 sec
  sleep(1);
 }
 pthread_exit(NULL);
}

/***********************************************/
/***********************************************/
//	Serial library impl'n
/***********************************************/
/***********************************************/

Serial::Serial():
 syncronizer(1) {}
Serial::~Serial()
{
 syncronizer = 0;
 //clear and close the port
 tcsetattr(fd, TCSANOW, &port_settings);
 close(fd);
}
void Serial::begin(speed_t baud)
{
  port = "/dev/ttyUSB0";
  baudrate = baud;

  //read and write mode, no blocking
  fd = open(port.c_str(), O_RDWR|O_NOCTTY|O_NDELAY);

  //error occurred
  int next_acm = 0;
  Profile::Int(next_acm, "serial ctor: next_acm");
  char buffer[256];
  while(fd == -1)
  {
   sprintf(buffer, "/dev/ttyACM%d", next_acm);
   printf("Unable to open port %s, trying port %s\n", port.c_str(), buffer);
   fd = open(buffer, O_RDWR|O_NOCTTY|O_NDELAY);
   next_acm++;
   if(next_acm == 0)
   {
    printf("Unable to open any port\n");
    pthread_exit(NULL);
   }
   port = buffer;
  }

  //prepare the port to change its settings
  fcntl(fd, F_SETFL, 0);
  printf("Port is open on %s\n", port.c_str());


  //set baud rates
  cfsetispeed(&port_settings, baudrate);
  cfsetospeed(&port_settings, baudrate);
  //set UART settings 8N1
  port_settings.c_cflag &= ~PARENB;
  port_settings.c_cflag &= ~CSTOPB;
  port_settings.c_cflag &= ~CSIZE;
  port_settings.c_cflag |= CS8;

  //disable Canonical mode
  port_settings.c_cflag &= ~CRTSCTS;
  port_settings.c_cflag |= CREAD|CLOCAL;
  port_settings.c_iflag &= ~(IXON|IXOFF|IXANY);
  port_settings.c_lflag &= ~(ICANON|ECHO|ECHOE|ISIG);
  port_settings.c_oflag &= ~OPOST;

  //blocking mode
  port_settings.c_cc[VMIN] = 0;
  port_settings.c_cc[VTIME] = 0;

  //set the settings and flush the port
  tcsetattr(fd, TCSANOW, &port_settings);
  tcsetattr(fd, TCSAFLUSH, &port_settings);

   
}
void Serial::Write(char c)
{
 Profile::Char(c, "Serial::Write");
// if(readPin(CAPTAIN))
 //{
 // return;
// }
 write(fd, &c, 1);
}
std::string Serial::Read()
{
  std::string current = "";
  while(1)
  {
   char c = 0;
   int bytes = read(fd, &c, 1);

   //it's possible for read not to actually read a byte
   if(bytes > 0)
   {
    if(c == '\n' || c == '\r')
    {
     //add the current string to the queue of strings
     if(!current.empty())
      received.push_back(current);
     break;
    }
    current += c;
   }
  }

  //return the current string and leave the queue untouched
  return current;
}
std::string Serial::Next()
{
 std::string output = "";
 while(!received.empty() && output == "")
 {
  output = received.front();
  received.pop_front();

  if((int)output[0] == 13 || (int)output[1] == 10)
  {
   output = "";
  }
 }
 return output;
}
void* Serial::read_serial(void* args)
{
 printf("Launching arduino read thread\n");
 Serial* This = (Serial*)args;

 bool idle = false;
 bool last_char_whitespace = false;
 TIMER read_serial_timer;
 initTimer(&read_serial_timer);
 while(This && This->syncronizer)
 {
  std::string str = This->Read();
  if(str.size() < 1 || str[0] == '\r' || str[0] == '\n' || str.find("idle") != std::string::npos) continue;
  if(show_debug && (DEBUG_FLAGS & DEBUG_ARDUINO))
   printf("\'%s\' read from arduino\n", str.c_str());
   FILE* fp = fopen("arduino_log.txt", "a");
   fprintf(fp, "%s %lf\n", str.c_str(), millis(&read_serial_timer));
   fclose(fp);
   initTimer(&read_serial_timer);
 }

 printf("Exiting arduino read thread\n");
 pthread_exit(NULL);
}
void Serial::clear()
{
 this->Serial::~Serial();
}
void* Serial::write_serial(void* args)
{
 printf("Launching arduino write thread\n");
 Serial* port = (Serial*)args;

 static int last_throttle = 0, last_steering = 0;
 while(port && port->syncronizer)
 {
  Profile::Int(last_throttle, "Serial::write_serial: last_throttle");
  Profile::Int(last_steering, "Serial::write_serial: last_steering");
  int a;
  char b;
  FILE* fp = fopen(MAIN_THROTTLE_FILE, "r");
  fscanf(fp, "%d", &a);
  fclose(fp);
  if(last_throttle != a)
  {
   b = (char)((a - 1000)/10);
   port->Write(b);
   last_throttle = a;
  }

  fp = fopen(MAIN_STEERING_FILE, "r");
  fscanf(fp, "%d", &a);
  fclose(fp);
  if(last_steering != a)
  {
   FILE* _fp_ = fopen("test_steering.txt", "a");
   b = -(char)a;
   fprintf(_fp_, "%d %d %lf\n", a, (int)b, millis(&global_timer));
   fclose(_fp_);
   if(a > 120)
   {
    for(unsigned i = 0; i <20; i++)
    {
     printf("ERROR! Read %d as an angle!\n", a);
    }
    continue;
    //for some reason, we would get erronously large values (1700)
    //thus causing the car to floor it...
//    continue;
   }
  // if((int)b >= 0)
  // {
  //  printf("Steering is out of bounds! ignoring\n");
  // continue;
  // }
   port->Write(b);
   last_steering = a;
  }
  usleep(100000);
 }

 char exit_t = 60;
 port->Write(exit_t);
 printf("Exiting arduino write thread\n");
 pthread_exit(NULL);
}

/***********************************************/
/***********************************************/
//	Keyboard input library
/***********************************************/
/***********************************************/

Keyboard::Keyboard(): syncronizer(1) {}
bool Keyboard::empty()
{
 return keys.empty();
}
char Keyboard::getKey()
{
 if(empty()) return 0;
 char top = keys.front();
 keys.pop_front();
 return top;
}
/*std::string Keyboard::getStr()
{
 static TIMER keyTimer;
 static bool calibrate_timer = true;
 if(calibrate_timer)
 {
  calibrate_timer = false;
  initTimer(&keyTimer);
 }

 std::string output = "";
 if(keys.empty())
 {
 } else {
  printf("%d\n", keys.size());
 }
 for(char c : keys)
 {
  output += c;
 }
 return output;
}*/
void* Keyboard::read_keyboard(void* args)
{
 printf("Launching keyboard thread\n");
 Keyboard* kb = (Keyboard*)args;

 while(kb && kb->syncronizer)
 {
  char c;
  scanf("%c", &c);
  kb->keys.push_back(c);
 }

 printf("Exiting keyboard thread\n");
 pthread_exit(NULL);
}

/***********************************************/
/***********************************************/
//	Tachometer library impl'n
/***********************************************/
/***********************************************/
Tachometer::Tachometer()
{
 static bool initialized = false;
 if(!initialized)
 {
  initPin(TACHOMETER, TACHOMETER_SYSFS);
  initialized = true;
 }
 revolutions = 0;
 odometer = 0;
 rpm = 0;
 avg_rpm = 0;
 calibrating = false;
 calibrated = false;
 syncronizer = 1;

 Profile::Unsigned(revolutions, "Tachometer: revolutions");
 Profile::Double(odometer, "Tachometer: odometer");
 Profile::Double(rpm, "Tachometer: rpm");
 Profile::Double(avg_rpm, "Tachometer: avg_rpm");
 Profile::Bool(calibrating, "Tachometer: calibrating");
 Profile::Bool(calibrated, "Tachometer: calibrated");
 Profile::Int(syncronizer, "Tachometer: thread starter");
 printf("Tachometer has been created\n");
}
Tachometer::~Tachometer()
{
 closePin(TACHOMETER);
 syncronizer = 0;
}
void Tachometer::calibrate()
{
 calibrated = false;
 calibrating = true;
 while(revolutions < 10);
 calibrating = false;
 calibrated = true;
 if(isFinishedBraking)
  isBraking = false;
 hard_brake(this);
}
void Tachometer::calibrateToNeutral()
{
 calibrated = false;
 calibrating = true;
 while(revolutions < 10);
 calibrating = false;
 calibrated = true;
 goNeutral();
}
void* Tachometer::read_tach(void* args)
{
 //extract the tach from the args
 printf("Launching tachometer thread\n");
 Tachometer* tach = (Tachometer*)args;

 //file for storing edges after calibration
 FILE* edge_f = fopen("tachometer_edges.txt", "w");
 fprintf(edge_f, "Edge [0 or 1]; time since last edge [ms]; global timer [ms]\n");
 printf("beginning tachometer\n");
 //file for storing edges before calibration
 FILE* edge_f_precalib = fopen("precalib_tachometer_edges.txt", "w");
 fprintf(edge_f_precalib, "Edge [0 or 1]; time since last edge [ms]; global timer [ms]\n");
printf("beginning tachometer\n");
 //file for debugging tach after calibration
 FILE* fp = fopen("tachometer_debug.txt", "w");
 fprintf(fp, "Revolution [rev]; RPM [rpm]; AVG_RPM [rpm]; ODOMETER [in]; global timer [ms]\n");
printf("beginning tachometer\n");
 //file for debugging tach before calibration
 FILE* fp_precalib = fopen("precalib_tachometer_debug.txt", "w");
 fprintf(fp_precalib,"Revolution [rev]; RPM [rpm]; AVG_RPM [rpm]; ODOMETER [in]; global timer [ms]\n");
printf("beginning tachometer\n");
 //timers for reading tachometer
 TIMER odom_timer, edge_timer, timeout;
 initTimer(&odom_timer);
 initTimer(&edge_timer);
 initTimer(&timeout);

 //used for rpms
 unsigned counts = 0;

 //used for detecting edges
 unsigned values[2] = {0, 0};

 //while running
 while(tach->syncronizer)
 {
  //select the appropriate edge file
  FILE* edge_of = tach->calibrated ? edge_f : edge_f_precalib;
  //select the appropriate debug file
  FILE* of = tach->calibrated ? fp : fp_precalib;

  //update the tachometer value
  values[1] = values[0];
  values[0] = readPin(TACHOMETER);

  //edge occurred
  if(values[0] != values[1])
  {
   //log the edge
   fprintf(edge_of, "%u %lf %lf\n", values[1], millis(&edge_timer), millis(&global_timer));
   printf("read edge: %u\n", values[0]);
   //full revolution occurred
   if(values[1] == 0)
   {
    tach->odometer += DISTANCE_WHEEL_REVOLUTION / 3.0;
    tach->revolutions++;
    counts++;
    if(show_debug && (DEBUG_FLAGS & DEBUG_TACH))
    {
     printf("\nRevolutions: %u\nDistance: %lf inches\n", tach->revolutions, tach->odometer);
    }
    if(tach->calibrated)
    {
     printf("Tach says car overshot by:\n%lf in [%lu rotations]\n",
      tach->odometer, tach->revolutions);
     fprintf(fp, "%lu %lf %lf %lf %lf\n", tach->revolutions, tach->rpm, tach->avg_rpm, tach->odometer, millis(&global_timer));
    }
   }
   initTimer(&edge_timer);
  }

  double ms = millis(&timeout);
  static unsigned avg_rpm_counter = 0;
  if(counts > 0)
  {
   avg_rpm_counter++;
   tach->rpm = (((double)counts)/ms)*(60000.0/*GEAR_RATIO*/);
   Profile::Double(tach->rpm, "read_tach: rpm");
   Profile::Double(millis(&global_timer), "read_tach: global_time");
   tach->running_avg_rpm += tach->rpm;
   if(avg_rpm_counter % 4 == 0)
   {
    tach->avg_rpm = tach->running_avg_rpm/4;
    Profile::Double(tach->avg_rpm, "read_tach: avg_rpm");
    Profile::Double(millis(&global_timer), "read_tach: global_time");
    tach->running_avg_rpm = 0;
   }
   fprintf(of, "%lu %lf %lf %lf %lf\n", tach->revolutions, tach->rpm, tach->avg_rpm, tach->odometer, millis(&global_timer));
   if(show_debug && (DEBUG_FLAGS & DEBUG_TACH))
   {
    printf("RPM: %lf\nAVG_RPM: %lf\n", tach->rpm, tach->avg_rpm);
   }
   initTimer(&timeout);
   counts = 0;
  }
  if(ms > 2000)
  {
   avg_rpm_counter = 0;
   tach->running_avg_rpm = 0;
   tach->avg_rpm = 0;
   tach->rpm = 0;
   if(show_debug && (DEBUG_FLAGS & DEBUG_TACH))
   {
    printf("RPM: 0\n");
   }
   initTimer(&timeout);
   counts = 0;
  }
 }
 fclose(fp);
 fclose(fp_precalib);
 fclose(edge_f);
 fclose(edge_f_precalib);
 pthread_exit(NULL);
}

/***********************************************/
/***********************************************/
//	Sonar library impl'n
/***********************************************/
/***********************************************/
Sonar::Sonar(jetsonGPIO ec, const char* ec_sysfs, jetsonGPIO tr, const char* tr_sysfs):
 echo(ec),
 echo_sysfs(ec_sysfs),
 trigger(tr),
 trigger_sysfs(tr_sysfs)
{
 //set echo for input
 initPin(ec, ec_sysfs, 0);
 //set trigger for output
 initPin(tr, tr_sysfs, 1);
}
Sonar::~Sonar()
{
 closePin(echo);
 closePin(trigger);
}

double sonarDist(Sonar* sonar)
{
 HCSR04 *hcsr04 = new HCSR04(sonar->trigger, sonar->echo);
 unsigned int duration = hcsr04->pingMedian(3);
 sonar->distance = ((double)duration)/58.0;
 return sonar->distance;
}
