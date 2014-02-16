#include <ecam/frame.h>

namespace ecam {

Frame::Frame(int width, int height)
  : m_width(width),
    m_height(height),
    m_data(NULL)
{
  if (m_width > 0 && m_height > 0)
    m_data = new Pixel[m_width * m_height];
}

Frame::~Frame()
{
  if (m_data != NULL)
    delete[] m_data;
}

}
