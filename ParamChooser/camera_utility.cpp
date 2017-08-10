#include "helper.hpp"

void flash_good_settings() {
    char setting_script[100];
    sprintf (setting_script, "bash camera_settings.sh %d", device);
    system (setting_script);
}

void flash_bad_settings() {
    char setting_script[100];
    sprintf (setting_script, "bash bad_settings.sh %d", device);
    system (setting_script);
}

void flash_setting (string setting, int value) {
    char setting_script[100];
    sprintf (setting_script, "bash bad_settings.sh %d %d", 
        setting.c_str(), value);
    system (setting_script);
}

void flash_settings (CameraSettings settings) {
    set_setting ("brightness", settings.brightness);
    set_setting ("contrast", settings.contrast);
    set_setting ("saturation", settings.saturation);
    set_setting ("white_balance_temperature_auto", settings.white_balance_temperature_auto);
    set_setting ("white_balance_temperature", settings.white_balance_temperature);
    set_setting ("power_line_frequency", settings.power_line_frequency);
    set_setting ("sharpness", settings.sharpness);
    set_setting ("backlight_compensation", settings.backlight_compensation);
    set_setting ("exposure_auto", settings.exposure_auto);
    set_setting ("exposure_absolute", settings.exposure_absolute);
}
