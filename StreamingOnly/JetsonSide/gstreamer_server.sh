#!/bin/bash

#takes three arguments: camera, client IP, client port
#e.g bash gstreamer_server.sh 1 192.168.1.34 5805
#e.g bash gstreamer_server.sh 1 10.34.19.5 5805

gst-launch-1.0 -v -e v4l2src device=/dev/video$1 -v !\
     'video/x-raw, format=(string)I420, width=(int)640, height=(int)480' !\
     omxh264enc bitrate=600000  ! 'video/x-h264, stream-format=(string)byte-stream' !\
     h264parse ! rtph264pay ! udpsink host=$2 port=$3