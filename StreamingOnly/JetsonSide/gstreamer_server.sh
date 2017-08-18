#!/bin/bash

gst-launch-1.0 -v -e v4l2src device=/dev/video0 -v !\
     'video/x-raw, format=(string)I420, width=(int)800, height=(int)448' !\
     omxh264enc bitrate=600000  ! 'video/x-h264, stream-format=(string)byte-stream' !\
     h264parse ! rtph264pay ! udpsink host=$1 port=$2
