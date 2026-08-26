/**
 * @file rtc_desktop_capturer.h
 * This header file defines the interface for capturing desktop media.
 */

#ifndef LIB_WEBRTC_RTC_DESKTOP_CAPTURER_HXX
#define LIB_WEBRTC_RTC_DESKTOP_CAPTURER_HXX

#include "rtc_desktop_media_list.h"
#include "rtc_types.h"
#include "rtc_video_device.h"

namespace libwebrtc {

class DesktopCapturerObserver;

/**
 * @brief External frame consumer (MyDesk lock-screen wiring).
 *
 * Called by the Rust producer inside its frame lock on the desktop capture
 * thread, so the given ARGB pointer stays valid for the whole call. Must
 * consume the frame synchronously (here: I420 conversion + OnFrame).
 * `len` is the exact byte count handed over by the producer — the consumer
 * must NOT recompute it from w/h (the producer's buffer may not be tightly
 * packed), only use it to validate.
 */
typedef void (*ExternalFrameConsumer)(void* ud, const uint8_t* argb, int w,
                                      int h, int len);

/**
 * @brief External frame source callback (MyDesk lock-screen wiring).
 *
 * Called from the desktop capture thread on every capture tick when set.
 * If a lock-screen frame is available, invokes consume(ud, argb, w, h) inside
 * its frame lock and returns 1; returns 0 to fall through to normal capture.
 */
typedef int (*ExternalFrameCallback)(void* user_data,
                                     ExternalFrameConsumer consume, void* ud);

/**
 * @brief The interface for capturing desktop media.
 *
 * This interface defines methods for registering and deregistering observer
 * for desktop capture events, starting and stopping desktop capture, and
 * retrieving the current capture state and media source.
 */
class RTCDesktopCapturer : public RefCountInterface {
 public:
  /**
   * @brief Enumeration for the possible states of desktop capture.
   */
  enum CaptureState { CS_RUNNING, CS_STOPPED, CS_FAILED };

 public:
  /**
   * @brief Registers the given observer for desktop capture events.
   *
   * @param observer Pointer to the observer to be registered.
   */
  virtual void RegisterDesktopCapturerObserver(
      DesktopCapturerObserver* observer) = 0;

  /**
   * @brief Deregisters the currently registered desktop capture observer.
   */
  virtual void DeRegisterDesktopCapturerObserver() = 0;

  /**
   * @brief Starts desktop capture with the given frame rate.
   *
   * @param fps The desired frame rate.
   *
   * @return The current capture state after attempting to start capture.
   */
  virtual CaptureState Start(uint32_t fps) = 0;

  /**
   * @brief Starts desktop capture with the given frame rate and capture
   *        dimensions.
   *
   * @param fps The desired frame rate.
   * @param x The left-most pixel coordinate of the capture region.
   * @param y The top-most pixel coordinate of the capture region.
   * @param w The width of the capture region.
   * @param h The height of the capture region.
   *
   * @return The current capture state after attempting to start capture.
   */
  virtual CaptureState Start(uint32_t fps, uint32_t x, uint32_t y, uint32_t w,
                             uint32_t h) = 0;

  /**
   * @brief Stops desktop capture.
   */
  virtual void Stop() = 0;

  /**
   * @brief Checks if desktop capture is currently running.
   *
   * @return True if capture is running, false otherwise.
   */
  virtual bool IsRunning() = 0;

  /**
   * @brief Sets an external frame source that replaces the normal desktop
   *        capture while set (e.g. lock-screen GDI frames).
   *
   * @param cb Callback invoked on every capture tick; may be nullptr to
   *           disable. If it returns a frame, that frame is fed into the
   *           video pipeline instead of the normal capture.
   * @param user_data Opaque pointer passed back to the callback.
   */
  virtual void SetExternalFrameCallback(ExternalFrameCallback cb,
                                        void* user_data) = 0;

  /**
   * @brief Clears the external frame source, restoring normal capture.
   */
  virtual void ClearExternalFrameCallback() = 0;

  /**
   * @brief Retrieves the media source for the current desktop capture.
   *
   * @return A scoped_refptr<MediaSource> representing the current capture
   *         media source.
   */
  virtual scoped_refptr<MediaSource> source() = 0;

  /**
   * @brief Destroys the RTCDesktopCapturer object.
   */
  virtual ~RTCDesktopCapturer() {}
};

/**
 * @brief Observer interface for desktop capturer events.
 *
 * This class defines the interface for an observer of the DesktopCapturer
 * class, allowing clients to be notified of events such as when capturing
 * begins or ends, and when an error occurs.
 */
class DesktopCapturerObserver {
 public:
  /**
   * @brief Called when desktop capture starts.
   *
   * @param capturer A reference to the capturer that started capturing.
   */
  virtual void OnStart(scoped_refptr<RTCDesktopCapturer> capturer) = 0;

  /**
   * @brief Called when desktop capture is paused.
   *
   * @param capturer A reference to the capturer that paused capturing.
   */
  virtual void OnPaused(scoped_refptr<RTCDesktopCapturer> capturer) = 0;

  /**
   * @brief Called when desktop capture stops.
   *
   * @param capturer A reference to the capturer that stopped capturing.
   */
  virtual void OnStop(scoped_refptr<RTCDesktopCapturer> capturer) = 0;

  /**
   * @brief Called when an error occurs during desktop capture.
   *
   * @param capturer A reference to the capturer that encountered an error.
   */
  virtual void OnError(scoped_refptr<RTCDesktopCapturer> capturer) = 0;

 protected:
  ~DesktopCapturerObserver() {}
};

}  // namespace libwebrtc

#endif  // LIB_WEBRTC_RTC_DESKTOP_CAPTURER_HXX
