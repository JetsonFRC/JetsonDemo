#!/bin/bash
# GStreamer start up script kks2017
#ClientIPaddress=192.168.1.46
#ClientIPaddress=192.168.1.34
ClientIPaddress=10.34.19.5
# Test command to display video on local host
#gst-launch -v -e v4l2src device=/dev/video0 -v ! 'video/x-raw-yuv,width=320,height=240,framerate=30/1' ! ffmpegcolorspace ! autovideosink 

#gst-launch-1.0 -v -e v4l2src device=/dev/video0 -v !\
#    'video/x-raw-yuv,width=800,height=448,framerate=30/1' !\
#    omxh264enc bitrate=600000 !\
#    rtph264pay pt=96 ! udpsink host=$ClientIPaddress port=5805 -v

gst-launch-1.0 -v -e v4l2src device=/dev/video0 -v !\
     'video/x-raw, format=(string)I420, width=(int)800, height=(int)448' !\
     omxh264enc bitrate=600000  ! 'video/x-h264, stream-format=(string)byte-stream' !\
     h264parse ! rtph264pay ! udpsink host=$ClientIPaddress port=5805
