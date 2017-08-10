LIB_PATH=/3419/ntcore/
NT_INCLUDE_PATH=/3419/ntcore/include/
WPI_INCLUDE_PATH=/3419/ntcore/wpiutil/include/
LIBS=`pkg-config --cflags --libs opencv gstreamer-1.0` -lgstapp-1.0 -lgstriff-1.0 -lgstbase-1.0 -lgstvideo-1.0 -lgstpbutils-1.0 -lntcore -lwpiutil -lpthread

all: clean main.cpp cap_gstreamer.cpp
	libtool --mode=link g++ -std=c++11 -Wall -g -L$(LIB_PATH) -I$(NT_INCLUDE_PATH) -I$(WPI_INCLUDE_PATH) -Iopencv-code/ vision.cpp cap_gstreamer.cpp main.cpp ${LIBS} -o gstream_cv

run: all
	./gstream_cv

clean:
	-rm gstream_cv

