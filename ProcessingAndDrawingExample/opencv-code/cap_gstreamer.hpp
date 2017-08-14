#ifndef CAP_GSTREAMER_HPP
#define CAP_GSTREAMER_HPP

#include "precomp.hpp"
#include <unistd.h>
#include <string.h>
#include <gst/gst.h>
#include <gst/gstbuffer.h>
#include <gst/video/video.h>
#include <gst/app/gstappsink.h>
#include <gst/app/gstappsrc.h>
#include <gst/riff/riff-media.h>
#include <gst/pbutils/missing-plugins.h>


#define VERSION_NUM(major, minor, micro) (major * 1000000 + minor * 1000 + micro)
#define FULL_GST_VERSION VERSION_NUM(GST_VERSION_MAJOR, GST_VERSION_MINOR, GST_VERSION_MICRO)

#if FULL_GST_VERSION >= VERSION_NUM(0,10,32)
#include <gst/pbutils/encoding-profile.h>
//#include <gst/base/gsttypefindhelper.h>
#endif


#ifdef NDEBUG
#define CV_WARN(message)
#else
#define CV_WARN(message) fprintf(stderr, "warning: %s (%s:%d)\n", message, __FILE__, __LINE__)
#endif

#if GST_VERSION_MAJOR == 0
#error This file has been modified to only support gstreamer-1.0
#endif

#define COLOR_ELEM "autovideoconvert"
#define COLOR_ELEM_NAME COLOR_ELEM

void toFraction(double decimal, double &numerator, double &denominator);
void handleMessage(GstElement * pipeline);


static cv::Mutex gst_initializer_mutex;

/*!
 * \brief The CvCapture_GStreamer class
 * Use GStreamer to capture video
 */
class CvCapture_GStreamer : public CvCapture
{
public:
    CvCapture_GStreamer() { init(); }
    virtual ~CvCapture_GStreamer() { close(); }

    virtual bool open( int type, const char* filename );
    virtual void close();

    virtual double getProperty(int);
    virtual bool setProperty(int, double);
    virtual bool grabFrame();
    virtual IplImage* retrieveFrame(int);

protected:
    void init();
    bool reopen();
    bool isPipelinePlaying();
    void startPipeline();
    void stopPipeline();
    void restartPipeline();
    void setFilter(const char* prop, int type, int v1, int v2 = 0);
    void removeFilter(const char *filter);
    static void newPad(GstElement *myelement,
                       GstPad     *pad,
                       gpointer    data);
    GstElement*   pipeline;
    GstElement*   uridecodebin;
    GstElement*   v4l2src;
    GstElement*   color;
    GstElement*   sink;

    GstSample*    sample;
    GstMapInfo*   info;

    GstBuffer*    buffer;
    GstCaps*      caps;
    IplImage*     frame;
    gint64        duration;
    gint          width;
    gint          height;
    double        fps;
};

/*!
 * \brief The CvVideoWriter_GStreamer class
 * Use Gstreamer to write video
 */
class CvVideoWriter_GStreamer : public CvVideoWriter
{
public:
    CvVideoWriter_GStreamer() { init(); }
    virtual ~CvVideoWriter_GStreamer() { close(); }

    virtual bool open( const char* filename, int fourcc,
                       double fps, CvSize frameSize, bool isColor );
    virtual void close();
    virtual bool writeFrame( const IplImage* image );
protected:
    void init();
    const char* filenameToMimetype(const char* filename);
    GstElement* pipeline;
    GstElement* source;
    GstElement* encodebin;
    GstElement* file;

    GstBuffer* buffer;
    int input_pix_fmt;
    int num_frames;
    double framerate;
};

#endif