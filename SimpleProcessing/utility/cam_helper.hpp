/*
Helper utilities for interacting with camera settings
*/

#ifndef CAM_HELPER_HPP
#define CAM_HELPER_HPP

#include <stdlib.h>
#include <stdio.h>
using namespace std;

/**
 * struct to hold camera setting information
 * can be passed to flash_settings function to flash usb cam settings with
 * video4linux. Comparator operators implement to easily check if stored camera
 * settings match those requested over networktables. Use v4l2-ctl -L command
 * to see which settings are available on your particular camera and what the
 * ranges are, you may need to modify this struct to match your camera.
 */
struct CameraSettings {
    //brightness: should be relatively high so the reflective tape is bright
    int brightness = 100; 

    //contrast: should be relatively high for higher contrast between light and 
    //dark parts of the image
    int contrast = 10; 

    //saturation: should be relatively high so the colored reflective tape has 
    //a full and consistent color
    int saturation = 100;

    //white balance temperature auto: menu setting, should be off
    int white_balance_temperature_auto = 0;

    //white balance temperature: not super important, but can be quite high
    int white_balance_temperature = 9000;

    //power line frequency: menu, on our camera 0=disabled, 1=50hz, 2=60hz
    int power_line_frequency = 2;

    //sharpness: does not matter much
    int sharpness = 25;

    //backlight compensation: off
    int backlight_compensation = 0;
    
    //exposure auto: menu item, should be set to off
    int exposure_auto = 1;

    //exposure absolute, should be as low as possible for minimal motion blur
    //and most consistent colors
    int exposure_absolute = 5;
};

inline bool operator== (const CameraSettings &s1,const CameraSettings &s2) {
    return
        s1.brightness == s2.brightness &&
        s1.contrast == s2.contrast &&
        s1.saturation == s2.saturation &&
        s1.white_balance_temperature_auto == s2.white_balance_temperature_auto &&
        s1.white_balance_temperature == s2.white_balance_temperature &&
        s1.power_line_frequency == s2.power_line_frequency &&
        s1.sharpness == s2.sharpness &&
        s1.backlight_compensation == s2.backlight_compensation &&
        s1.exposure_auto == s2.exposure_auto &&
        s1.exposure_absolute == s2.exposure_absolute;
}

inline bool operator!= (const CameraSettings &s1,const CameraSettings &s2) {
    return !(s1 == s2);
}

/**
 * flashes camera settings stored in struct to specified device
 * @param device video input to flash settings, usually 0 or 1. Check which 
 *  device your camera is on with ls /dev/video*
 * @param settings camera settings to be flashed
 */
void flash_settings (int device, const CameraSettings &settings);

#endif