
#!/bin/sh
if [ ! -n "$1" ]; then
  echo "Usage: $0 'debug' | 'release'"
  exit 0
fi

MODE=$1
OUT_DIR=./out-$MODE
DEBUG="false"
if [ "$MODE" == "debug" ]; then
  DEBUG="true"
fi

# x86	32 位	老旧 PC、工控机	❌ 已淘汰（消费端）
# x64	64 位	现代 Windows/Linux PC、服务器	✅ 桌面/服务器主流
# ARM	32 位	老安卓手机、树莓派 1/2、IoT	⚠️ 嵌入式仍有使用
# ARM64	64 位	iPhone、Android 旗舰、Mac M 系列、树莓派 4/5、云服务器	✅ 移动/新兴计算主流
# export ARCH=x64 # x86, x64, arm, arm64

# 预先
# python3 build/linux/sysroot_scripts/install-sysroot.py --arch=arm
# python3 build/linux/sysroot_scripts/install-sysroot.py --arch=arm64

gn gen $OUT_DIR/Linux-x64 --args="
    target_os=\"linux\"
    target_cpu=\"x64\"
    is_debug=$DEBUG
    rtc_include_tests=false
    rtc_use_h264=true
    rtc_libvpx_build_vp9 = true
    enable_libaom=true
    ffmpeg_branding=\"Chrome\"
    is_component_build=false
    use_rtti=true
    use_custom_libcxx=false
    rtc_enable_protobuf=false"

ninja -C $OUT_DIR/Linux-x64 libwebrtc

gn gen $OUT_DIR/Linux-x86 --args="
    target_os=\"linux\"
    target_cpu=\"x86\"
    is_debug=$DEBUG
    rtc_include_tests=false
    rtc_use_h264=true
    rtc_libvpx_build_vp9=true
    enable_libaom=true
    ffmpeg_branding=\"Chrome\"
    is_component_build=false
    use_rtti=true
    use_custom_libcxx=false
    rtc_enable_protobuf=false"

ninja -C $OUT_DIR/Linux-x86 libwebrtc

gn gen $OUT_DIR/Linux-arm --args="
    target_os=\"linux\"
    target_cpu=\"arm\"
    is_debug=$DEBUG
    rtc_include_tests=false
    rtc_use_h264=true
    rtc_libvpx_build_vp9=true
    enable_libaom=true
    ffmpeg_branding=\"Chrome\"
    is_component_build=false
    use_rtti=true
    use_custom_libcxx=false
    rtc_enable_protobuf=false"

ninja -C $OUT_DIR/Linux-arm libwebrtc

gn gen $OUT_DIR/Linux-arm64 --args="
    target_os=\"linux\"
    target_cpu=\"arm64\"
    is_debug=$DEBUG
    rtc_include_tests=false 
    rtc_use_h264=true
    rtc_libvpx_build_vp9=true
    enable_libaom=true
    ffmpeg_branding=\"Chrome\"
    is_component_build=false
    use_rtti=true
    use_custom_libcxx=false
    rtc_enable_protobuf=false"

ninja -C $OUT_DIR/Linux-arm64 libwebrtc

mkdir -p $OUT_DIR/linux-lib/linux-arm64
mkdir -p $OUT_DIR/linux-lib/linux-arm
mkdir -p $OUT_DIR/linux-lib/linux-x86
mkdir -p $OUT_DIR/linux-lib/linux-x64

cp $OUT_DIR/Linux-arm64/libwebrtc.so $OUT_DIR/linux-lib/linux-arm64/
cp $OUT_DIR/Linux-arm/libwebrtc.so $OUT_DIR/linux-lib/linux-arm/
cp $OUT_DIR/Linux-x86/libwebrtc.so $OUT_DIR/linux-lib/linux-x86/
cp $OUT_DIR/Linux-x64/libwebrtc.so $OUT_DIR/linux-lib/linux-x64/

cp -r ./libwebrtc/include $OUT_DIR/linux-lib/include

