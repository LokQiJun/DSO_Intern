cd ${BUILD_ROOT}/libusb/android/jni

export NDK=${TOOLCHAIN_ROOT}
${NDK}/ndk-build clean
${NDK}/ndk-build -B -r -R

cp ${BUILD_ROOT}/libusb/android/libs/arm64-v8a/* ${PREFIX}/lib
cp ${BUILD_ROOT}/libusb/libusb/libusb.h ${PREFIX}/include

