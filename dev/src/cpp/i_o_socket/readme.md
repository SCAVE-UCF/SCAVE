To access the GPIOs of the JetsonTX2 include the given header file **jetsonTX2.h**  and **jetsonTX2.c** file to your program folder. It can manipulate certain pins which are availabe through "sysfs". [Read more here](https://www.kernel.org/doc/Documentation/gpio/sysfs.txt)

Followinng are the functions that can be used to access/manipulate GPIOs and their description:



#gpioExport (gpioPin)
>>In order to access any pin call this func and pass Phyical GPIO number.

#gpioUnexport (gpioPin)
>>Pass the Phyical GPIO number and the pin will become idle.

#gpioSetDirection (gpioPin, inputPin/outputPin)
>>Takes the pin number and makes it i/p or o/p.

#gpioSetValue (gpioPin, high)
>>If pin is o/p then sends either 1 or 0 to it.

#gpioGetValue (gpioPin, &value)
>>If pin is i/p then reads tha values and saves it in *value*.

#gpioSetEdge (gpioPin, &edge)
>>Senses "rising",  "falling", "both" or "none" edges from an i/p pin and stores in edge.

#gpioOpen (gpioPin)
>>Returns the fileDescriptor for a pin

#gpioClose (fileDescriptor)
>>Closes the given fileDescriptor

#gpioActiveLow (gpioPin, value)
>>Checks if the pin is low or not. returns 1 if the pin is active_low.
