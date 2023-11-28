cd ~/src/gnuradio-android/uhd_old/host
#git clean -xdf

rm -r build
mkdir build
cd build

cmake -DCMAKE_INSTALL_PREFIX=/home/android/src/gnuradio-android/toolchain/arm64-v8a \
  -DCMAKE_TOOLCHAIN_FILE=/home/android/Android/Sdk/ndk/20.0.5594570/build/cmake/android.toolchain.cmake \
  -DANDROID_ABI=arm64-v8a -DANDROID_ARM_NEON=ON \
  -DANDROID_NATIVE_API_LEVEL=29 \
  -DANDROID_STL=c++_shared \
  -DBOOST_ROOT=/home/android/src/gnuradio-android/toolchain/arm64-v8a \
  -DBoost_DEBUG=OFF \
  -DBoost_COMPILER=-clang \
  -DBoost_USE_STATIC_LIBS=ON \
  -DBoost_USE_DEBUG_LIBS=OFF \
  -DBoost_ARCHITECTURE=-a64 \
  -DENABLE_STATIC_LIBS=OFF \
  -DENABLE_EXAMPLES=OFF \
  -DENABLE_TESTS=OFF \
  -DENABLE_UTILS=ON \
  -DENABLE_PYTHON_API=OFF \
  -DENABLE_MANUAL=OFF \
  -DENABLE_DOXYGEN=OFF \
  -DENABLE_MAN_PAGES=ON \
  -DENABLE_OCTOCLOCK=OFF \
  -DENABLE_E300=OFF \
  -DENABLE_E320=OFF \
  -DENABLE_N300=OFF \
  -DENABLE_N320=OFF \
  -DENABLE_X300=ON \
  -DENABLE_USRP2=ON \
  -DENABLE_N230=OFF \
  -DENABLE_MPMD=OFF \
  -DENABLE_B100=Off \
  -DENABLE_B200=ON \
  -DENABLE_USRP1=OFF \
  -DCMAKE_FIND_ROOT_PATH=/home/android/src/gnuradio-android/toolchain/arm64-v8a \
  ../

make -j2

make install

