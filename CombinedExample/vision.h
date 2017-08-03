#ifndef VISION
#define VISION

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <vector>
#include <iostream>

	struct VisionResultsPackage{
		cv::Point midPoint;
		double widths[2];
		double heights[2];
		int hsv[3];
	};

	typedef std::vector<cv::Point> contour_type;

	const int RES_X = 320, RES_Y = 240;
	const int MIN_HUE = 60, MAX_HUE = 90; //65, 90
	const int MIN_SAT = 0, MAX_SAT = 255;
	const int MIN_VAL = 20, MAX_VAL = 255; //50, 255

	const double
	MIN_AREA = 0.001, MAX_AREA = 1000000,
	MIN_WIDTH = 0, MAX_WIDTH = 100000, //rectangle width
	MIN_HEIGHT = 0, MAX_HEIGHT = 100000, //rectangle height
	MIN_RECT_RAT = 1.5, MAX_RECT_RAT = 8, //rect height / rect width
	MIN_AREA_RAT = 0.85, MAX_AREA_RAT = 100; //convex hull area / contour area, this can probably be more aggressive

	VisionResultsPackage calculate(cv::Mat &bgr,
		cv::Point2d &ul_final,
		cv::Point2d &ur_final,
		cv::Point2d &ll_final,
		cv::Point2d &lr_final,
		cv::Mat &greenImage);

#endif