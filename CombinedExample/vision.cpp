#include "vision.h"
#include <iostream>
using namespace std;


//Set up constants
cv::RNG rng(12345);
cv::Scalar MY_RED (0, 0, 255);
cv::Scalar MY_BLUE (255, 0, 0);
cv::Scalar MY_GREEN (0, 255, 0);
cv::Scalar MY_PURPLE (255, 0, 255);
cv::Scalar GUIDE_DOT(255,255,0);

//utility functions
void CopyPointData(const cv::Point & pSource, cv::Point2d & pTarget)
{
	pTarget.x = pSource.x;
	pTarget.y = pSource.y;
}

inline int getHue (cv::Mat &img, int r, int c) {
	return img.at<cv::Vec3b>(r, c)[0];
}

inline int getSat (cv::Mat &img, int r, int c) {
	return img.at<cv::Vec3b>(r, c)[1];
}

inline int getVal (cv::Mat &img, int r, int c) {
	return img.at<cv::Vec3b>(r, c)[2];
}

void draw_point (cv::Mat &img, cv::Point &p, cv::Scalar &color) {
	cv::circle(img, p, 4, color, 4);
}

//checks for contour validity
bool is_valid (contour_type &contour) {
	bool valid = true; //start out assuming its valid, disprove this later

	//find bounding rect & convex hull
	cv::Rect rect = cv::boundingRect(contour);
	contour_type hull;
	cv::convexHull(contour, hull);

	double totalArea = (RES_X * RES_Y);

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


VisionResultsPackage calculate(cv::Mat &bgr,
		cv::Point2d &ul_final,
		cv::Point2d &ur_final,
		cv::Point2d &ll_final,
		cv::Point2d &lr_final,
		cv::Mat &greenMat){

	
	//blur the image
	cv::blur(bgr, bgr, cv::Size(5,5));
	cv::Mat hsvMat;
	//convert to hsv
	cv::cvtColor(bgr, hsvMat, cv::COLOR_BGR2HSV);

	//store HSV values at a given test point to send back
	cv::Point testPoint(120,120);
	int hue = getHue(hsvMat, testPoint.x,testPoint.y);
	int sat = getSat(hsvMat, testPoint.x,testPoint.y);
	int val = getVal(hsvMat, testPoint.x,testPoint.y);

	//threshold on green (light ring color)
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
	cout << "NumContours: " << numContours << endl;
	
	
	
	//find the largest contour in the image
	contour_type largest;
	double largestArea = 0;
	for (int i=0; i<numContours; i++){
		double curArea = cv::contourArea(valid_contour_hulls[i], true);
		if (curArea > largestArea){
			largestArea = curArea;
			largest = valid_contour_hulls[i];
		}
	}

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

	cv::threshold (greenMat, greenMat, 0, 255, cv::THRESH_BINARY);
    cv::cvtColor(greenMat, greenMat, CV_GRAY2BGR);

	//draw the 4 corners on the image
	draw_point (bgr, ul, MY_BLUE);
  	draw_point (bgr, ur, MY_RED);
  	draw_point (bgr, ll, MY_BLUE);
  	draw_point (bgr, lr, MY_RED);
	draw_point (bgr, testPoint, GUIDE_DOT);

  	draw_point (greenMat, ul, MY_BLUE);
  	draw_point (greenMat, ur, MY_RED);
  	draw_point (greenMat, ll, MY_BLUE);
  	draw_point (greenMat, lr, MY_RED);
	draw_point (greenMat, testPoint, GUIDE_DOT);
	
	//pass the point data back
	CopyPointData(ul, ul_final);
	CopyPointData(ur, ur_final);
	CopyPointData(ll, ll_final);
	CopyPointData(lr, lr_final);

	//find the center of mass of the largest contour
	cv::Moments centerMass = cv::moments(largest, true);
	double centerX = (centerMass.m10)/(centerMass.m00);
	double centerY = (centerMass.m01)/(centerMass.m00);
	cv::Point center(centerX,centerY);

	//draw the center of mass and the contour itself
	draw_point (bgr, center, MY_PURPLE);
	draw_point (greenMat, center, MY_PURPLE);
	vector<contour_type> largestArr;
	largestArr.push_back(largest);
	cv::drawContours(greenMat, largestArr , 0, MY_GREEN, 2);

	double top_width = ur.x - ul.x;
	double bottom_width = lr.x - ll.x;
	double left_height = ll.y - ul.y;
	double right_height = lr.y - ur.y;

	//create the results package
	VisionResultsPackage res;
	res.midPoint = center;
	res.widths[0] = top_width;
	res.widths[1] = bottom_width;
	res.heights[0] = left_height;
	res.heights[1] = right_height;
	res.hsv[0] = hue;
	res.hsv[1] = sat;
	res.hsv[2] = val;
	return res;
}
