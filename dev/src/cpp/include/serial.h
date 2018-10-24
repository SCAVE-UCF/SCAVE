#ifndef __SERIAL_H__
#define __SERIAL_H__

//#include "main.h"
#include "threads.h"
//path to possible arduinos on the jetson
#ifdef HOME_TEST
#define ACM0 "/dev/tty.usbmodemFD121"
#define ACM1 "/dev/tty.usbmodemFD121"
#endif
#ifndef HOME_TEST
#define ACM0 "/dev/ttyACM0"
#define ACM1 "/dev/ttyACM1"
#define ACM2 "/dev/ttyACM2"
#endif


class serial
{
public:
 int fd; //file descriptor
 int running[2];
 thread *threads[2];
 speed_t baudrate;
 std::string port;
 std::list< std::string > received_data;
 termios ps; //port settings

 serial();
 ~serial();

 void begin();
 void _write(char c);
 char _read();
 std::string next();
 void clearReadBuffer();

 static void* _read_serial(void* args);
 static void* _write_serial(void* args);
};

#endif
