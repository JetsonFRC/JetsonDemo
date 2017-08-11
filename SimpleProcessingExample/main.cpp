#include "VisionResultsPackage.h"
#include "gst_pipeline.hpp"
#include "helper.hpp"

using namespace std;

const double
	MIN_AREA = 0.001, MAX_AREA = 1000000,
	MIN_WIDTH = 0, MAX_WIDTH = 100000, //rectangle width
	MIN_HEIGHT = 0, MAX_HEIGHT = 100000, //rectangle height
	MIN_RECT_RAT = 1.5, MAX_RECT_RAT = 8, //rect height / rect width
	MIN_AREA_RAT = 0.85, MAX_AREA_RAT = 100; //convex hull area / contour area, this can probably be more aggressive

int device = 0;
int width = 320;
int height = 240;
int framerate = 15;
bool mjpeg = false;

int bitrate = 600000;
int port = 5805;

string ip = "192.168.1.34";

shared_ptr<NetworkTable> mNetworkTable;

void startNetworkTables(shared_ptr<NetworkTable> pNetworkTable, string tableName){
	NetworkTable::SetClientMode();
    NetworkTable::SetDSClientEnabled(false);
    NetworkTable::SetIPAddress(llvm::StringRef(ip));
    NetworkTable::Initialize();
	pNetworkTable = NetworkTable::GetTable(tableName);
}

bool is_valid (contour_type &contour) {
	bool valid = true; //start out assuming its valid, disprove this later

	//find bounding rect & convex hull
	cv::Rect rect = cv::boundingRect(contour);
	contour_type hull;
	cv::convexHull(contour, hull);

  double totalArea = (width * height);

	//calculate relevant ratios & values
	double area = cv::contourArea(contour) / totalArea;
	//double perim = cv::arcLength(hull, true);

	double convex_area = cv::contourArea(hull) / totalArea;

	double width = rect.width, height = rect.height;

	double area_rat = area / convex_area;
	double rect_rat = height / width;

  //check ratios & values for validity
	if (area < MIN_AREA || area > MAX_AREA) valid = false;
	if (area_rat < MIN_AREA_RAT || area_rat > MAX_AREA_RAT) valid = false;
	if (rect_rat < MIN_RECT_RAT || rect_rat > MAX_RECT_RAT) valid = false;
	if (width < MIN_WIDTH || width > MAX_WIDTH) valid = false;
	if (height < MIN_HEIGHT || height > MAX_HEIGHT) valid = false;

	return valid;
}

void ProcessImage(cv::Mat& bgr, VisionResultsPackage& results){
	//blur first
	cv::blur(bgr, bgr, cv::Size(5,5));
	cv::Mat hsvMat;
	//convert to hsv
	cv::cvtColor(bgr, hsvMat, cv::COLOR_BGR2HSV);
	cv::Mat greenMat;
	cv::inRange(hsvMat,
				cv::Scalar(MIN_HUE, MIN_SAT, MIN_VAL),
				cv::Scalar(MAX_HUE, MAX_SAT, MAX_VAL),
				greenMat);

	//contour detection
	vector<contour_type> contours;
	vector<cv::Vec4i> hierarchy; //throwaway, needed for function
	cv::findContours( greenMat, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
	
	//store the convex hulls of any valid contours
	vector<contour_type> valid_contour_hulls;
	for (int i = 0; i < (int)contours.size(); i++) {
		contour_type contour = contours[i];
		if (is_valid (contour)) {
			contour_type hull;
			cv::convexHull(contour, hull);
			valid_contour_hulls.push_back(hull);
		}
	}

	int numContours = valid_contour_hulls.size();
	results.put("Total Contours Found", numContours);
	contour_type largest;
	double largestArea = 0;

	//find the largest contour in the image
	for (int i=0; i<numContours; i++){
		double curArea = cv::contourArea(valid_contour_hulls[i], true);
		if (curArea > largestArea){
			largestArea = curArea;
			largest = valid_contour_hulls[i];
		}
	}

	results.put("Largest Contour Area", largestArea);

	//get the points of corners
	vector<cv::Point> all_points;
	all_points.insert (all_points.end(), largest.begin(), largest.end());

	//find which corner is which
	cv::Point ul (1000, 1000), ur (0, 1000), ll (1000, 0), lr (0, 0);
	for (int i = 0; i < (int)all_points.size(); i++) {
		int sum = all_points[i].x + all_points[i].y;
		int dif = all_points[i].x - all_points[i].y;

		if (sum < ul.x + ul.y) {
			ul = all_points[i];
		}

		if (sum > lr.x + lr.y) {
			lr = all_points[i];
		}

		if (dif < ll.x - ll.y) {
			ll = all_points[i];
		}

		if (dif > ur.x - ur.y) {
			ur = all_points[i];
		}
	}

	//find the center of mass of the largest contour
	cv::Moments centerMass = cv::moments(largest, true);
	double centerX = (centerMass.m10)/(centerMass.m00);
	double centerY = (centerMass.m01)/(centerMass.m00);
	cv::Point center(centerX,centerY);

	double top_width = ur.x - ul.x;
	double bottom_width = lr.x - ll.x;
	double left_height = ll.y - ul.y;
	double right_height = lr.y - ur.y;

	results.put("Center X", centerX);
	results.put("Center Y", centerY);
	results.put("Top Width", top_width);
	results.put("Bottom Width", bottom_width);
	results.put("Left Height", left_height);
	results.put("Right Height", right_height);
	string t(millis_since_epoch());
	results.setWriteTime(t);
	results.writeToNetworkTables();
}

int main(){
	startNetworkTables(mNetworkTable, "SimpleVisionProcessing");

	CvCapture_GStreamer mycam;
    string read_pipeline = createReadPipelineSplit (
            device, width, height, framerate, mjpeg, 
            bitrate, ip, port);

	mycam.open (CV_CAP_GSTREAMER_FILE, read_pipeline.c_str());


}