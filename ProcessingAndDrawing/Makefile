GST_INC    := -I/usr/include/gstreamer-1.0 -I/usr/include/glib-2.0 -I/usr/lib/aarch64-linux-gnu/glib-2.0/include -I/usr/lib/aarch64-linux-gnu/gstreamer-1.0/include
LOCAL_INC  := -Isrc/opencv-code/ -Isrc/utility/
NTCORE_INC := -I/3419/ntcore/include/ -I/3419/ntcore/wpiutil/include/
INC        := ${NTCORE_INC} ${LOCAL_INC} ${GST_INC}

LIBS       := -L/3419/ntcore/ `pkg-config --cflags --libs opencv gstreamer-1.0` -lgstapp-1.0 -lgstriff-1.0 -lgstbase-1.0 -lgstvideo-1.0 -lgstpbutils-1.0 -lntcore -lwpiutil -lpthread

SOURCES    := $(shell find src -type f -name *.cpp)
OBJECTS    := $(patsubst src/%,obj/%,$(SOURCES:.cpp=.o))

CPP_ARGS  := -std=c++11 -Wall -Werror -g

all: obj gstream_cv

obj:
	@mkdir -p obj/

run: gstream_cv
	./gstream_cv

obj/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	g++ $(CPP_ARGS) $(INC) -c -o $@ $<
	@g++ $(CPP_ARGS) ${INC} -MM src/$*.cpp > obj/$*.d
	@cp -f obj/$*.d obj/$*.d.tmp
	@sed -e 's|.*:|obj/$*.o:|' < obj/$*.d.tmp > obj/$*.d
	@sed -e 's/.*://' -e 's/\\$$//' < obj/$*.d.tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> obj/$*.d
	@rm -f obj/$*.d.tmp

#need to use libtool because of gstreamer libraries
gstream_cv: ${OBJECTS}
	libtool --mode=link g++ $(CPP_ARGS) ${INC} ${LIBS} -o gstream_cv ${OBJECTS}


clean:
	-rm gstream_cv
	-rm -rf obj

.PHONY: clean run
