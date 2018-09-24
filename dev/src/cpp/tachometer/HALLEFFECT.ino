 volatile byte revolutions; //assigning variable types
 unsigned int rpm;
 unsigned long timeold;
 void setup()
 {
   Serial.begin(9600); //baud rate for serial
   attachInterrupt(digitalPinToInterrupt(2), rpm_fun, LOW); //interrupt triggers on pin 2,goes to rpm_fun when pin 2 goes low
   revolutions = 0; //initializing variabels
   rpm = 0; 
   timeold = 0;
 }
 void loop()
 {
   if (revolutions > 50) { 
     //Update RPM every 50 counts, increase this for better RPM resolution,
     //decrease for faster update
     rpm = 60*revolutions/(millis() - timeold); 
     //calcultaes RPMs using measured RPM changes from 
     //interrupts and time passed(millis function)
     //and time since the last RPM print (timeold)
     timeold = millis(); 
     //sets timeold to time used during the measurement
     revolutions = 0; //reset revolutions for the next if statement
     Serial.println(rpm,DEC)//print RPM; 
   }
 }
 
 void rpm_fun()
 {
   revolutions++ ; //increment revolutions on trigger
   rpm = 60*revolutions/(millis() - timeold);
//   timeold=millis();  This was used for testing the sensor
//   Serial.print("revolutions = "); 
//   Serial.print(revolutions);
//   Serial.println();
//   if (revolutions==30) {  
//   Serial.print("RPMS: ");
//   Serial.print(rpm);
//   Serial.println();
//   }
 }
//-----------------------------------------------
