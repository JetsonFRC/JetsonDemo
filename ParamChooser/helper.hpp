#ifndef HELPER_HPP
#define HELPER_HPP

#include <chrono>
#include <stdio.h>
#include <string>
#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include "cap_gstreamer.hpp"

#include <time.h>
#include <sys/time.h>

#include "networktables/NetworkTable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;
using namespace std::chrono;

typedef unsigned long long ui64;
typedef long long i64;


extern inline ui64 millis_since_epoch() {
    ui64 time = (time_point_cast<milliseconds>
        (system_clock::now()).time_since_epoch()).count();
    return time;
}

#endif