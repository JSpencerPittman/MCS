#include <iostream>
#include <opencv2/opencv.hpp>
#include "stream_protocol.h"
#include <ctime>
#include <algorithm>
#include <chrono>
#include <thread>

using namespace std;

int main() {
    cv::VideoCapture cap("../media/donny.mp4");

    if(!cap.isOpened()) 
    {
        cout << "IT WONT OPEN WHAT DID YOU DOOOOO!!!!" << endl;
        return -1;
    }

    double fps = cap.get(cv::CAP_PROP_FPS);
    double timePerFrame = 1/fps;

    time_t timeSinceFrame = time(NULL);

    while (true) {
        cv::Mat frame;

        if (!cap.read(frame)) {
            cout << "UMM WERE DONE HERE." << endl;
            break;
        }

        cv::imshow("HERE PAL", frame);
        cv::waitKey(1);
        
        int waitTime = max(timePerFrame - difftime(time(NULL), timeSinceFrame), 0.0) * 1000000000;
        this_thread::sleep_for(chrono::nanoseconds(waitTime));
        time(&timeSinceFrame);
        
    }
}