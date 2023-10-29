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
#include <gnuradio/analog/sig_source.h>
#include <gnuradio/blocks/add_blk.h>
#include <gnuradio/blocks/complex_to_mag.h>
#include <gnuradio/blocks/float_to_complex.h>
#include <gnuradio/blocks/multiply.h>
#include <gnuradio/blocks/sub.h>
#include <gnuradio/blocks/wavfile_sink.h>
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
//    setenv("GR_CONF_CONTROLPORT_ON", "true", 1);


//    const char *usbfs_path = env->GetStringUTFChars(usbfsPath, NULL);

    tb = gr::make_top_block("AM mod demod");

//    std::stringstream args;
//    args << "bbl=foo,type=b200,fd=" << fd << ",usbfs_path=" << usbfs_path;
//    GR_INFO("fg", boost::str(boost::format("Using UHD args=%1%") % args.str()));

// Variables:
    int samp_rate = 480000;
    double gain = 0.5;
    int frequency = 48000;
    try {
// Blocks:
//    gr::limesdr::source::sptr sdrSource = gr::limesdr::source::make("0009072C00D51D1F", 1, "", false);
//        sdrSource->set_sample_rate(samp_rate);
//        sdrSource->set_center_freq(100e6, 1);
//        sdrSource->set_bandwidth(1.5e6, 1);
//        sdrSource->set_digital_filter(samp_rate, 1);
//        sdrSource->set_gain(10, 1);
//        sdrSource->set_antenna(0, 1);
//        sdrSource->calibrate(2.5e6, 1);

    gr::limesdr::sink::sptr sdrSink = gr::limesdr::sink::make("", 0, "", ""); //serial="0009072C00D51D1F"
        sdrSink->set_sample_rate(samp_rate);
        sdrSink->set_center_freq(100e6, 0);
        sdrSink->set_bandwidth(5e6, 0);
        sdrSink->set_digital_filter(samp_rate, 0);
        sdrSink->set_gain(10, 0);
        sdrSink->set_antenna(0, 0);
        sdrSink->calibrate(2.5e6, 0);


//    gr::blocks::sub_ff::sptr subNoise = gr::blocks::sub_ff::make(1);
//    gr::blocks::multiply_ff::sptr multiplierModulation = gr::blocks::multiply_ff::make(1);
//    gr::blocks::float_to_complex::sptr convertFloatComplex = gr::blocks::float_to_complex::make(1);
//    gr::blocks::complex_to_mag::sptr convertComplexMag = gr::blocks::complex_to_mag::make(1);
//    gr::blocks::add_ff::sptr adderNoise = gr::blocks::add_ff::make(1);
//    gr::blocks::add_ff::sptr adderModulation = gr::blocks::add_ff::make(1);
//    gr::analog::sig_source_f::sptr carrierSignal = gr::analog::sig_source_f::make(samp_rate, gr::analog::GR_COS_WAVE, (frequency * 10), 2, 0,0);
//    gr::analog::sig_source_f::sptr constNoise = gr::analog::sig_source_f::make(0, gr::analog::GR_CONST_WAVE, 0, 0, static_cast<float>(2 * gain));
//    gr::analog::sig_source_f::sptr constImaginary = gr::analog::sig_source_f::make(0, gr::analog::GR_CONST_WAVE, 0, 0, 0);
//    gr::blocks::wavfile_source::sptr wavSource = gr::blocks::wavfile_source::make("/storage/emulated/0/wavfiles/input.wav", false);
//    gr::blocks::wavfile_sink::sptr wavSink = gr::blocks::wavfile_sink::make("/storage/emulated/0/wavfiles/output.wav", 1, frequency );
//    gr::filter::interp_fir_filter_fff::sptr lpf = gr::filter::interp_fir_filter_fff::make(
//            1,
//            gr::filter::firdes::low_pass(
//                    gain,
//                    samp_rate,
//                    (frequency * 1.5),
//                    2000,
//                    gr::filter::firdes::WIN_HAMMING,
//                    6.76));

// Connections:
    //send
//    tb->connect(wavSource, 0, multiplierModulation, 0);
//    tb->connect(carrierSignal, 0, multiplierModulation, 1);
//    tb->connect(multiplierModulation, 0, adderModulation, 0);
//    tb->connect(carrierSignal, 0, adderModulation, 1);
//    tb->connect(adderModulation, 0, convertFloatComplex, 0);
//    tb->connect(constImaginary, 0, convertFloatComplex, 1);
//    tb->connect(convertFloatComplex, 0, sdrSink, 0);
    //receive
//    tb->connect(sdrSource, 0, convertComplexMag, 0);
//    tb->connect(convertComplexMag, 0, lpf, 0);
//    tb->connect(lpf, 0, subNoise, 0);
//    tb->connect(constNoise, 0, subNoise, 1);
//    tb->connect(subNoise, 0, wavSink, 0);

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
JNIEXPORT jobject JNICALL
Java_net_bastibl_fmrx_MainActivity_fgStop(JNIEnv * env, jobject thiz) {
    tb->stop();
    tb->wait();

    return nullptr;
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
