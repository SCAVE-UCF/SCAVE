# Instructions for serial.cpp: 
- It runs as a separate thread in the main.cpp running every 10 ms and reading from both *throttle.txt* and *steering.txt* and writes to Arduino UNO through Serial bus.
- Also deals with opening and closing of USB port to communicate through serial as it automatically checks where the Arduino is and opens that port.
- Just launch the _serial_write_ and _serial_read_ threads for serial write and read with Arduino.
