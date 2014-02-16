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
#include "player.h"

#include <ecam/localcamera.h>

Player::Player()
  : m_camera(NULL)
{
  QString title("Easy Camera (version %1) usage example #5");
  setWindowTitle(title.arg(ecam::version().c_str()));
  setAlignment(Qt::AlignCenter);

  std::vector<std::string> devices = ecam::LocalCamera::devices();
  if (!devices.empty()) {
    std::string device = devices.front();

    ecam::LocalCamera *localCamera = new ecam::LocalCamera();
    localCamera->setDeviceName(device);
    localCamera->setIoMethod(ecam::LocalCamera::MMAP);
    localCamera->setDecoder(this, false);

    if (localCamera->open() && localCamera->play()) {
      m_camera = localCamera;
      connect(this, SIGNAL(frameCaptured(QImage)), SLOT(showFrame(QImage)));
    } else {
      delete localCamera;
    }
  }

  if (m_camera != NULL) {
    setGeometry(100, 100, m_camera->frameWidth(), m_camera->frameHeight());
  } else {
    setGeometry(100, 100, 640, 480);
    setText("No cameras found!");
  }
}

Player::~Player()
{
  if (m_camera != NULL)
    delete m_camera;
}

bool Player::decode(const uint8_t *data, unsigned size, ecam::Frame &frame)
{
  const uint8_t *src = data;
  uint8_t *dst = frame.data();

  for (int i = 0; i < size; i += 4, src += 4) {
    double Y1 = static_cast<double>(src[0]);
    double cb = static_cast<double>(src[1]) - 128.0;
    double Y2 = static_cast<double>(src[2]);
    double cr = static_cast<double>(src[3]) - 128.0;

    uint8_t r1 = uint8_t(Y1 + 1.402 * cr);
    uint8_t g1 = uint8_t(Y1 - 0.344 * cb - 0.714 * cr);
    uint8_t b1 = uint8_t(Y1 + 1.772 * cb);
    uint8_t r2 = uint8_t(Y2 + 1.402 * cr);
    uint8_t g2 = uint8_t(Y2 - 0.344 * cb - 0.714 * cr);
    uint8_t b2 = uint8_t(Y2 + 1.772 * cb);

    int j = i << 1;
    dst[j + 3] = 0xFF;
    dst[j + 2] = r1;
    dst[j + 1] = g1;
    dst[j + 0] = b1;
    dst[j + 7] = 0xFF;
    dst[j + 6] = r2;
    dst[j + 5] = g2;
    dst[j + 4] = b2;
  }

  QImage image(frame.data(), frame.width(), frame.height(),
               QImage::Format_ARGB32);
  emit frameCaptured(image);

  return true;
}

void Player::showFrame(const QImage &frame)
{
  setPixmap(QPixmap::fromImage(frame));
}
