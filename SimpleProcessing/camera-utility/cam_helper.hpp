#ifndef CAM_HELPER_HPP
#define CAM_HELPER_HPP

#include "helper.hpp"

struct CameraSettings {
    int brightness = 100;
    int contrast = 10;
    int saturation = 100;
    int white_balance_temperature_auto = 0;
    int white_balance_temperature = 9000;
    int power_line_frequency = 2;
    int sharpness = 25;
    int backlight_compensation = 0;
    int exposure_auto = 1;
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

void flash_settings (int device, CameraSettings settings);

#endif