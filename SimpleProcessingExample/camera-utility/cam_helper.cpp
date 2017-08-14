#include "cam_helper.hpp"

void flash_setting (int device, string setting, int value) {
    char setting_script[100];
    sprintf (setting_script, "bash /3419/JetsonDemo/ParamChooser/camera-utility/set_cam_setting.sh %d %s %d", 
        device, setting.c_str(), value);
    system (setting_script);
}

void flash_settings (int device, CameraSettings settings) {
    flash_setting (device, "brightness", settings.brightness);
    flash_setting (device, "contrast", settings.contrast);
    flash_setting (device, "saturation", settings.saturation);
    flash_setting (device, "white_balance_temperature_auto", settings.white_balance_temperature_auto);
    flash_setting (device, "white_balance_temperature", settings.white_balance_temperature);
    flash_setting (device, "power_line_frequency", settings.power_line_frequency);
    flash_setting (device, "sharpness", settings.sharpness);
    flash_setting (device, "backlight_compensation", settings.backlight_compensation);
    flash_setting (device, "exposure_auto", settings.exposure_auto);
    flash_setting (device, "exposure_absolute", settings.exposure_absolute);
}
