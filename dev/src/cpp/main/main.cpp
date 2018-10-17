#include "../include/main.h"


int main(){
	unsigned int status, steering, throttle;
	status = 1;
	
	FILE * status_fp;
	FILE * throttle_fp;
	FILE * steering_fp;
	


	while(status){
		

		//read status file
		status_fp = fopen(MAIN_STATUS_FILE, "r");
		fscanf(status_fp, "%u", &status);

		//read steering file
		steering_fp = fopen(MAIN_STEERING_FILE , "r");
		fscanf(steering_fp, "%u", &steering);

		//read throttle file
		throttle_fp = fopen(MAIN_THROTTLE_FILE , "r");
		fscanf(throttle_fp, "%u", &throttle);


		

		//TODO Serial write to arduino
		std::cout<<"Status: " << status << "  Steering: "<<steering<<" Throtte: "<<throttle<<std::endl;


		if (status ==0){
			printf("Exiting loop\n");
			break;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	}

	fclose(status_fp);
	fclose(steering_fp);
	fclose(throttle_fp);	



	return 0;
}



