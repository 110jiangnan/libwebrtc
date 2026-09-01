
#!/bin/sh
if [ ! -n "$1" ]; then
  echo "Usage: $0 'debug' | 'release'"
  exit 0
fi

MODE=$1
OUT_DIR=./out-android-$MODE
OUT_DIR1=out-android-$MODE
DEBUG="false"
if [ "$MODE" == "debug" ]; then
  DEBUG="true"
fi

echo "android.sh: MODE=$MODE, DEBUG=$DEBUG"

# WebRTC 提供了脚本直接生成 AAR
# 构建完整 AAR（包含所有 ABI）
python3 ./tools_webrtc/android/build_aar.py \
    --build-dir $OUT_DIR1 \
    --output $OUT_DIR1/libwebrtc.aar \
    --extra-gn-args 'treat_warnings_as_errors=false' \
    --arch arm64-v8a armeabi-v7a x86_64