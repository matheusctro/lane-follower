#include <opencv2/opencv.hpp>
#include <raspicam_cv.h>
#include <iostream>
#include <chrono>
#include <ctime>
#include <wiringPi.h>

using namespace std;
using namespace cv;
using namespace raspicam;


//Image Processing Variables
Mat frame, Matrix, framePers, frameGray, frameThresh, frameEdge, frameFinal, frameFinalDuplicate;
Mat ROILane;
int LeftLanePos, RightLanePos, frameCenter, laneCenter, Result;

stringstream ss;

RaspiCam_Cv Camera;

vector<int> histogramLane;

Point2f Source[] = {Point2f(60,150), Point2f(275,150), Point2f(0,210), Point2f(360,210)};
Point2f Destination[] = {Point2f(60,0), Point2f(300,0), Point2f(60,240), Point2f(300,240)};

//Machine Learning Variables
CascadeClassifier Stop_Cascade;
Mat frame_stop, RoI_Stop, gray_Stop;
vector<Rect> Stop;

void Setup(int argc, char **argv, RaspiCam_Cv &Camera)
{
    Camera.set ( CAP_PROP_FRAME_WIDTH, ( "-w", argc, argv, 360 ) );
    Camera.set ( CAP_PROP_FRAME_HEIGHT, ( "-h", argc, argv, 240 ) );
    Camera.set ( CAP_PROP_BRIGHTNESS, ( "-br", argc, argv, 50 ) );
    Camera.set ( CAP_PROP_CONTRAST, ( "-co", argc, argv, 50 ) );
    Camera.set ( CAP_PROP_SATURATION, ( "-sa", argc, argv, 50 ) );
    Camera.set ( CAP_PROP_GAIN, ( "-g", argc, argv, 50 ) );
    Camera.set ( CAP_PROP_FPS, ( "-fps", argc, argv, 0 ) ); //mudar este valor para conseguir mais frames por segundo
}

void Capture()
{
    Camera.grab();
    Camera.retrieve(frame);
    frame_stop = frame.clone();
    //cvtColor(frame, frame_stop, COLOR_BGR2RGB);
    //cvtColor(frame, frame, COLOR_BGR2RGB);
    

}

void Perspective()
{
    line(frame, Source[0], Source[1], Scalar(0,0,255),2);
    line(frame, Source[1], Source[3], Scalar(0,0,255),2);
    line(frame, Source[3], Source[2], Scalar(0,0,255),2);
    line(frame, Source[2], Source[0], Scalar(0,0,255),2);
    
    //line(frame, Destination[0], Destination[1], Scalar(0,255,0),2);
    //line(frame, Destination[1], Destination[3], Scalar(0,255,0),2);
    //line(frame, Destination[3], Destination[2], Scalar(0,255,0),2);
    //line(frame, Destination[2], Destination[0], Scalar(0,255,0),2);
    
    Matrix = getPerspectiveTransform(Source, Destination);
    warpPerspective(frame, framePers, Matrix, Size(360,240));
}

void Threshold()
{
    cvtColor(framePers, frameGray, COLOR_RGB2GRAY);
    inRange(frameGray, 50, 255, frameThresh);
    Canny(frameGray,frameEdge, 250, 100, 3, false);
    add(frameThresh, frameEdge, frameFinal);
    cvtColor(frameFinal, frameFinal, COLOR_GRAY2RGB);
    cvtColor(frameFinal, frameFinalDuplicate, COLOR_RGB2BGR); //used int histogram function only
}

void Histogram()
{
    histogramLane.resize(360);
    histogramLane.clear();
    for(int i=0; i<360; i++) //frame.size().width = 360
    {
        ROILane = frameFinalDuplicate(Rect(i, 140, 1, 100));
        divide(255, ROILane, ROILane);
        histogramLane.push_back((int)(sum(ROILane)[0]));
    }
}

void LaneFinder()
{
    vector<int>:: iterator LeftPtr;
    LeftPtr = max_element(histogramLane.begin(), histogramLane.begin() + 130);
    LeftLanePos = distance(histogramLane.begin(), LeftPtr);
    
    vector<int>:: iterator RightPtr;
    RightPtr = max_element(histogramLane.begin() + 220, histogramLane.end());
    RightLanePos = distance(histogramLane.begin(), RightPtr);
    
    line(frameFinal, Point2f(LeftLanePos,0), Point2f(LeftLanePos,240), Scalar(0,255,0),2);
    line(frameFinal, Point2f(RightLanePos,0), Point2f(RightLanePos,240), Scalar(0,255,0),2);
}

void LaneCenter()
{
    laneCenter = (RightLanePos-LeftLanePos)/2 + LeftLanePos;
    frameCenter = 174;
    line(frameFinal, Point2f(laneCenter,0), Point2f(laneCenter,240), Scalar(0,255,0), 3);
    line(frameFinal, Point2f(frameCenter,0), Point2f(frameCenter,240), Scalar(255,0,0), 3);
    
    Result = laneCenter-frameCenter;
}

void Stop_detection()
{
    if ( !Stop_Cascade.load("//home//pi//tcc//stop_cascade.xml"))
    {
        printf("Unable to open stop cascade file");
    }
    
    RoI_Stop = frame_stop(Rect(0,0,150,140)); //0,0,360,240
    cvtColor(RoI_Stop, gray_Stop, COLOR_RGB2GRAY);
    equalizeHist(gray_Stop, gray_Stop);
    Stop_Cascade.detectMultiScale(gray_Stop, Stop);
    
    for(int i=0; i < Stop.size(); i++)
    {
        Point P1(Stop[i].x, Stop[i].y);
        Point P2(Stop[i].x + Stop[i].width, Stop[i].x + Stop[i].height);
        
        rectangle(RoI_Stop, P1, P2, Scalar(0, 0, 255), 2);
        putText(RoI_Stop, "Stop Sign", P1, FONT_HERSHEY_PLAIN, 1 ,Scalar(0,0,255,255), 2);
        
        ss.str("");
        ss.clear();
        ss<<"D = "<<P2.x - P1.x<<" (pixels)";
        putText(RoI_Stop, ss.str(), Point2f(1,130), 0, 1, Scalar(0,0,255), 2);
    }
}

int main(int argc, char **argv)
{
    wiringPiSetup();
    pinMode(21, OUTPUT);
    pinMode(22, OUTPUT);
    pinMode(23, OUTPUT);
    pinMode(24, OUTPUT);
    
    Setup(argc, argv, Camera);
    cout << "Connecting to Camera" << endl;
    if (!Camera.open())
    {
        cout << "Failed to connect" << endl;
        return -1;
    }
    cout << "Camera ID = " << Camera.getId() << endl;

    while(1)
    {
        auto start = std::chrono::system_clock::now();
        
        Capture();
        Perspective();
        Threshold();
        Histogram();
        LaneFinder();
        LaneCenter();
        Stop_detection();
        
        if (Result == 0)
        {
            digitalWrite(21, 0);
            digitalWrite(22, 0); //decimal = 0
            digitalWrite(23, 0);
            digitalWrite(24, 0);
            cout<<"Forward"<<endl;
        }
        else if (Result > 0 && Result < 10)
        {
            digitalWrite(21, 1);
            digitalWrite(22, 0); //decimal = 1
            digitalWrite(23, 0);
            digitalWrite(24, 0);
            cout<<"Right1"<<endl;
        }
        else if (Result >= 10 && Result < 20)
        {
            digitalWrite(21, 0);
            digitalWrite(22, 1); //decimal = 2
            digitalWrite(23, 0);
            digitalWrite(24, 0);
            cout<<"Right2"<<endl;
        }
        else if (Result > 20)
        {
            digitalWrite(21, 1);
            digitalWrite(22, 1); //decimal = 3
            digitalWrite(23, 0);
            digitalWrite(24, 0);
            cout<<"Right3"<<endl;
        }
        else if (Result < 0 && Result > -10)
        {
            digitalWrite(21, 0);
            digitalWrite(22, 0); //decimal = 4
            digitalWrite(23, 1);
            digitalWrite(24, 0);
            cout<<"Left1"<<endl;
        }
        else if (Result <= -10 && Result > -20)
        {
            digitalWrite(21, 1);
            digitalWrite(22, 0); //decimal = 5
            digitalWrite(23, 1);
            digitalWrite(24, 0);
            cout<<"Left2"<<endl;
        }
        else if (Result < -20)
        {
            digitalWrite(21, 0);
            digitalWrite(22, 1); //decimal = 6
            digitalWrite(23, 1);
            digitalWrite(24, 0);
            cout<<"Left3"<<endl;
        }
        
        ss.str(" ");
        ss.clear();
        ss<<"Result = "<<Result;
        putText(frame, ss.str(), Point2f(1,50), 0,1,Scalar(0,0,255),2);
        
        namedWindow("Original", WINDOW_KEEPRATIO);
        moveWindow("Original", 0, 0);
        resizeWindow("Original", 640, 480);
        imshow("Original", frame);

        namedWindow("Perspective", WINDOW_KEEPRATIO);
        moveWindow("Perspective", 640, 0);
        resizeWindow("Perspective", 640, 480);
        imshow("Perspective", framePers);
        
        namedWindow("Final", WINDOW_KEEPRATIO);
        moveWindow("Final", 1280, 0);
        resizeWindow("Final", 640, 480);
        imshow("Final", frameFinal);

        namedWindow("Stop Sign", WINDOW_KEEPRATIO);
        moveWindow("Stop Sign", 1280, 480);
        resizeWindow("Stop Sign", 640, 480);
        imshow("Stop Sign", RoI_Stop);

        waitKey(1);
        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end-start;
        float t = elapsed_seconds.count();
        int FPS = 1/t;

        cout << "FPS = " << FPS << endl;

    }

    return 0;
}
