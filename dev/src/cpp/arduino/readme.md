# Arduino main
Followinng are the codes that can be used to use the arduino and their description:

# main.ino
>> file to be upload to the Arduino for controlling the vehicle (throttle and steering angle), after upload the jetson just need to send
>> positive number (1 byte) from 0-100, means speed, Arduino convert to 1000 – 2000 (number *10 + 1000) and send pwm to the motor.
>> negative number (1 byte) from -60 to -130, means angle, Arduino convert to 60-130 (-number) and send pwm to the steering control.

