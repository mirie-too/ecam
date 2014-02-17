#ifndef LIBECAM_FRAME_H_
#define LIBECAM_FRAME_H_

#include <ecam/config.h>

#include <stdint.h>

namespace ecam {

/**
 * The Frame class is used for storing decoded to RGB32 video frames. This class
 * do not use any deep copying, so be careful with modofocations to its content.
 *
 * @todo Make this non-copyable?
 */
class Frame {
  public:
    /**
     * Represents a single pixel in the frame. Components are red, green, blue,
     * alpha in order.
     */
    struct Pixel {
        uint8_t r, g, b, a;
    };

    /**
     * Constructs a new Frame object and allocates the memory.
     * @param width frame width
     * @param height frame height
     */
    Frame(int width, int height);
    ~Frame();

    /**
     * @return Pointer to the actual pixels, stored by rows.
     */
    const Pixel *pixels() const { return m_data; }
    Pixel *pixels() { return m_data; }

    /**
     * @return Frame width in pixels.
     */
    int width() const { return m_width; }

    /**
     * @return Frame height in pixels.
     */
    int height() const { return m_height; }

    /**
     * @return Pointer to the actual data converted to uint8_t *.
     */
    const uint8_t *data() const
    {
      return reinterpret_cast<const uint8_t *>(m_data);
    }
    uint8_t *data() { return reinterpret_cast<uint8_t *>(m_data); }

    /**
     * @return Number of bytes used.
     */
    int dataSize() const { return m_width * m_height * sizeof(Pixel); }

  private:
    int m_width, m_height;
    Pixel *m_data;
};

}

#endif
