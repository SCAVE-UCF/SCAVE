volatile byte revolutions; //assigning variable types
unsigned int rpm;
unsigned long timeold;
const byte interruptPin = 2;
void setup()
{
	Serial.begin(9600); //baud rate for serial
	pinMode(interruptPin, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(interruptPin), rpm_fun, FALLING); //interrupt triggers on pin 2,goes to rpm_fun when pin 2 goes low																		// or attachInterrupt(0, rpm_fun, FALLING);
	revolutions = 0; //initializing variabels
	rpm = 0;
	timeold = 0;
}
void loop()
{
	// or  if (millis() - lastmillis == 1000){ //Uptade every one second, this will be equal to reading frecuency (Hz).

	if (revolutions > 5) {
		detachInterrupt(interruptPin);//Disable interrupt when calculating
									  //Update RPM every 5 counts, increase this for better RPM resolution,
									  //decrease for faster update
		rpm = 60 * 1000 * revolutions / (millis() - timeold);   // Convert frecuency to RPM, note: this works for one interruption per full rotation
																//calcultaes RPMs using measured RPM changes from 
																//interrupts and time passed(millis function)
																//and time since the last RPM print (timeold)
		timeold = millis();
		//sets timeold to time used during the measurement
		revolutions = 0; //reset revolutions for the next if statement
		Serial.println(rpm, DEC)//print RPM; 
		attachInterrupt(interruptPin, rpm_fun, FALLING); //enable interrupt
	}
}

// this code will be executed every time the interrupt 0 (pin2) gets low.
void rpm_fun()
{
	revolutions++; //increment revolutions on trigger

}

