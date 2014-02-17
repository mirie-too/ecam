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
#ifndef LIBECAM_DECODER_H_
#define LIBECAM_DECODER_H_

#include <ecam/config.h>

namespace ecam {

class Frame;

/**
 * The Decoder class provides an abstract interface for video decoders.
 */
class Decoder {
  public:
    virtual ~Decoder() {}

    /**
     * Performs actual decoding of frame data to RGB32 buffer.
     * @param data raw frame data
     * @param size frame data size in bytes
     * @param frame Frame object to be filled in with RGB32.
     * @return treu if no errors occured.
     */
    virtual bool decode(const uint8_t *data, unsigned size, Frame &frame) = 0;
};

}

#endif
