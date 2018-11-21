


#include <sl/Camera.hpp>
#include <sys/stat.h>
#include <sys/types.h>
#define THROTTLE_FILE "/home/nvidia/Desktop/Githubs/SCAVE-Camera/dev/src/python/teleOp/throttle.txt"
#define STEERING_FILE "/home/nvidia/Desktop/Githubs/SCAVE-Camera/dev/src/python/teleOp/steering.txt"
#define TACHOMETER_FILE "/home/nvidia/Desktop/Githubs/SCAVE-Camera/dev/src/cpp/os/tacho.txt"
#define NUM_FRAMES 10


void readFiles(int* throttle, int* steering, int* tachometer);



using namespace sl;

std::string path = "/home/nvidia/Desktop/Images/";


int main(int argc, char **argv) {

    // Create a ZED camera object
    Camera zed;

    // Set configuration parameters
    InitParameters init_params;
    init_params.camera_resolution = RESOLUTION_HD1080; // Use HD1080 video mode
    init_params.camera_fps = 30; // Set fps at 30

    // Open the camera
    ERROR_CODE err = zed.open(init_params);
    if (err != SUCCESS)
        exit(-1);
    // TODO check if exists or make output directory
    /*
    try{
	if (fs::exists(path)){
		std::cout<<"Output path is: "<<path<<std::endl;
	}
	else{
		fs::create_directory(path);
	}
    }
    catch (const fs::filesystem_error& ex){
	std::cout<<ex.what()<<std::endl;
    }
    */


    // Capture NUM_FRAMES frames and stop
    int i = 0;

    //variable to store images in
    sl::Mat image;

    //temp variables for angle, steering etc
    int angle, throttle, rpm; //is tachometer an int??

    while (i < NUM_FRAMES) {
        // Grab an image
        if (zed.grab() == SUCCESS) {
            //get steering angle, accelteration, tachometer
	     readFiles(&throttle, &angle, &rpm);
            // A new image is available if grab() returns SUCCESS
            zed.retrieveImage(image, VIEW_LEFT); // Get the left image
            unsigned long long timestamp = zed.getCameraTimestamp(); // Get the timestamp 
	    
	    String file = std::string(path+"time_"+std::to_string(timestamp)+"_rpm_"+std::to_string(rpm)+"_throttle_"+std::to_string(throttle)+"_angle_"+std::to_string(angle)+"_frame.jpeg").c_str();
	    image.write(file);
            printf("Image resolution: %lu x %lu  || Image timestamp: %llu\n", image.getWidth(), image.getHeight(), timestamp);
            i++;
        }
    }

    // Close the camera
    zed.close();
    return 0;
}


/***********************************************************************
readfiles:
	Paramaters: pointer to steering and throttle and tachometer
	Reads from steering.txt and throttle.txt - updates values in throttle
	and steering
************************************************************************/
void readFiles(int* throttle, int* steering, int* tachometer)
{
	FILE* t_f = fopen(THROTTLE_FILE, "r");
	FILE* s_f = fopen(STEERING_FILE, "r");
	FILE* tac_f = fopen(TACHOMETER_FILE, "r");
	if (t_f==NULL||s_f==NULL||tac_f==NULL){
		std::cout<<"Error opening "<<TACHOMETER_FILE<<std::endl<<STEERING_FILE<<std::endl<<THROTTLE_FILE<<std::endl;
	}
	else{
		fscanf(t_f, "%d", throttle);
		fscanf(s_f, "%d", steering);
		fscanf(tac_f, "%d", tachometer);
		fclose(t_f);
		fclose(s_f);
		fclose(tac_f);

	}

}
