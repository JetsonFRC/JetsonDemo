#!/bin/bash
v4l2-ctl -d /dev/video$1 \
--set-ctrl brightness=20 \
--set-ctrl contrast=2 \
--set-ctrl saturation=50 \
--set-ctrl white_balance_temperature_auto=0 \
--set-ctrl white_balance_temperature=9000 \
--set-ctrl power_line_frequency=2 \
--set-ctrl sharpness=24 \
--set-ctrl backlight_compensation=0 \
--set-ctrl exposure_auto=1 \
--set-ctrl exposure_absolute=10