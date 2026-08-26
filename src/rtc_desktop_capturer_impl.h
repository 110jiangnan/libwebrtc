/*
 * Copyright 2022 LiveKit
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef LIBWEBRTC_RTC_DESKTOP_CAPTURER_IMPL_HXX
#define LIBWEBRTC_RTC_DESKTOP_CAPTURER_IMPL_HXX

#include <mutex>

#include "api/video/i420_buffer.h"
#include "api/video/video_frame.h"
#include "include/rtc_desktop_capturer.h"
#include "include/rtc_types.h"
#include "modules/desktop_capture/desktop_and_cursor_composer.h"
#include "modules/desktop_capture/desktop_capture_options.h"
#include "modules/desktop_capture/desktop_capturer.h"
#include "modules/desktop_capture/desktop_frame.h"
#include "rtc_base/thread.h"
#include "src/internal/vcm_capturer.h"
#include "src/internal/video_capturer.h"

namespace libwebrtc {

class RTCDesktopCapturerImpl : public RTCDesktopCapturer,
                               public webrtc::DesktopCapturer::Callback,
                               public webrtc::internal::VideoCapturer {
 public:
  RTCDesktopCapturerImpl(DesktopType type,
                         webrtc::DesktopCapturer::SourceId source_id,
                         webrtc::Thread* signaling_thread,
                         scoped_refptr<MediaSource> source, bool showCursor = true);
  ~RTCDesktopCapturerImpl();

  void RegisterDesktopCapturerObserver(
      DesktopCapturerObserver* observer) override {
    observer_ = observer;
  }

  void DeRegisterDesktopCapturerObserver() override { observer_ = nullptr; }
  CaptureState Start(uint32_t fps) override;

  CaptureState Start(uint32_t fps, uint32_t x, uint32_t y, uint32_t w,
                     uint32_t h) override;

  void Stop() override;

  bool IsRunning() override;

  void SetExternalFrameCallback(ExternalFrameCallback cb,
                                void* user_data) override {
    std::lock_guard<std::mutex> lock(external_mutex_);
    external_cb_ = cb;
    external_user_data_ = user_data;
  }

  void ClearExternalFrameCallback() override {
    std::lock_guard<std::mutex> lock(external_mutex_);
    external_cb_ = nullptr;
  }

  scoped_refptr<MediaSource> source() override { return source_; }

 protected:
  virtual void OnCaptureResult(
      webrtc::DesktopCapturer::Result result,
      std::unique_ptr<webrtc::DesktopFrame> frame) override;

 private:
  void CaptureFrame();
  // 把外部回调给的裸帧按与 OnCaptureResult 相同的方式转 I420 并 OnFrame。
  void ProcessExternalFrame(int width, int height, const uint8_t* data,
                            size_t len);
  // Rust 帧锁内同步调用的消费入口(静态, 作为函数指针传给外部帧回调):
  // 直接转 I420 + OnFrame, 见 CaptureFrame。
  static void ConsumeExternalFrame(void* ud, const uint8_t* data, int w, int h,
                                   int len);
  webrtc::DesktopCaptureOptions options_;
  std::unique_ptr<webrtc::DesktopCapturer> capturer_;
  std::unique_ptr<webrtc::Thread> thread_;
  webrtc::scoped_refptr<webrtc::I420Buffer> i420_buffer_;
  CaptureState capture_state_ = CS_STOPPED;
  DesktopType type_;
  webrtc::DesktopCapturer::SourceId source_id_;
  DesktopCapturerObserver* observer_ = nullptr;
  uint32_t capture_delay_ = 1000;  // 1s
  webrtc::DesktopCapturer::Result result_ =
      webrtc::DesktopCapturer::Result::SUCCESS;
  webrtc::Thread* signaling_thread_ = nullptr;
  scoped_refptr<MediaSource> source_;
  uint32_t x_ = 0;
  uint32_t y_ = 0;
  uint32_t w_ = 0;
  uint32_t h_ = 0;
  // 外部帧来源(锁屏 GDI 帧)。设置后 capture 循环优先用它, 不采 DXGI。
  // 由插件线程写、采集线程每帧读, 用互斥锁防数据竞争。
  std::mutex external_mutex_;
  ExternalFrameCallback external_cb_ = nullptr;
  void* external_user_data_ = nullptr;
};

}  // namespace libwebrtc

#endif  // LIBWEBRTC_RTC_DESKTOP_CAPTURER_IMPL_HXX
