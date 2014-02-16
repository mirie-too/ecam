#ifndef LIBECAM_FRAME_H_
#define LIBECAM_FRAME_H_

#include <ecam/config.h>

#include <stdint.h>

namespace ecam {

class Frame {
  public:
    struct Pixel {
        uint8_t r, g, b, a;
    };

    Frame(int width, int height);
    ~Frame();

    const Pixel *pixels() const { return m_data; }
    Pixel *pixels() { return m_data; }

    int width() const { return m_width; }
    int height() const { return m_height; }

    const uint8_t *data() const
    {
      return reinterpret_cast<const uint8_t *>(m_data);
    }
    uint8_t *data() { return reinterpret_cast<uint8_t *>(m_data); }
    int dataSize() const { return m_width * m_height * sizeof(Pixel); }

  private:
    int m_width, m_height;
    Pixel *m_data;
};

}

#endif
