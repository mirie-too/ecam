#ifndef LIBECAM_CAPABILITIES_H_
#define LIBECAM_CAPABILITIES_H_

#include <ecam/config.h>

#include <vector>

namespace ecam {

using std::vector;

/**
 * The Capabilities class holds information describing the camera. Not all
 * Capabilities fields are supported by all cameras, so do not treat this
 * information as an absolute truth.
 */
class Capabilities {
    friend class LocalCamera;

  public:
    /**
     * Constructs an invalid Capabilities object.
     */
    Capabilities()
      : m_isCamera(false),
        m_numInputs(0)
    {}

    /**
     * @return Camera name in human-readable form.
     */
    const string &name() const { return m_name; }

    /**
     * @return Name of the driver the camera is using.
     */
    const string &driver() const { return m_driver; }

    /**
     * @return Name of the interface bus the camera is connected to.
     */
    const string &bus() const { return m_bus; }

    /**
     * @return true if the device supports video capturing.
     */
    bool isCamera() const { return m_isCamera; }

    /**
     * @return Number of available video inputs.
     */
    int numInputs() const { return m_numInputs; }

    /**
     * @return Number of available pixel formats.
     */
    int numPixelFormats() const { return m_pixelFormats.size(); }

    /**
     * Returns the name of pixel format in Video 4 Linux form: string of four
     * letter, for example, 'YUYV', "JPEG', "MJPG' etc. Reffer to V4L2
     * documentation for details.
     * @param i Pixel format index
     * @return Pixel format name
     */
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
