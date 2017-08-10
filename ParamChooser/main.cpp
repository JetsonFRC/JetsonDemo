#include "helper.hpp"
#include "cam_helper.hpp"
#include "gst_pipeline.hpp"
#include "color.hpp"
#include "cam_helper.hpp"

using namespace std;

shared_ptr<NetworkTable> myNetworkTable; //our networktable for reading/writing
string netTableAddress = "192.168.1.34"; //address of the rio

//camera parameters
int 
device = 0, //bc we are using video0 in this case
width = 320, 
height = 240, 
framerate = 15, 
mjpeg = false; //mjpeg is not better than just grabbing a raw image in this case

//network parameters
int
bitrate = 600000, //kbit/sec over network
port_stream = 5806, //destination port for raw image
port_thresh = 5805; //destination port for thresholded image
string ip = "192.168.1.34"; //destination ip

string tableName = "CVResultsTable";
bool verbose = true;

namespace params {
    int test_x = 50, test_y = 50;
    int min_hue = 0, max_hue = 255;
    int min_sat = 0, max_sat = 255;
    int min_val = 0, max_val = 255;
    int blur = 5;
}

CameraSettings cam_settings;

struct ImgResults {
    int hue, sat, val;
};

ImgResults simple_pipeline (const cv::Mat &bgr, cv::Mat &processedImage);
void send_initial_img_params ();
void update_img_params ();
void set_cam_params ();
void update_camera_params ();

void init_networktables () {
    NetworkTable::SetClientMode();
    NetworkTable::SetDSClientEnabled(false);
    NetworkTable::SetIPAddress(llvm::StringRef(netTableAddress));
    NetworkTable::Initialize();
    if (verbose) printf ("Initialized table\n");
    myNetworkTable = NetworkTable::GetTable(tableName);
}

int main () {
    init_networktables();

    send_initial_img_params ();
    set_cam_params();

    //open camera using CvCapture_GStreamer class
    CvCapture_GStreamer mycam;
    string read_pipeline = createReadPipelineSplit (
            device, width, height, framerate, mjpeg, 
            bitrate, ip, port_stream);
    if (verbose) {
        printf ("GStreamer read pipeline: %s\n", read_pipeline.c_str()); 
    }
    mycam.open (CV_CAP_GSTREAMER_FILE, read_pipeline.c_str());

    if (verbose) {
        printf ("Succesfully opened camera with dimensions: %dx%d\n",
            width, height);
    }

    //open vidoe writer using CvVideoWriter_GStreamer class
    CvVideoWriter_GStreamer mywriter;
    string write_pipeline = create_write_pipeline (width, height, framerate, 
            bitrate, ip, port_thresh);
    if (verbose) {
        printf ("GStreamer write pipeline: %s\n", write_pipeline.c_str());
    }
    mywriter.open (write_pipeline.c_str(), 
        0, framerate, cv::Size(width, height), true);

    //initialize raw & processed image matrices
    cv::Mat cameraFrame, processedImage;

    //take each frame from the pipeline
    for (long long frame = 0; ; frame++) {
        update_img_params();
        update_camera_params();

        bool success = mycam.grabFrame();
        if (verbose) printf ("frame #%lld\n", frame);

        if (success) {
            //printf ("TEST X, TEST Y: %d, %d\n", params::test_x, params::test_y);

            IplImage *img = mycam.retrieveFrame(0); //store frame in IplImage
            cameraFrame = cv::cvarrToMat (img); //convert IplImage to cv::Mat
            processedImage = cameraFrame; 
    
            ImgResults res = simple_pipeline (cameraFrame, processedImage);
            myNetworkTable -> PutNumber ("TEST_HUE", res.hue);
            myNetworkTable -> PutNumber ("TEST_SAT", res.sat);
            myNetworkTable -> PutNumber ("TEST_VAL", res.val);

            //pass the results back out
            IplImage outImage = (IplImage) processedImage;
            mywriter.writeFrame (&outImage); //write output image over network
        }

        //delay for 10 millisecondss
        usleep (10);
    }

    mywriter.close();
    mycam.close();
    return 0;
}


ImgResults simple_pipeline (const cv::Mat &bgr, cv::Mat &processedImage) {

    //blur the image
    cv::Point testPoint (params::test_x, params::test_y);
    cv::blur(bgr, bgr, cv::Size(params::blur, params::blur));
    cv::Mat hsvMat;
    //convert to hsv
    cv::cvtColor(bgr, hsvMat, cv::COLOR_BGR2HSV);

    //store HSV values at a given test point to send back
    int hue = getHue(hsvMat, testPoint.x, testPoint.y);
    int sat = getSat(hsvMat, testPoint.x, testPoint.y);
    int val = getVal(hsvMat, testPoint.x, testPoint.y);

    //threshold on green (light ring color)
    cv::Mat greenThreshed;
    cv::inRange(hsvMat,
                cv::Scalar(params::min_hue, params::min_sat, params::min_val),
                cv::Scalar(params::max_hue, params::max_sat, params::max_val),
                greenThreshed);

    processedImage = greenThreshed.clone();
    cv::threshold (processedImage, processedImage, 0, 255, cv::THRESH_BINARY);
    cv::cvtColor (processedImage, processedImage, CV_GRAY2BGR); 
    //processedImage = bgr.clone();  

    drawPoint (processedImage, testPoint, PURPLE);

    ImgResults res;
    res.hue = hue;
    res.sat = sat;
    res.val = val;

    return res;
}

void send_initial_img_params () {
    myNetworkTable -> PutNumber ("PARAM_TEST_X", params::test_x);
    myNetworkTable -> PutNumber ("PARAM_TEST_Y", params::test_y);
    myNetworkTable -> PutNumber ("PARAM_MIN_HUE", params::min_hue);
    myNetworkTable -> PutNumber ("PARAM_MAX_HUE", params::max_hue);
    myNetworkTable -> PutNumber ("PARAM_MIN_SAT", params::min_sat);
    myNetworkTable -> PutNumber ("PARAM_MAX_SAT", params::max_sat);
    myNetworkTable -> PutNumber ("PARAM_MIN_VAL", params::min_val);
    myNetworkTable -> PutNumber ("PARAM_MAX_VAL", params::max_val);
    myNetworkTable -> PutNumber ("PARAM_BLUR", params::blur);
}

void update_img_params () {
    params::test_x = myNetworkTable -> GetNumber ("PARAM_TEST_X", params::test_x);
    params::test_y = myNetworkTable -> GetNumber ("PARAM_TEST_Y", params::test_y);
    params::min_hue = myNetworkTable -> GetNumber ("PARAM_MIN_HUE", params::min_hue);
    params::max_hue = myNetworkTable -> GetNumber ("PARAM_MAX_HUE", params::max_hue);
    params::min_sat = myNetworkTable -> GetNumber ("PARAM_MIN_SAT", params::min_sat);
    params::max_sat = myNetworkTable -> GetNumber ("PARAM_MAX_SAT", params::max_sat);
    params::min_val = myNetworkTable -> GetNumber ("PARAM_MIN_VAL", params::min_val);
    params::max_val = myNetworkTable -> GetNumber ("PARAM_MAX_VAL", params::max_val);
    params::blur = myNetworkTable -> GetNumber ("PARAM_BLUR", params::blur);
}

void push_settings_to_network (CameraSettings settings) {
    myNetworkTable -> PutNumber ("CAM_brightness", settings.brightness);
    myNetworkTable -> PutNumber ("CAM_contrast", settings.contrast);
    myNetworkTable -> PutNumber ("CAM_saturation", settings.saturation);
    myNetworkTable -> PutNumber ("CAM_white_balance_temperature_auto", settings.white_balance_temperature_auto);
    myNetworkTable -> PutNumber ("CAM_white_balance_temperature", settings.white_balance_temperature);
    myNetworkTable -> PutNumber ("CAM_power_line_frequency", settings.power_line_frequency);
    myNetworkTable -> PutNumber ("CAM_sharpness", settings.sharpness);
    myNetworkTable -> PutNumber ("CAM_backlight_compensation", settings.backlight_compensation);
    myNetworkTable -> PutNumber ("CAM_exposure_auto", settings.exposure_auto);
    myNetworkTable -> PutNumber ("CAM_exposure_absolute", settings.exposure_absolute);
}

CameraSettings grab_settings_from_network () {
    CameraSettings result;
    result.brightness = myNetworkTable -> GetNumber ("CAM_brightness", cam_settings.brightness);
    result.contrast = myNetworkTable -> GetNumber ("CAM_contrast", cam_settings.contrast);
    result.saturation = myNetworkTable -> GetNumber ("CAM_saturation", cam_settings.saturation);
    result.white_balance_temperature_auto = myNetworkTable -> GetNumber ("CAM_white_balance_temperature_auto", cam_settings.white_balance_temperature_auto);
    result.white_balance_temperature = myNetworkTable -> GetNumber ("CAM_white_balance_temperature", cam_settings.white_balance_temperature);
    result.power_line_frequency = myNetworkTable -> GetNumber ("CAM_power_line_frequency", cam_settings.power_line_frequency);
    result.sharpness = myNetworkTable -> GetNumber ("CAM_sharpness", cam_settings.sharpness);
    result.backlight_compensation = myNetworkTable -> GetNumber ("CAM_backlight_compensation", cam_settings.backlight_compensation);
    result.exposure_auto = myNetworkTable -> GetNumber ("CAM_exposure_auto", cam_settings.exposure_auto);
    result.exposure_absolute = myNetworkTable -> GetNumber ("CAM_exposure_absolute", cam_settings.exposure_absolute);
    return result;
}

void set_cam_params () {
    flash_settings (device, cam_settings);
    push_settings_to_network (cam_settings);
}

void update_camera_params () {
    CameraSettings new_settings = grab_settings_from_network();
    if (new_settings != cam_settings) {
        cam_settings = new_settings;
        set_cam_params();
    }
}

