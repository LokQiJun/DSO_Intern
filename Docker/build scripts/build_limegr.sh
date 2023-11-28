cd gr-limesdr

rm -r build && mkdir build && cd build

cmake -DCMAKE_INSTALL_PREFIX=/home/android/src/gnuradio-android/toolchain/arm64-v8a \
 -DCMAKE_TOOLCHAIN_FILE=/home/android/Android/Sdk/ndk/20.0.5594570/build/cmake/android.toolchain.cmake \
 -DANDROID_ABI=arm64-v8a \
 -DANDROID_ARM_NEON=ON \
 -DANDROID_STL=c++_shared \
 -DANDROID_NATIVE_API_LEVEL=29 \
 -DPYTHON_EXECUTABLE=/usr/bin/python3 \
 -DBOOST_ROOT=/home/android/src/gnuradio-android/toolchain/arm64-v8a \
 -DBoost_DEBUG=OFF \
 -DBoost_COMPILER=-clang \
 -DBoost_USE_STATIC_LIBS=ON \
 -DBoost_USE_DEBUG_LIBS=OFF \
 -DBoost_ARCHITECTURE=-a64 \
 -DCMAKE_FIND_ROOT_PATH=/home/android/src/gnuradio-android/toolchain/arm64-v8a \
 -DENABLE_DOXYGEN=OFF \
 -DENABLE_PYTHON=OFF \
 ../

make -j2

make install

