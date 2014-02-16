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

class Camera {
  public:
    virtual ~Camera();

    int frameWidth() const { return m_frame.width(); }
    int frameHeight() const { return m_frame.height(); }

    virtual bool open() = 0;
    virtual bool close() = 0;

    virtual bool queryCapabilities(Capabilities &caps) = 0;

    virtual bool play();

    virtual bool stop();

    bool isRunning() const
    {
      lock_guard<mutex>(m_mutex);
      return m_isRunning;
    }

    Decoder *decoder() { return m_decoder; }

    void setDecoder(Decoder *decoder, bool autoDelete);

    bool autoDeleteDecoder() const { return m_autoDeleteDecoder; }

  protected:
    struct Buffer {
        Buffer()
          : data(NULL),
            size(0)
        {}

        uint8_t *data;
        int size;
    };

    Camera()
      : m_thread(NULL),
        m_isRunning(false),
        m_decoder(NULL),
        m_autoDeleteDecoder(true),
        m_frame(640, 480)
    {}

    void processFrame(const uint8_t *data, unsigned size);

  private:
    static void run(void *that);

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
