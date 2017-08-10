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

#include "vision.hpp"
#include "networktables/NetworkTable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

using namespace std;
using namespace std::chrono;



typedef unsigned long long ui64;
typedef long long i64;

struct VisionResultsPackage;

extern inline ui64 millis_since_epoch() {
    ui64 time = (time_point_cast<milliseconds>
        (system_clock::now()).time_since_epoch()).count();
    return time;
}

//network tables helper functions
void putNumber (const string &name, const double value);
void putString (const string &name, const string &value);
void putBoolean (const string &name, const bool &value);

//useful for testing OpenCV drawing to see you can modify an image
void fillCircle (cv::Mat img, int rad, cv::Point center);


/**
 * returns a gstreamer pipeline for reading an image from the camera
 * and saving it to an opencv IplImage object
 * @param vid_device video input to use, if there are multiple cameras
 * @param width width dimension of resolution (must be in allowed dims)
 * @param height height dimension of resolution (must be in allowed dims)
 * @param framerate number of frames per second to grab from the camera
 * @param mjpeg whether or not to use mjpeg compression on the camera hardware 
        before image grabbed by jetson; could decrease usb bandwith but increase
        time to decode. If false, grabs raw yuyv image.
 * @return gstreamer pipeling string to feed to CvCapture_GStreamer class
 */
string createReadPipeline (int vid_device, int width, int height, 
    int framerate, bool mjpeg);

/**
 * returns a gstreamer pipeline for reading an image from the camera
 * and splitting it into two sinks, one that saves an opencv IplImage and one 
 * that sends the image over the network to a driver-station laptop using h264 
 * encoding
 * @param vid_device video input to use, if there are multiple cameras
 * @param width width dimension of resolution (must be in allowed dims)
 * @param height height dimension of resolution (must be in allowed dims)
 * @param framerate number of frames per second to grab from the camera
 * @param mjpeg whether or not to use mjpeg compression on the camera hardware 
        before image grabbed by jetson; could decrease usb bandwith but increase
        time to decode. If false, grabs raw yuyv image.
 * @param bitrate kbit/sec to send over the network
 * @param ip destination ip address of the image
 * @param port destination port of the image
 * @return gstreamer pipeling string to feed to CvCapture_GStreamer class
 */
string createReadPipelineSplit (int vid_device, int width, int height, 
    int framerate, bool mjpeg, int bitrate, string ip, int port);

/**
 * returns a gstreamer pipeline for writing an image to the network from an 
 * OpenCV IplImage object
 * @param width width dimension of resolution (must be in allowed dims)
 * @param height height dimension of resolution (must be in allowed dims)
 * @param framerate number of frames per second to send over the network
 * @param bitrate kbit/sec to send over the network
 * @param ip destination ip address of the image
 * @param port destination port of the image
 * @return gstreamer pipeling string to feed to CvVideoWriter_GStreamer class
 */
string create_write_pipeline (int width, int height, int framerate, 
    int bitrate, string ip, int port);


void pushToNetworkTables (VisionResultsPackage info);

//camera parameters
const int 
device = 0, //bc we are using video0 in this case
width = 320, 
height = 240, 
framerate = 15, 
mjpeg = false; //mjpeg is not better than just grabbing a raw image in this case

//network parameters
const int
bitrate = 600000, //kbit/sec over network
port_stream = 5806, //destination port for raw image
port_thresh = 5805; //destination port for thresholded image
string ip = "192.168.1.34"; //destination ip

const string tableName = "CVResultsTable";

const bool verbose = true;

struct CameraSettings {
    int 
    brightness = 100,
    contrast = 10,
    saturation = 100,
    white_balance_temperature_auto = 0,
    white_balance_temperature = 9000,
    power_line_frequency = 2,
    sharpness = 25,
    backlight_compensation = 0,
    exposure_auto = 1,
    exposure_absolute = 5;
};

void flash_setting (string setting, int value);
void flash_settings (CameraSettings settings);

#endif