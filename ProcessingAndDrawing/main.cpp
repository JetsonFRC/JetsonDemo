#include "helper.hpp"
#include "gst_pipeline.hpp"
#include "vision.hppq"

using namespace std;

shared_ptr<NetworkTable> myNetworkTable; //our networktable for reading/writing
string netTableAddress = "192.168.1.34"; //address of the rio

//useful for testing OpenCV drawing to see you can modify an image
void fillCircle (cv::Mat img, int rad, cv::Point center);
void pushToNetworkTables (VisionResultsPackage info);

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

void flash_good_settings() {
    char setting_script[100];
    sprintf (setting_script, "bash good_settings.sh %d", device);
    system (setting_script);
}

void flash_bad_settings() {
    char setting_script[100];
    sprintf (setting_script, "bash bad_settings.sh %d", device);
    system (setting_script);
}

int main () {
    //call the bash script to set camera settings
    flash_good_settings();

    //initialize NetworkTables
    NetworkTable::SetClientMode();
    NetworkTable::SetDSClientEnabled(false);
    NetworkTable::SetIPAddress(llvm::StringRef(netTableAddress));
    NetworkTable::Initialize();
    if (verbose) printf ("Initialized table\n");
    myNetworkTable = NetworkTable::GetTable(tableName);

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

    if (verbose) {
        printf ("Data header:\n%s", 
            VisionResultsPackage::createCSVHeader().c_str());
    }

    //take each frame from the pipeline
    for (long long frame = 0; ; frame++) {
        //have to alternate from bad settings to good settings on some cameras
        //because of weird firmware issues, sometimes the flash doesn't stick 
        //otherwise
        if (frame < 10) {
            flash_bad_settings();
        }
        else if (frame == 50) {
            flash_good_settings();
        }

        bool success = mycam.grabFrame();

        if (verbose) printf ("frame #%lld\n", frame);

        if (success) {
            IplImage *img = mycam.retrieveFrame(0); //store frame in IplImage
            cameraFrame = cv::cvarrToMat (img); //convert IplImage to cv::Mat
            processedImage = cameraFrame;
                
            //process the image, put the information into network tables
            VisionResultsPackage info = calculate(cameraFrame, processedImage);

            pushToNetworkTables (info);
          
            //pass the results back out
            IplImage outImage = (IplImage) processedImage;
            printf ("results string: %s\n", info.createCSVLine().c_str());
            if (verbose) {
                printf ("Out image stats: (depth %d), (nchannels %d)\n", 
                    outImage.depth, outImage.nChannels);
            }
            mywriter.writeFrame (&outImage); //write output image over network
        }

        //delay for 10 millisecondss
        usleep (10);
    }

    mywriter.close();
    mycam.close();
    return 0;
}

void fillCircle (cv::Mat img, int rad, cv::Point center) {
    int thickness = -1;
    int lineType = 8;
    cv::circle (img, center, rad, cv::Scalar(0, 0, 255), thickness, lineType);
}

void pushToNetworkTables (VisionResultsPackage info) {
    myNetworkTable -> PutString ("VisionResults", info.createCSVLine());
    myNetworkTable -> PutString ("VisionResultsHeader", info.createCSVHeader());
    myNetworkTable -> PutNumber ("Sample Hue", info.sampleHue);
    myNetworkTable -> PutNumber ("Sample Sat", info.sampleSat);
    myNetworkTable -> PutNumber ("Sample Val", info.sampleVal);
    myNetworkTable -> Flush();
}

