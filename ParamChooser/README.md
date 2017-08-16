# Param chooser example #

## Overview ##
Example project for modifying OpenCV and camera parameters over NetworkTables.

This project allows you to change your camera parameters and vision parameters 
while running your code, which makes it easier to fiddle until you have the 
right settings. Example use cases include finding correct exposure and 
brightness settings for your camera to isolate the target effectively, and 
choosing good HSV thresholds to make sure you isolate the target under a 
variety of angles and lighting conditions. 

## Program structure ##
The structure of this program is very similar to the ProcessingAndDrawing 
example, in that when you run the program it opens a gstreamer pipeline with two
parts, one that sends a raw image to your driver station laptop, and another 
that feeds the raw image into an OpenCV pipeline and sends back a modified 
version of the image. On every iteration the program grabs values from 
NetworkTables to set camera parameters such as exposure and brightness, and 
values related to the OpenCV pipeline such as thresholding parameters. Viewing 
the gstreamer streams on your laptop allows you to see what parameters give you 
the best thresolded image.

## Instructions ##

Under "network parameters" in main.cpp, enter the ip address of your 
driverstation computer. If you have not done so already, set a static ip address
on your driverstation computer.

Run the OutlineViewer program on your dashboard laptop in the directory 
wpilib/tools/OutlineViewer.jar so you can access NetworkTables.

After compiling and running this project, modify the values in the outline 
viewer to change camera and pipeline parameters, and view the affects of these 
changes with the gstreamer viewers on your laptop.
