#include "../include/serial.h"

serial::serial():
 fd(-1),
 port(DEV_TTY_USBMODEM),
 running{1, 1}
// threads{new thread(serial::_read_serial, this),
  //        new thread(serial::_write_serial, this)}
{
 begin();
 //allow serial port to stabilize
 usleep(10000);
// threads[0]->launch();
// threads[1]->launch();
}



serial::~serial()
{
 //the read thread blocks, thus we want to ignore it when closing to ensure a smooth exit.
 //write, on the other hand, must ensure it writes a sensible ending value to the arduino before closing
 tcsetattr(fd, TCSANOW, &ps);
 if(fd != -1)
  close(fd);
}

void serial::begin()
{
#define NON_CANONICAL (O_RDWR|O_NOCTTY|O_NDELAY)

 for(int i = 0; i < 10 && fd == -1; i++)
 {
  char buffer[5];
  sprintf(buffer, "%d", i);
  port = std::string(DEV_TTY_USBX) + std::string(buffer);
  fd = open(port.c_str(), NON_CANONICAL);
  if(fd == -1)
  {
   printf("Unable to open serial tx/rx on _%s_\n", port.c_str());
   //debugln(std::string("Unable to open serial tx/rx on _") + port + std::string("_"));
  }
 }
 for(int i = 0; i < 10 && fd == -1; i++)
 {
  char buffer[5];
  sprintf(buffer, "%d", i);
  port = std::string(DEV_TTY_ACMX) + std::string(buffer);
  fd = open(port.c_str(), NON_CANONICAL);
  if(fd == -1)
  {
   printf("Unable to open serial tx/rx on _%s_\n", port.c_str());
   //debugln(std::string("Unable to open serial tx/rx on _") + port + std::string("_"));
  }
 }
 if(fd == -1)
 {
  printf("Finally, trying _%s_\n", DEV_TTY_USBMODEM);
  //debugln(std::string("Finally, trying _") + std::string(DEV_TTY_USBMODEM) + std::string("_"));
  port = DEV_TTY_USBMODEM;
  fd = open(port.c_str(), NON_CANONICAL);
  if(fd == -1)
  {
   printf("Fatal error trying to find arduino or PCB!\n");
   //debugln("Fatal error trying to find arduino or PCB!");
   #ifndef JETSON
   usleep(10000);
   return;
   #endif
   pthread_exit(NULL);
  }
 }

 printf("Successfully opened serial on _%s_\n", port.c_str());
 //debugln(std::string("Successfully opened serial on _") + port + std::string("_"));

 fcntl(fd, F_SETFL, 0);
 cfsetispeed(&ps, BAUD_RATE);
 cfsetospeed(&ps, BAUD_RATE);
 ps.c_cflag &= ~PARENB;
 ps.c_cflag &= ~CSTOPB;
 ps.c_cflag &= ~CSIZE;
 ps.c_cflag |= CS8;
 ps.c_cflag &= ~CRTSCTS;
 ps.c_cflag |= CREAD|CLOCAL;
 ps.c_iflag &= ~(IXON|IXOFF|IXANY);
 ps.c_lflag &= ~(ICANON|ECHO|ECHOE|ISIG);
 ps.c_oflag &= ~OPOST;

 ps.c_cc[VMIN]=0;
 ps.c_cc[VTIME]=0;

 tcsetattr(fd, TCSANOW, &ps);
 tcsetattr(fd, TCSAFLUSH, &ps);
}
void serial::_write(char c)
{
 #ifndef JETSON
 usleep(10000);
 return;
 #endif
 write(fd, &c, 1);
}
char serial::_read()
{
 #ifndef JETSON
 usleep(100000);
 return 0;
 #endif
 char c = 0;
// printf("Reading arduino char\n");
 while(read(fd, &c, 1) < 1);
 char buffer[256];
 sprintf(buffer, "_%c_", c);
// debugln(buffer);
// printf("%c", c);
 return c;
}
std::string serial::next()
{
 std::string output = "";

 if(!received_data.empty())
 {
  std::string front = received_data.front();
  received_data.pop_front();
 }

 return output;
}
void* serial::_read_serial(void* args)
{
 printf("Launching arduino read thread...\n");
 //debugln("Launching arduino read thread...");
 serial* arduino = (serial*)args;
 if(!arduino)
 {
  printf("ERROR: arduino not detected! aborting read...\n");
  //debugln("ERROR: arduino not detected! aborting read...");
  pthread_exit(NULL);
 }
// arduino->threads[1]->launch();
 initFile("arduinoReadLog.txt");

 std::string output = "";
 while(arduino && arduino->running[0])
 {
  char input = arduino->_read();
  if(input == '\n')
  {
   printf("Read from arduino: _%s_\n", output.c_str());
   //debugln(std::string("Read from arduino: _") + output + std::string("_"));

   char buffer[1024];
   sprintf(buffer, "[READ]: ___%s___ %lf\n", output.c_str(), millis(&global_timer));
   printf("%s\n", buffer);
//   writeFile("arduinoReadLog.txt", buffer);
//   arduino->received_data.push_back(output);
   output = "";
  } else {
   output += input;
  }
 }

 //practically unreachable, but leave this in for compilability
 printf("Terminating arduino read thread...\n");
 //debugln("Somehow terminating arduino read thread...");
 pthread_exit(NULL);
}







void* serial::_write_serial(void* args)
{
 printf("Launching arduino write thread\n");
 //debugln("Launching arduino write thread");
 serial* arduino = (serial*)args;
 if(!arduino)
 {
  printf("ERROR: arduino not detected! Aborting write...\n");
  //debugln("ERROR: arduino not detected! Aborting write...");
  pthread_exit(NULL);
 }

 initFile("arduinoWriteLog.txt");

 int last_throttle = 0, last_steering = 0;
 while(arduino && arduino->running[1])
 {
  int motor, steering;
  char m_b, s_b;
  char buffer[1024];

  motor = readFile<unsigned>(MAIN_THROTTLE_FILE, "%u", &MAIN_THROTTLE_MUTEX);
  steering = readFile<unsigned>(MAIN_STEERING_FILE, "%u", &MAIN_STEERING_MUTEX);

  if(last_throttle != motor)
  {
   printf("%u\n", readPin(BRAKING_PIN));
   m_b = (char)((motor-1000)/10);
   arduino->_write(m_b);
   last_throttle = motor;

   printf("Writing to arduino: %u [%d]\n", motor, (int)m_b);

  // sprintf(buffer, "[WRITE]: %u [%d], as motor %lf\n", motor, (int)m_b, millis(&global_timer));
//   writeFile("arduinoWriteLog.txt", buffer);
  }

  if(last_steering != steering)
  {
   s_b = -(char)steering;
   if(steering > 120)
   {
//    sprintf(buffer, "[WRITE]: %u [%d], as steering IGNORED %lf\n", steering, (int)s_b, millis(&global_timer));
  //  writeFile("arduinoWriteLog.txt", buffer);
    continue;
   }
   arduino->_write(s_b);
   last_steering = steering;

   printf("Writing to arduino: %u [%d]\n", steering, (int)steering);

//   sprintf(buffer, "[WRITE]: %u [%d], as steering %lf\n", steering, (int)s_b, millis(&global_timer));
//   writeFile("arduinoWriteLog.txt", buffer);
  }
  usleep(10000);
 }

 char exit_byte = 60;
 arduino->_write(exit_byte);
 printf("Terminating arduino write thread...\n");
 pthread_exit(NULL);
}
