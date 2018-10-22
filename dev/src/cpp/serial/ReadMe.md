# Serial CPP
Followinng are the functions that can be used to use serial port and their description:

# file serial.cpp, Serial class: 
- It runs as a separate thread in the main.cpp running every 10 ms and reading from both *throttle.txt* and *steering.txt* and writes to Arduino UNO through Serial bus.
- Also deals with opening and closing of USB port to communicate through serial as it automatically checks where the Arduino is and opens that port.
- Just launch the _serial_write_ and _serial_read_ threads for serial write and read with Arduino.

# file serial_functions.cpp includes the following functions

#int open_port(const char* port)
>>Open the port to be used in serial communication , receive the name of the port , ex ACM0

#int configure_port(int fd)
>>Configure the port for the communication, 8 bit, 9600 baud rate 

#int serial_read()
>>Read a data from serial port ACM0, return the value as a byte 

#void* serial_write(int byte)
>>Write a byte to the serial port ACM0, receive the byte to be send

# file serial_test.cpp 
>> Came with it own main function for testing the serial port 
>> To use it : Just build serial_test.cpp and run in the terminal, it will read the data from "/home/nvidia/Documents/dev/src/py/teleOp/throttle.txt" and "/home/nvidia/Documents/dev/src/py/teleOp/steering.txt", do the correspondent conversions and send the bytes to the Arduino.

