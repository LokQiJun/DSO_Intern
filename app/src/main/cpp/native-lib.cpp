#include <jni.h>
#include <string>
#include <iostream>
#include <sstream>
#include <stdlib.h>

// Get any GNU Radio headers
#include <gnuradio/constants.h>
#include <gnuradio/blocks/null_source.h>
#include <gnuradio/blocks/null_sink.h>
#include <gnuradio/blocks/ctrlport_probe2_f.h>
#include <gnuradio/fft/ctrlport_probe_psd.h>

#include <gnuradio/top_block.h>
#include <gnuradio/blocks/add_const_ff.h>
#include <gnuradio/blocks/float_to_complex.h>
#include <gnuradio/blocks/multiply_const.h>
#include <gnuradio/blocks/null_source.h>
#include <gnuradio/blocks/repeat.h>
#include <gnuradio/blocks/wavfile_source.h>
#include <gnuradio/filter/firdes.h>
#include <gnuradio/filter/interp_fir_filter.h>
#include <gnuradio/filter/fir_filter_blk.h>

#include <limesdr/source.h>
#include <limesdr/sink.h>

#include <gnuradio/prefs.h>
#include <gnuradio/sys_paths.h>
#include <gnuradio/logger.h>

#include <libusb.h>
gr::top_block_sptr tb;


extern "C"
JNIEXPORT jstring JNICALL
Java_net_bastibl_fmrx_MainActivity_fgInit(JNIEnv * env, jobject thiz, int fd, jstring usbfsPath) {

    setenv("VOLK_CONFIGPATH", getenv("EXTERNAL_STORAGE"), 1);

    const char *usbfs_path = env->GetStringUTFChars(usbfsPath, NULL);

    tb = gr::make_top_block("AM mod demod");


// Variables:
    double sdr_rate = 2e6;
    double samp_rate = 48e3;
    int decim = 16;
    double center_freq = 100e6;
    float audio_gain = 1.0;
    try {
// Blocks:

    gr::limesdr::sink::sptr sdrSink = gr::limesdr::sink::make("0009072C00D51D1F", 0, "", "", fd); //serial="0009072C00D51D1F"
        sdrSink->set_sample_rate(sdr_rate);
        sdrSink->set_center_freq(center_freq, 0);
        sdrSink->set_gain(10, 0);
        sdrSink->set_antenna(0, 0);
        sdrSink->calibrate(2.5e6, 0);

    gr::blocks::wavfile_source::sptr wavSource = gr::blocks::wavfile_source::make("/storage/emulated/0/wavfiles/input.wav", false);
    gr::blocks::repeat::sptr repeat = gr::blocks::repeat::make(sizeof(float)*1, decim);;
    gr::blocks::null_source::sptr nullSource = gr::blocks::null_source::make(sizeof(float)*1);;
    gr::blocks::multiply_const_ff::sptr multiplyGain = gr::blocks::multiply_const_ff::make(audio_gain);;
    gr::blocks::float_to_complex::sptr floatComplex = gr::blocks::float_to_complex::make(1);;
    gr::blocks::add_const_ff::sptr addConst = gr::blocks::add_const_ff::make(1);
    gr::filter::fir_filter_ccf::sptr lpf = gr::filter::fir_filter_ccf::make(
            1,
            gr::filter::firdes::low_pass(
                    1,
                    samp_rate,
                    12e3,
                    2e3,
                    gr::filter::firdes::WIN_HAMMING,
                    6.76));


// Connections:
        tb->connect(wavSource, 0, multiplyGain, 0);
        tb->connect(multiplyGain, 0, addConst, 0);
        tb->connect(addConst, 0, repeat, 0);
        tb->connect(repeat, 0, floatComplex, 0);
        tb->connect(nullSource, 0, floatComplex, 1);
        tb->connect(floatComplex, 0, lpf, 0);
        tb->connect(lpf, 0, sdrSink, 0);

//    GR_DEBUG("gnuradio", "constructed flowgraph");
    } catch (const std::exception &e) {
        return env->NewStringUTF(e.what());
    }

    return env->NewStringUTF("None");
}

extern "C"
JNIEXPORT jstring JNICALL
Java_net_bastibl_fmrx_MainActivity_fgStart(JNIEnv * env, jobject thiz, jstring tmpName) {

    const char *tmp_c;
    tmp_c = env->GetStringUTFChars(tmpName, NULL);
    setenv("TMP", tmp_c, 1);

//    GR_DEBUG("gnuradio", "JNI starting flowgraph");
    try {
        tb->start();
        tb->wait();
    } catch (const std::exception &e) {
        return env->NewStringUTF(e.what());
    }

    return env->NewStringUTF("None");
}

extern "C"
JNIEXPORT jstring JNICALL
Java_net_bastibl_fmrx_MainActivity_fgStop(JNIEnv * env, jobject thiz) {
    try {
        tb->stop();
        tb->wait();
    } catch (const std::exception &e) {
        return env->NewStringUTF(e.what());
    }

    return env->NewStringUTF("None");
}

extern "C"
JNIEXPORT jstring JNICALL
Java_net_bastibl_fmrx_MainActivity_checkUSB(JNIEnv *env, jobject thiz)
{ 
    libusb_context *ctx; 
    int r = libusb_init(&ctx);
    if(r < 0) return env->NewStringUTF((std::to_string(r) + "Libusb init error").c_str());

 
    libusb_device **list; 
    int count = static_cast<int>(libusb_get_device_list(ctx, &list));
    if (count < 0) {
        libusb_exit(ctx); 
        return env->NewStringUTF((std::to_string(r) + "Failed to get USB device list").c_str());
    } else if (count == 0) {
        libusb_free_device_list(list, 1); 
        libusb_exit(ctx);
        return env->NewStringUTF("Empty USB device list"); 
    }
 
    std::string usbInfo;

    for (int i = 0; i < count; ++i) { 
        libusb_device *device = list[i]; 
        libusb_device_descriptor desc; 
        int r = libusb_get_device_descriptor(device, &desc); 
        if (r < 0) { 
            usbInfo += std::to_string(r) + ": Failed to get device descriptor for Index: " + std::to_string(i) + "\n"; 
            continue; 
 
        } 
        usbInfo += "Vendor ID: 0x" + std::to_string(desc.idVendor) + " Product ID: 0x" + std::to_string(desc.idProduct) + "\n"; 

        libusb_device_handle *tempDev_handle(nullptr);
        if(libusb_open(list[i], &tempDev_handle) != 0 || tempDev_handle == nullptr) {
            usbInfo += "Unable to open above USB device\n";
            continue;
        }
    } 
 
    libusb_free_device_list(list, 1); 
    libusb_exit(ctx); 

    return env->NewStringUTF(usbInfo.c_str());
}

// from https://github.com/libusb/libusb/blob/master/android/README
extern "C"
JNIEXPORT jstring JNICALL
Java_net_bastibl_fmrx_MainActivity_checkUSBTwo(JNIEnv *env, jobject thiz, jint fileDescriptor)
{ 
    //Setting up and init
    libusb_context *ctx;
//	libusb_set_option(&ctx, LIBUSB_OPTION_WEAK_AUTHORITY, NULL);
    int r = libusb_init(&ctx);
    if(r < 0) return env->NewStringUTF((std::to_string(r) + ": Libusb init error").c_str());

    //Wrapping an existing system device
    libusb_device_handle *devh;
    r = libusb_wrap_sys_device(NULL, (intptr_t)fileDescriptor, &devh);
    if(r < 0) {
        libusb_exit(ctx);
        return env->NewStringUTF((std::to_string(r) + ": Error wrapping system device").c_str());
    }

    std::string usbInfo;
    char data[255];
    try {
        //read device name
        r = libusb_get_string_descriptor_ascii(devh, LIBUSB_CLASS_COMM, (unsigned char*)data, sizeof(data));
        if(r > 0) usbInfo += "Name: " + std::string(data, size_t(r)) + "\n";
        else usbInfo += std::to_string(r) + ": Error getting device name\n";

        //Opening device from the handle
        libusb_device *dev = libusb_get_device(devh);
        if (dev) {
            //Check operating speed
            int speed = libusb_get_device_speed(dev);
            if(speed == LIBUSB_SPEED_HIGH)
                usbInfo += "Media: USB 2.0\n";
            else if(speed == LIBUSB_SPEED_SUPER)
                usbInfo += "Media: USB 3.0\n";
            else
                usbInfo += "Media: USB\n";

            //Get device descriptor
            struct libusb_device_descriptor desc;
            r = libusb_get_device_descriptor(dev, &desc);
            if (r < 0) {
                usbInfo += std::to_string(r) + ": Error getting device descriptor\n";
            } else {
                //Read device addr
                int pid = desc.idProduct;
                int vid = desc.idVendor;
                r = std::sprintf(data, "%.4x:%.4x", int(vid), int(pid));
                if (r > 0) usbInfo += "Addr: " + std::string(data, size_t(r)) + "\n";
                else usbInfo += std::to_string(r) + ": Error getting device addr\n";

                //Read serial number
                if (desc.iSerialNumber > 0)
                {
                    r = libusb_get_string_descriptor_ascii(devh, desc.iSerialNumber,(unsigned char*)data, sizeof(data));
                    if (r < 0) usbInfo += "Failed to get serial number\n";
                    else usbInfo += "Serial Num: " + std::string(data, size_t(r)) + "\n";
                }
            }
        } else {
            usbInfo += "Error Opening device from device handler";
        }

        //Release resources
        libusb_close(devh);
	libusb_exit(ctx);
    } catch (const std::exception &e) {
        usbInfo += e.what();
    }

    return env->NewStringUTF(usbInfo.c_str());
}
