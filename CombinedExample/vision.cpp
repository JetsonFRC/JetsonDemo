#include "vision.hpp"
#include "helper.hpp"
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
void CopyPointData (const cv::Point &pSource, cv::Point2d &pTarget) {
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


VisionResultsPackage calculate(cv::Mat &bgr, cv::Mat &processedImage){
    ui64 time_began = millis_since_epoch();
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
                processedImage);

    //contour detection
    vector<contour_type> contours;
    vector<cv::Vec4i> hierarchy; //throwaway, needed for function
    try {
        cv::findContours (processedImage, contours, hierarchy, 
            cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
    }
    catch (...) { //TODO: change this to the error that occurs when there are no contours
        return processingFailurePackage();
    }

    if (contours.size() < 1) { //definitely did not find 
        return processingFailurePackage();
    }
    
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
    printf ("Num contours: %d\n", numContours);
    
    if (numContours < 1) { //definitely did not find 
        return processingFailurePackage();
    }

    //find the largest contour in the image
    contour_type largest;
    double largestArea = 0;
    for (int i = 0; i < numContours; i++){
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

    cv::threshold (processedImage, processedImage, 0, 255, cv::THRESH_BINARY);
    cv::cvtColor(processedImage, processedImage, CV_GRAY2BGR);

    //draw the 4 corners on the image
    draw_point (bgr, ul, MY_BLUE);
    draw_point (bgr, ur, MY_RED);
    draw_point (bgr, ll, MY_BLUE);
    draw_point (bgr, lr, MY_RED);
    draw_point (bgr, testPoint, GUIDE_DOT);

    draw_point (processedImage, ul, MY_BLUE);
    draw_point (processedImage, ur, MY_RED);
    draw_point (processedImage, ll, MY_BLUE);
    draw_point (processedImage, lr, MY_RED);
    draw_point (processedImage, testPoint, GUIDE_DOT);

    //find the center of mass of the largest contour
    cv::Moments centerMass = cv::moments(largest, true);
    double centerX = (centerMass.m10) / (centerMass.m00);
    double centerY = (centerMass.m01) / (centerMass.m00);
    cv::Point center (centerX, centerY);

    //draw the center of mass and the contour itself
    draw_point (bgr, center, MY_PURPLE);
    draw_point (processedImage, center, MY_PURPLE);
    vector<contour_type> largestArr;
    largestArr.push_back(largest);
    cv::drawContours(processedImage, largestArr , 0, MY_GREEN, 2);

    double top_width = ur.x - ul.x;
    double bottom_width = lr.x - ll.x;
    double left_height = ll.y - ul.y;
    double right_height = lr.y - ur.y;

    //create the results package
    VisionResultsPackage res;
    res.timestamp = time_began;
    
    CopyPointData (ul, res.ul);
    CopyPointData (ur, res.ur);
    CopyPointData (ll, res.ll);
    CopyPointData (lr, res.lr);
    CopyPointData (center, res.midPoint);

    res.upperWidth = top_width;
    res.lowerWidth = bottom_width;
    res.leftHeight = left_height;
    res.rightHeight = right_height;

    res.sampleHue = hue;
    res.sampleSat = sat;
    res.sampleVal = val;
    return res;
}