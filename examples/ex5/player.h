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
#ifndef PLAYER_H
#define PLAYER_H

#include <QLabel>

#include <ecam/decoder.h>

namespace ecam {

class Camera;

}

class Player : public QLabel, ecam::Decoder
{
    Q_OBJECT

  public:
    Player();
    ~Player();

    bool decode(const uint8_t *data, unsigned size, ecam::Frame &frame);

  signals:
    void frameCaptured(const QImage &frame);

  public slots:
    void showFrame(const QImage &frame);

  private:
    ecam::Camera *m_camera;
};

#endif // PLAYER_H
