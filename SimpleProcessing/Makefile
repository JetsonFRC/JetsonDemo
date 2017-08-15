LIB_PATH=/3419/ntcore/
NT_INCLUDE_PATH=/3419/ntcore/include/
WPI_INCLUDE_PATH=/3419/ntcore/wpiutil/include/
LIBS=`pkg-config --cflags --libs opencv gstreamer-1.0` -lgstapp-1.0 -lgstriff-1.0 -lgstbase-1.0 -lgstvideo-1.0 -lgstpbutils-1.0 -lntcore -lwpiutil -lpthread
CPP_FLAGS=-std=c++11 -Wall -Werror -g -L$(LIB_PATH) -I$(NT_INCLUDE_PATH) -I$(WPI_INCLUDE_PATH) -Iopencv-code/ -Icamera-utility/

all: objects/main.o objects/cap_gstreamer.o objects/cam_helper.o
	libtool --mode=link g++ $(CPP_FLAGS) objects/*.o ${LIBS} -o gstream_cv

run: all
	./gstream_cv

objects/main.o: main.cpp camera-utility/cam_helper.hpp opencv-code/*.hpp helper.hpp gst_pipeline.hpp
	g++ -c $(CPP_FLAGS) main.cpp -o objects/main.o

objects/cam_helper.o: camera-utility/*.cpp camera-utility/*.hpp helper.hpp
	g++ -c $(CPP_FLAGS) camera-utility/cam_helper.cpp -o objects/cam_helper.o

objects/cap_gstreamer.o: opencv-code/*.cpp opencv-code/*.hpp
	g++ -c $(CPP_FLAGS) opencv-code/cap_gstreamer.cpp -o objects/cap_gstreamer.o

clean:
	-rm gstream_cv
	-rm objects/*.o

