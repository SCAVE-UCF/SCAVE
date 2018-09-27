// exampleApp.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <sys/time.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include "jetsonTX2.h"
#include "tachometer.h"

using namespace std;

int getkey() {
    int character;
    struct termios orig_term_attr;
    struct termios new_term_attr;

    /* set the terminal to raw mode */
    tcgetattr(fileno(stdin), &orig_term_attr);
    memcpy(&new_term_attr, &orig_term_attr, sizeof(struct termios));
    new_term_attr.c_lflag &= ~(ECHO|ICANON);
    new_term_attr.c_cc[VTIME] = 0;
    new_term_attr.c_cc[VMIN] = 0;
    tcsetattr(fileno(stdin), TCSANOW, &new_term_attr);

    /* read a character from the stdin stream without blocking */
    /*   returns EOF (-1) if no character is available */
    character = fgetc(stdin);

    /* restore the original terminal attributes */
    tcsetattr(fileno(stdin), TCSANOW, &orig_term_attr);

    return character;
}

int main(int argc, char *argv[]){
	cout << "Please press the button! ESC key quits the program" << endl;
    cout << "Syms the tachometer" << endl;


	TX2pin tachometer_syms = pin11;     // Ouput
    // Make the button and led available in user space
    gpioExport(tachometer_syms) ;
    gpioSetDirection(tachometer_syms,outputPin) ;
    // Reverse the button wiring; this is for when the button is wired
    // with a pull up resistor
    // gpioActiveLow(pushButton, true);


    cout<< "Test pin tachometer_syms" <<endl;
    for(int i=0; i<2; i++){
        cout << "Setting the pin on" << endl;
        gpioSetValue(tachometer_syms, on);
        usleep(2000000);         // on for 2000ms
        cout << "Setting the pin off" << endl;
        gpioSetValue(tachometer_syms, off);
        usleep(2000000);         // off for 2000ms
    }

    // Wait for the push button to be pressed
    

    unsigned int value = low;
    int ledValue = low ;
    // Turn off the LED
	int rpm = 30;
    gpioSetValue(tachometer_syms,on) ;
	unsigned int t_1second = 1000000; //second 
	unsigned int  t1min = 60 * t_1second;
	unsigned int t_hight = ((int)(t1min / rpm * 0.9));
	unsigned int t_low= ((int)(t1min / rpm * 0.1));
	cout << "Desired rpm=  "<< rpm << "thigh=" << t_hight << "tlow=" << t_low << endl;
    while(getkey() != 27) {
		
       if(getkey() == 115) //s
		{
         cout<<"New rmp" <<endl;
			int ini=0;
			string mystr;
			while (mystr !="ok")
			{
			//cin>>ini;
			//cout<< ini;
				
				cout << "write ok ";
				getline (cin, mystr);
				cout << "you said " << mystr << endl;
			}

			  cout << "Please enter an integer value: " << endl;
			  cin >> rpm;
		
			t_hight = ((int)(t1min / rpm * 0.9));
			t_low= ((int)(t1min / rpm * 0.1));
			cout << "Desired rpm=  "<< rpm << "thigh=" << t_hight << "tlow=" << t_low << endl;

        }
			cout << "hight" << endl;
			gpioSetValue(tachometer_syms, on);
			usleep(t_hight);         // on for 2000ms
			cout << "low" << endl;
			gpioSetValue(tachometer_syms, off);
			usleep(t_low);         // off for 2000ms
		
    }

    cout << "Syms the tachometer finished." << endl;
    gpioUnexport(tachometer_syms);     // unexport the LED
    return 0;
}


