set -xe

#############################################################
### CONFIG
#############################################################
export TOOLCHAIN_ROOT=${HOME}/Android/Sdk/ndk/20.0.5594570
export HOST_ARCH=linux-x86_64

#############################################################
### DERIVED CONFIG
#############################################################
export SYS_ROOT=${TOOLCHAIN_ROOT}/sysroot
export TOOLCHAIN_BIN=${TOOLCHAIN_ROOT}/toolchains/llvm/prebuilt/${HOST_ARCH}/bin
export API_LEVEL=29
export CC="${TOOLCHAIN_BIN}/aarch64-linux-android${API_LEVEL}-clang"
export CXX="${TOOLCHAIN_BIN}/aarch64-linux-android${API_LEVEL}-clang++"
export LD=${TOOLCHAIN_BIN}/aarch64-linux-android-ld
export AR=${TOOLCHAIN_BIN}/aarch64-linux-android-ar
export RANLIB=${TOOLCHAIN_BIN}/aarch64-linux-android-ranlib
export STRIP=${TOOLCHAIN_BIN}/aarch64-linux-android-strip
export BUILD_ROOT=$(dirname $(readlink -f "$0"))
export PATH=${TOOLCHAIN_BIN}:${PATH}
export PREFIX=${BUILD_ROOT}/toolchain/arm64-v8a
export PKG_CONFIG_PATH=${PREFIX}/lib/pkgconfig
export NCORES=$(getconf _NPROCESSORS_ONLN)

mkdir -p ${PREFIX}


cd ${BUILD_ROOT}
git clone git@github.com:myriadrf/LimeSuite.git
cd ${BUILD_ROOT}/LimeSuite
rm -r build && mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=${PREFIX} \
  -DCMAKE_TOOLCHAIN_FILE=${TOOLCHAIN_ROOT}/build/cmake/android.toolchain.cmake \
  -DANDROID_ABI=arm64-v8a -DANDROID_ARM_NEON=ON \
  -DANDROID_NATIVE_API_LEVEL=${API_LEVEL} \
  -DANDROID_STL=c++_shared \
  -DCMAKE_FIND_ROOT_PATH=${PREFIX} \
   -DLIBUSB_1_LIBRARY=${PREFIX}/lib/libusb1.0.so \
   -DLIBUSB_1_INCLUDE_DIRS=${PREFIX}/include \
   -DENABLE_LIME_UTIL=OFF \
   -DENABLE_QUICKTEST=OFF \
   -DENABLE_MCU_TESTBENCH=OFF \ 
   -DENABLE_EXAMPLES=OFF \
  ../

make -j ${NCORES}
make install