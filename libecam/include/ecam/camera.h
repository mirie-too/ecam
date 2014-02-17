/**************************************************************************
 Copyright (c) 2014, Sergey Kulishenko <serkul(at)ukr(dot)net>

 Permission to use, copy, modify, and/or distribute this software for any
 purpose with or without fee is hereby granted, provided that the above
 copyright notice and this permission notice appear in all copies.

 THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
**************************************************************************/
#ifndef LIBECAM_CAMERA_H_
#define LIBECAM_CAMERA_H_

#include <ecam/config.h>

#include <ecam/capabilities.h>
#include <ecam/frame.h>
#include <ecam/thread.h>

namespace ecam {

class Decoder;
class Processor;

/**
 * The Camera class serves as a general video capture interface. It allows to
 * control common capturing tasks (open and close device, start and stop
 * playback,set frame size, query capabilities etc).
 */
class Camera {
  public:
    virtual ~Camera();

    /**
     * @return current frame width.
     */
    int frameWidth() const { return m_frame.width(); }

    /**
     * @return Current frame height.
     */
    int frameHeight() const { return m_frame.height(); }

    /**
     * Opens the device and prepare it for capturing.
     * @return true if no errors occured.
     */
    virtual bool open() = 0;

    /**
     * Closes the device, stops capturing if it is active.
     * @return true if no errors occured.
     */
    virtual bool close() = 0;

    /**
     * Queries the device capabilities (see Capabilities for details). Please
     * note, it's not always possible to query the capabilities exactly,
     * especially for remote cameras. Device must be open.
     * @return true if no error occured.
     */
    virtual bool queryCapabilities(Capabilities &caps) = 0;

    /**
     * Starts capturing in a dedicated thread. Device must be open.
     * @return true if no error occured.
     */
    virtual bool play();

    /**
     * Stops capturing. Device must be open.
     * @return true if no error occured.
     */
    virtual bool stop();

    /**
     * @return true if device is capturing video.
     */
    bool isRunning() const
    {
      lock_guard<mutex>(m_mutex);
      return m_isRunning;
    }

    /**
     * @return Decoder currently set to device, can be NULL.
     *         See Decoder for details.
     */
    Decoder *decoder() { return m_decoder; }

    /**
     * Sets the new video decoder, can be NULL. Old decoder is deleted if
     * autoDeleteDecoder() is true.
     * @param decoder New decoder to set.
     * @param autoDelete Allow auto delete of new decoder.
     */
    void setDecoder(Decoder *decoder, bool autoDelete);

    bool autoDeleteDecoder() const { return m_autoDeleteDecoder; }

  protected:
    /**
     * @internal
     * Just a wrapper for memory buffer, used internally for raw frame data.
     */
    struct Buffer {
        Buffer()
          : data(NULL),
            size(0)
        {}

        uint8_t *data;
        int size;
    };

    /**
     * Constructs a new camera object with 640x480 frame size.
     */
    Camera()
      : m_thread(NULL),
        m_isRunning(false),
        m_decoder(NULL),
        m_autoDeleteDecoder(true),
        m_frame(640, 480)
    {}

    /**
     * @internal
     * Called from teh capturing thread to process frame data (decode, apply
     * processor etc).
     * @param data raw frame data
     * @param size data size in bytes
     */
    void processFrame(const uint8_t *data, unsigned size);

  private:
    static void run(void *that);

    /**
     * Main video capture function, runs in a separate thread.
     */
    virtual void run() = 0;

    thread *m_thread;
    bool m_isRunning;
    mutable mutex m_mutex;
    Decoder *m_decoder;
    bool m_autoDeleteDecoder;
    Frame m_frame;
};

}

#endif
