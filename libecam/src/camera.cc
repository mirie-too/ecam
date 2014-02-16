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
#include <ecam/camera.h>

#include <ecam/decoder.h>
#include <ecam/processor.h>

namespace ecam {

void Camera::run(void *that)
{
  reinterpret_cast<Camera *>(that)->run();
}

Camera::~Camera()
{
  stop();

  if (m_autoDeleteDecoder && m_decoder != NULL)
    delete m_decoder;
}

bool Camera::play()
{
  if (!isRunning()) {
    m_isRunning = true;
    m_thread = new thread(run, this);
  }
  return true;
}

bool Camera::stop()
{
  m_mutex.lock();
  if (isRunning()) {
    m_isRunning = false;
    m_mutex.unlock();
    m_thread->join();
    delete m_thread;
    m_thread = NULL;
  } else {
    m_mutex.unlock();
  }
  return true;
}

void Camera::setDecoder(Decoder *decoder, bool autoDelete)
{
  if (m_autoDeleteDecoder && m_decoder != NULL)
    delete m_decoder;
  m_decoder = decoder;
  m_autoDeleteDecoder = autoDelete;
}


void Camera::processFrame(const uint8_t *data, unsigned size)
{
  if (m_decoder == NULL)
    return;

  if (!m_decoder->decode(data, size, m_frame))
    return;
}


}
