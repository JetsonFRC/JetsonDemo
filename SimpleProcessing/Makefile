LIB_PATH=/3419/ntcore/
NT_INCLUDE_PATH=/3419/ntcore/include/
WPI_INCLUDE_PATH=/3419/ntcore/wpiutil/include/
LIBS=`pkg-config --cflags --libs opencv gstreamer-1.0` -lgstapp-1.0 -lgstriff-1.0 -lgstbase-1.0 -lgstvideo-1.0 -lgstpbutils-1.0 -lntcore -lwpiutil -lpthread
LOCAL_INCLUDES=-Iopencv-code/ -Iutility/ -Ivision-results/
CPP_FLAGS=-std=c++11 -Wall -Werror -g -L$(LIB_PATH) -I$(NT_INCLUDE_PATH) -I$(WPI_INCLUDE_PATH) ${LOCAL_INCLUDES}
OBJ=objects/main.o objects/cap_gstreamer.o objects/cam_helper.o objects/VisionResultsPackage.o

all: gstream_cv

run: gstream_cv
	./gstream_cv

objects/main.o: main.cpp opencv-code/* utility/*
	g++ -c $(CPP_FLAGS) main.cpp -o objects/main.o

objects/cam_helper.o: utility/cam_helper.cpp utility/cam_helper.hpp
	g++ -c $(CPP_FLAGS) utility/cam_helper.cpp -o objects/cam_helper.o

objects/cap_gstreamer.o: opencv-code/*
	g++ -c $(CPP_FLAGS) opencv-code/cap_gstreamer.cpp -o objects/cap_gstreamer.o

objects/VisionResultsPackage.o: vision-results/*
	g++ -c $(CPP_FLAGS) vision-results/VisionResultsPackage.cpp -o objects/VisionResultsPackage.o

#need to use libtool because of gstreamer libraries
gstream_cv: ${OBJ}
	libtool --mode=link g++ $(CPP_FLAGS) ${OBJ} ${LIBS} -o gstream_cv

clean:
	-rm gstream_cv
	-rm objects/*.o

