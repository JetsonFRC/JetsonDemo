#include "cam_helper.hpp"

void flash_settings (int device, const CameraSettings &settings) {
    char setting_script[1000];

    //create system call to use v4l2 to change camera settings according
    //to data in CameraSettings struct
    sprintf (setting_script, 
        "v4l2-ctl -d /dev/video%d  "
            "--set-ctrl brightness=%d "
            "--set-ctrl contrast=%d "
            "--set-ctrl saturation=%d "
            "--set-ctrl white_balance_temperature_auto=%d "
            "--set-ctrl white_balance_temperature=%d "
            "--set-ctrl power_line_frequency=%d "
            "--set-ctrl sharpness=%d "
            "--set-ctrl backlight_compensation=%d "
            "--set-ctrl exposure_auto=%d "
            "--set-ctrl exposure_absolute=%d ",
        device, 
        settings.brightness,
        settings.contrast,
        settings.saturation,
        settings.white_balance_temperature_auto,
        settings.white_balance_temperature,
        settings.power_line_frequency,
        settings.sharpness,
        settings.backlight_compensation,
        settings.exposure_auto,
        settings.exposure_absolute);

    //run the system call created in the above step
    system (setting_script);
}