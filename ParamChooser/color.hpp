/*
 * Useful file for color related utilities including retrieving color 
 * information from an image, color constants, and drawing points on an image
 */

#ifndef COLOR_HPP
#define COLOR_HPP



const cv::Scalar RED (0, 0, 255);
const cv::Scalar BLUE (255, 0, 0);
const cv::Scalar GREEN (0, 255, 0);
const cv::Scalar PURPLE (255, 0, 255);
const cv::Scalar TORQUOISE (255,255,0);

inline int getHue (const cv::Mat &img, int r, int c) {
    return img.at<cv::Vec3b>(r, c)[0];
}

inline int getSat (const cv::Mat &img, int r, int c) {
    return img.at<cv::Vec3b>(r, c)[1];
}

inline int getVal (const cv::Mat &img, int r, int c) {
    return img.at<cv::Vec3b>(r, c)[2];
}

void drawPoint (cv::Mat &img, const cv::Point &p, const cv::Scalar &color) {
    cv::circle(img, p, 4, color, 4);
}


#endif