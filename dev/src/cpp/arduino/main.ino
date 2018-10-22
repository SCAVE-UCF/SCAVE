//Arduino main function 
#include <Servo.h>
#include <limits.h>

#define NEUTRAL 1600

Servo motor;
Servo steering;

void setup() 
{
  //init uart interface
  Serial.begin(9600);
  
  pinMode(11, OUTPUT);
  digitalWrite(11, LOW);
  pinMode(5, INPUT);
  pinMode(6, OUTPUT);
  //open port 9 for the motor, 10 for the steering
  motor.attach(9, 1000, 2000);
  steering.attach(10);
  motor.write(NEUTRAL);
  steering.write(90);
  Serial.println("Begin");
  delay(4000);  
}
unsigned dist = 0;
char Byte = 0;


void loop() 
{ 

  Serial.println("begin");
  while(1)
  {
    if(Serial.available()>0)
    {
      //Serial.println(Serial.read());
      Serial.readBytes(&Byte, 1);
      Serial.println((int)Byte);

	  // if byte is possitive means speed, send pwm to the motor
      if((int)Byte >= 0)
      {
       motor.write((10 * Byte) + 1000); 
      }
	  // if byte is negative means angle, send pwm to the steering control
	  else {
       steering.write(-Byte); 
      }
      
      }
  //TODO: add the tachometer part 
  return;
}


