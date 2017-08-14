#ifndef HELPER_HPP
#define HELPER_HPP

#include <chrono> //time
#include <stdio.h> //i/o
#include <string> //strings
#include <iostream> //more i/o

//opencv and image processing
#include <opencv2/highgui/highgui.hpp> 
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include "cap_gstreamer.hpp" //gstreamer capture helper

#include <time.h> //time
#include <sys/time.h> //time

#include "networktables/NetworkTable.h" //networktables
#include <stdlib.h> //c standard library
#include <string.h> //more strings

using namespace std;
using namespace std::chrono;

typedef unsigned long long ui64;
typedef long long i64;

//helper function to get system time
extern inline ui64 millis_since_epoch() {
    ui64 time = (time_point_cast<milliseconds>
        (system_clock::now()).time_since_epoch()).count();
    return time;
}

#endif