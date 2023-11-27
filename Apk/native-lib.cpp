#include <jni.h>
#include <string>
#include <iostream>
#include <sstream>

// Get any GNU Radio headers
#include <gnuradio/constants.h>
#include <gnuradio/prefs.h>
#include <gnuradio/sys_paths.h>
#include <gnuradio/logger.h>
#include <gnuradio/top_block.h>
#include <gnuradio/blocks/null_source.h>
#include <gnuradio/blocks/throttle.h>
#include <gnuradio/blocks/null_sink.h>
#include <gnuradio/blocks/ctrlport_probe2_f.h>
#include <gnuradio/fft/ctrlport_probe_psd.h>
#include <gnuradio/blocks/float_to_complex.h>
#include <gnuradio/uhd/usrp_source.h>
#include <libhackrf/hackrf.h>
#include <gnuradio/analog/sig_source.h>
#include <osmosdr/source.h>
#include <grand/opensl_sink.h>

#include <gnuradio/filter/firdes.h>
#include <gnuradio/filter/fir_filter_blk.h>
#include <gnuradio/analog/quadrature_demod_cf.h>
#include <gnuradio/analog/fastnoise_source.h>
#include <gnuradio/blocks/ctrlport_probe2_f.h>
#include <gnuradio/filter/pfb_arb_resampler_ccf.h>
#include <gnuradio/filter/freq_xlating_fir_filter.h>
#include <stdlib.h>

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
#include <gnuradio/uhd/usrp_sink.h>
#include <gnuradio/uhd/usrp_source.h>

#include <gnuradio/prefs.h>
#include <gnuradio/sys_paths.h>
#include <gnuradio/logger.h>

gr::top_block_sptr tb;


extern "C"
JNIEXPORT jobject JNICALL
        Java_net_bastibl_fmrx_MainActivity_fgInit(JNIEnv * env, jobject thiz, int fd, jstring usbfsPath) {

    tb = gr::make_top_block("AM mod demod");



// Variables:
    int samp_rate = 480000;
    double gain = 0.5;
    unsigned int frequency = 48000;

// Blocks:

//    gr::blocks::throttle::sptr limiter = gr::blocks::throttle::make(sizeof(gr_complex) * 1, samp_rate * 100, true);

    gr::blocks::sub_ff::sptr subNoise = gr::blocks::sub_ff::make(1);
    gr::blocks::multiply_ff::sptr multiplierModulation = gr::blocks::multiply_ff::make(1);
    gr::blocks::float_to_complex::sptr convertFloatComplex = gr::blocks::float_to_complex::make(1);
    gr::blocks::complex_to_mag::sptr convertComplexMag = gr::blocks::complex_to_mag::make(1);
    gr::blocks::add_ff::sptr adderNoise = gr::blocks::add_ff::make(1);
    gr::blocks::add_ff::sptr adderModulation = gr::blocks::add_ff::make(1);
    gr::analog::sig_source_f::sptr carrierSignal = gr::analog::sig_source_f::make(samp_rate, gr::analog::GR_COS_WAVE, (frequency * 10), 2, 0,0);
    gr::analog::sig_source_f::sptr constNoise = gr::analog::sig_source_f::make(0, gr::analog::GR_CONST_WAVE, 0, 0, static_cast<float>(2 * gain));
    gr::analog::sig_source_f::sptr constImaginary = gr::analog::sig_source_f::make(0, gr::analog::GR_CONST_WAVE, 0, 0, 0);
    gr::blocks::wavfile_source::sptr wavSource = gr::blocks::wavfile_source::make("/storage/emulated/0/wavfiles/input.wav", false);
    gr::blocks::wavfile_sink::sptr wavSink = gr::blocks::wavfile_sink::make("/storage/emulated/0/wavfiles/output.wav", 1, frequency);
    gr::filter::interp_fir_filter_fff::sptr lpf = gr::filter::interp_fir_filter_fff::make(
            1,
            gr::filter::firdes::low_pass(
                    gain,
                    samp_rate,
                    (frequency * 1.5),
                    2000,
                    gr::filter::firdes::WIN_HAMMING,
                    6.76));

 Connections:
    //send
    tb->connect(wavSource, 0, multiplierModulation, 0);
    tb->connect(carrierSignal, 0, multiplierModulation, 1);
    tb->connect(multiplierModulation, 0, adderModulation, 0);
    tb->connect(carrierSignal, 0, adderModulation, 1);
    tb->connect(adderModulation, 0, convertFloatComplex, 0);
    tb->connect(constImaginary, 0, convertFloatComplex, 1);
//    tb->connect(convertFloatComplex, 0, sdrSink, 0);
    //receive
//    tb->connect(sdrSource, 0, convertComplexMag, 0);
    tb->connect(convertFloatComplex, 0, convertComplexMag, 0);
//    tb->connect(convertFloatComplex, 0, limiter, 0);
//    tb->connect(limiter, 0, convertComplexMag, 0);
    tb->connect(convertComplexMag, 0, lpf, 0);
    tb->connect(lpf, 0, subNoise, 0);
    tb->connect(constNoise, 0, subNoise, 1);
    tb->connect(subNoise, 0, wavSink, 0);

    return nullptr;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_net_bastibl_fmrx_MainActivity_fgStart(JNIEnv * env, jobject thiz, jstring tmpName) {

    const char *tmp_c;
    tmp_c = env->GetStringUTFChars(tmpName, NULL);
    setenv("TMP", tmp_c, 1);

    tb->start();
    tb->wait();

    return nullptr;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_net_bastibl_fmrx_MainActivity_fgStop(JNIEnv * env, jobject thiz) {
    tb->stop();
    tb->wait();

    return nullptr;
}