#include <opencv2/opencv.hpp>
//#include <raspicam_cv.h>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <chrono>
#include <ctime>

using namespace std;
using namespace cv;
//using namespace raspicam;

Mat frame, frame1;
VideoCapture cap(0);

//RaspiCam_Cv Camera;


void Capture()
{
    cap >> frame;
    
    if(frame.empty())
    {
        cerr << "ERROR: Unable to grab from the camera" << endl;
        return;
    }
    //Camera.grab();
    //Camera.retrieve(frame);
    cvtColor(frame, frame1, COLOR_BGR2RGB);
    //cvtColor(frame, frame, COLOR_BGR2RGB);

}

int main(int argc, char **argv)
{
    cout << "Connecting to Camera" << endl;
    
	if(!cap.isOpened())
    {
		cerr << "ERROR: Unable to open the camera" <<endl;
		return 0;
	}

    while(1)
    {
        auto start = std::chrono::system_clock::now();
        Capture();

        //namedWindow("BGR", WINDOW_KEEPRATIO);
        //moveWindow("BGR", 50, 100);
        //resizeWindow("BGR", 640, 480);
        //imshow("frame", frame);

        //namedWindow("RGB", WINDOW_KEEPRATIO);
        //moveWindow("RGB", 700, 100);
        //resizeWindow("RGB", 640, 480);
        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end-start;
        float t = elapsed_seconds.count();
        int FPS = 1/t;

        cout << "FPS = " << FPS << endl;
        
        imshow("frame", frame);
        waitKey(1);

    }

    return 0;
}
