#ifndef LIBECAM_CAPABILITIES_H_
#define LIBECAM_CAPABILITIES_H_

#include <ecam/config.h>

#include <vector>

namespace ecam {

using std::vector;

class Capabilities {
    friend class LocalCamera;

  public:
    Capabilities()
      : m_isCamera(false),
        m_numInputs(0)
    {}

    const string &name() const { return m_name; }

    const string &driver() const { return m_driver; }

    const string &bus() const { return m_bus; }

    bool isCamera() const { return m_isCamera; }

    int numInputs() const { return m_numInputs; }

    int numPixelFormats() const { return m_pixelFormats.size(); }

    const string &pixelFormat(int i) const
    {
      assert(i >= 0 && i < numPixelFormats());
      return m_pixelFormats[i];
    }

  private:
    void clear();

    bool m_isCamera;
    string m_name;
    string m_driver;
    string m_bus;
    int m_numInputs;
    vector<string> m_pixelFormats;
};

}

#endif
