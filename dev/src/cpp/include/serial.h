#ifndef __SERIAL_H__
#define __SERIAL_H__

#include "main.h"
#include "threads.h"

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
