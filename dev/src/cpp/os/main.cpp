
#include "../include/main.h"

int main(int argc, char *argv[]) {
	cout << "Starting main..." << endl;
		
	//TODO Initialize serial
	int fd = open_port(ACM1);
	fd = configure_port(fd);
	
	int throttle = 1600, steering = 90;
	int throttle_prev = 1600, steering_prev = 90; //store last value to chack for change
	int wait = 0; //delay for writing unchanged values
	
	//TODO
	write(fd, &steering, 1);
	write(fd, &throttle, 1);
	
	

	while(true){

		//values read from throttle.txt and steering.txt
		readFiles(&throttle, &steering);
		cout << "Steering  " << steering <<endl;
		cout << "Throttle  " << throttle <<endl;

		//check if steering value has changed or has been 1/2 second since last write
		if ((steering != steering_prev) || wait>50){
                    
                                //reset delay
                                    wait = 0;
				
				if (steering < MIN_STEERING) steering = MIN_STEERING;
				else if (steering > MAX_STEERING) steering = MAX_STEERING;
				int neg_steering = -steering;

				//TODO test serial write
				write(fd, &neg_steering, 1);

				cout << "Stearing  " << steering <<endl;
				steering_prev = steering;
		}
		
		//check if throttle value has changed or has been 1/2 second since last write
		if ((throttle != throttle_prev) ||wait>50){
			
			//TODO make algorithm for when to send more reliable
			
			//reset delay
			wait = 0;

			if (throttle < MIN_THROTTLE) throttle = MIN_THROTTLE;
			else if (throttle > MAX_THROTTLE) throttle = MAX_THROTTLE;
			int norm_throttle = (int)((throttle-1000)/10);
			
			//TODO test serial write
			write(fd, &norm_throttle, 1);
			
			cout << "Throttle  " << throttle <<endl;
			throttle_prev = throttle;
		}
		else{
			// increment delay
			wait++;
		}
	  
		//sleep for 10ms
		//std::this_thread::sleep_for(std::chrono::milliseconds(10));
		
                usleep(10000);
	}
	
	//TODO
	close(fd);

  	cout << "Closing main" << endl;

	return 0;
}

/***********************************************************************
readfiles:
	Paramaters: pointer to steering and throttle 
	Reads from steering.txt and throttle.txt - updates values in throttle
	and steering
************************************************************************/
void readFiles(int* throttle, int* steering)
{
	FILE* t_f = fopen(THROTTLE_FILE, "r");
	FILE* s_f = fopen(STEERING_FILE, "r");
	if (t_f==NULL){
		cout<<"Error opening "<<THROTTLE_FILE<<endl;

	}
	else if (s_f==NULL){
		cout<<"Error opening "<<THROTTLE_FILE<<endl;
	}
	else{
		fscanf(t_f, "%d", throttle);
		fscanf(s_f, "%d", steering);
		fclose(t_f);
		fclose(s_f);

	}

}
