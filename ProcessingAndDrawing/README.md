# Processing and drawing example #

## Overview ##
Example project for performing OpenCV processing to an image, sending back 
visual feedback as well as results.

This program looks like something one might use in competition; it includes 
a code for processing an image to gather relevant data from it such as the 
position of a target, and additionally sends back two camera streams with a raw
and modified image for debugging and visual feedback. You may also use OpenCV to
draw on the image for more advanced visual feedback (e.g rectangle around the 
target) 

## Program structure ##
When you run the program it opens a gstreamer pipeline with two
parts, one that sends a raw image to your driver station laptop, and another 
that feeds the raw image into an OpenCV pipeline and sends back a modified 
version of the image. Raw camera images are passed to the "calculate" method in
vision.cpp, which performs an OpenCV pipeline and stores the results in a 
VisionResultsPackage struct. Data from this struct is logged locally and sent
over the network in main.cpp. A modified image from the OpenCV pipeline 
(possibly drawn on, thresholded, etc) is sent over gstreamer to the driver 
station laptop after calculation.

Camera settings must be done in a weird way; the script good_settings.sh 
_should_ set the correct settings, but in our experience some cameras do not get 
flashed correctly by just running this script in isolation. Instead, we set 
incorrect settings first using bad_settings.sh and then later set the correct 
settings; this seemed to work more consistently. We do not know why just using
good_settings.sh does not work consistently and suspect it may be a firmware 
issue on some cameras.

Note: The timestamp sent back may not be synchronized with the rio time 
if your carrier board does not have a battery powered clock. If you need to know 
the delay time between the time an image was grabbed and the time its data was 
recieved, it may be necessary to add your own time synchronization procedure or 
send over a delay as well as an absolute time. 

Note: One flaw with the current setup is that whenever new code is flashed to
the RoboRio, the Jetson's connection to the Rio over NetworkTables will break
and not automatically reconnect. This means that when writing code on the 
RoboRio to make use of the Jetson's vision information, you must re-run the
code each time you flash new code to the roborio, which can be slightly 
annoying. We were not able to include this in the guide, but we recommend 
modifying our code to either automatically reconnect to NetworkTables if you
are capable and choose that method of communication. Alternatively, our team has
had success in the past using direct UDP communication between the RoboRio and 
Jetson which bypasses this problem entirely, and is faster. UDP also has the 
property of sending and recieving dicrete packets rather than updating 
key-value pairs, which is arguably a better (if slightly more difficult to use)
model of communication for this use case. If you are comfortable using UDP 
that is certainly a good alternative to NetworkTables.

## Instructions ##

Under "network parameters" in main.cpp, enter the ip address of your 
driverstation computer. If you have not done so already, set a static ip address
on your driverstation computer. 

Edit vision.cpp and vision.hpp to modify the vision pipeline, you can keep the 
current structure if you wish. See the SimpleProcessing example for a more 
advanced VisionResultsPackage datastructure you may use. You should use the 
ParamChooser project to determine good camera parameters and thresolding 
parameters before launching this project. 

Add drawing to your calculate function to better debug the vision pipeline and 
have robust visual feedback. You can also put variables about the robot on 
NetworkTables from the rio, read them from the Jetson, and draw them from the 
calculate function.