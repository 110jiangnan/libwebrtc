@echo off
setlocal EnableExtensions
REM ============================================================
REM  build_win64.cmd — 构建 libwebrtc (release) 并复制到 flutter-webrtc
REM  产物: out-release/Windows-x64/libwebrtc.dll + libwebrtc.dll.lib
REM  目标: flutter-webrtc\third_party\libwebrtc\lib\win64
REM  用法: 双击运行, 或 build_win64.cmd
REM ============================================================

set "GN=E:\game\MyDesk\MyDesk\libwebrtc\depot_tools\gn"
set "NINJA=E:\home\qt\Tools\Ninja\ninja"
set "SRC=E:\game\MyDesk\MyDesk\libwebrtc\src\libwebrtc"
set "DEST=E:\game\MyDesk\MyDesk\flutter-webrtc\third_party\libwebrtc\lib\win64"

if not exist "%GN%"   (echo [ERROR] gn not found: %GN%      & exit /b 1)
if not exist "%NINJA%" (echo [ERROR] ninja not found: %NINJA% & exit /b 1)
if not exist "%SRC%"  (echo [ERROR] src not found: %SRC%    & exit /b 1)

pushd "%SRC%"

echo == [1/3] gn gen ==
"%GN%" gen out-release/Windows-x64 --args="target_os=\"win\" target_cpu=\"x64\" is_component_build=false is_clang=true is_debug=false rtc_use_h264=true proprietary_codecs=true ffmpeg_branding=\"Chrome\" rtc_include_tests=false rtc_build_examples=false libwebrtc_desktop_capture=true" --ide=vs2022
if errorlevel 1 (echo [ERROR] gn gen failed & popd & exit /b 1)

echo == [2/3] ninja build ==
"%NINJA%" -C out-release/Windows-x64 libwebrtc
if errorlevel 1 (echo [ERROR] ninja build failed & popd & exit /b 1)

echo == [3/3] copy to flutter-webrtc ==
if not exist "%DEST%" mkdir "%DEST%"
copy /y "out-release\Windows-x64\libwebrtc.dll"     "%DEST%\libwebrtc.dll"
copy /y "out-release\Windows-x64\libwebrtc.dll.lib" "%DEST%\libwebrtc.dll.lib"

popd
echo == done: %DEST% ==
endlocal
