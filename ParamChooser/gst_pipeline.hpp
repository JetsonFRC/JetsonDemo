#ifndef GST_PIPELINE_HPP
#define GST_PIPELINE_HPP

#include <string>
#include <stdio.h>

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
extern string createReadPipeline (int vid_device, int width, int height, 
    int framerate, bool mjpeg) {

    char buff[500];
    if (mjpeg) {
        sprintf (buff,
            "v4l2src device=/dev/video%d ! "
            "image/jpeg,format=(string)BGR,width=(int)%d,height=(int)%d,framerate=(fraction)%d/1 ! "
            "jpegdec ! autovideoconvert ! appsink",
            vid_device, width, height, framerate);
    } 
    else {
        sprintf (buff,
            "v4l2src device=/dev/video%d ! "
            "video/x-raw,format=(string)BGR,width=(int)%d,height=(int)%d,framerate=(fraction)%d/1 ! "
            "autovideoconvert ! appsink",
            vid_device, width, height, framerate);
    }

    string pipstring = buff;
    printf ("read string: %s\n", pipstring.c_str());
    return pipstring;
}

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
extern string createReadPipelineSplit (int vid_device, int width, int height, 
    int framerate, bool mjpeg, int bitrate, string ip, int port) {

    char buff[500];
    if (mjpeg) {
        sprintf (buff,
            "v4l2src device=/dev/video%d ! "
            "image/jpeg,format=(string)BGR,width=(int)%d,height=(int)%d,framerate=(fraction)%d/1 ! jpegdec ! "
            "tee name=split "
                "split. ! queue ! videoconvert ! omxh264enc bitrate=%d ! "
                    "video/x-h264, stream-format=(string)byte-stream ! h264parse ! "
                    "rtph264pay ! udpsink host=%s port=%d "
                "split. ! queue ! autovideoconvert ! appsink",
            //"appsink",
            vid_device, width, height, framerate, bitrate, ip.c_str(), port);
    } 
    else {
        sprintf (buff,
            "v4l2src device=/dev/video%d ! "
            "video/x-raw,format=(string)BGR,width=(int)%d,height=(int)%d,framerate=(fraction)%d/1 ! "
            "tee name=split "
                "split. ! queue ! videoconvert ! omxh264enc bitrate=%d ! "
                    "video/x-h264, stream-format=(string)byte-stream ! h264parse ! "
                    "rtph264pay ! udpsink host=%s port=%d "
                "split. ! queue ! autovideoconvert ! appsink",
            //"appsink",
            vid_device, width, height, framerate, bitrate, ip.c_str(), port);
    }

    string pipstring = buff;
    printf ("read string: %s\n", pipstring.c_str());
    return pipstring;
}

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
extern string create_write_pipeline (int width, int height, int framerate, 
    int bitrate, string ip, int port) {

    char buff[500];
    sprintf (buff,
        "appsrc ! "
        "video/x-raw, format=(string)BGR, width=(int)%d, height=(int)%d, framerate=(fraction)%d/1 ! "
        "videoconvert ! omxh264enc bitrate=%d ! video/x-h264, stream-format=(string)byte-stream ! h264parse ! rtph264pay ! "
        "udpsink host=%s port=%d",
        width, height, framerate, bitrate, ip.c_str(), port);

     string pipstring = buff;
    
    printf ("write string: %s\n", pipstring.c_str());
    return pipstring;
}

#endif