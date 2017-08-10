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