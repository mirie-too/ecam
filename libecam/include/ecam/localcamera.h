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
#ifndef LIBECAM_LOCALCAMERA_H_
#define LIBECAM_LOCALCAMERA_H_

#include <ecam/config.h>

#include <vector>

#include <ecam/camera.h>

namespace ecam {

using std::vector;

/**
 * The LocalCamera class controls local capturing devices connected via USB, PCI
 * buses etc. Only cameras which are visible to V4L2 (Linux) and DirectShow
 * (Windows) are supported.
 */
class LocalCamera : public Camera
{
  public:
    /**
     * This enum is used to cpecify the I/O mode to be used. In the most cases
     * MMAP is an excellent choise here.
     */
    enum IOMethod { READ, MMAP };

    /**
     * @return List of all available local video devices.
     *         Quering capabilities is needed to extract cameras only.
     */
    static vector<string> devices();

    LocalCamera();

    /**
     * @return Name of the camera in platform-dependant form.
     */
    const string &deviceName() const { return m_deviceName; }

    /**
     * @return Name of the camera casted to const char *.
     */
    const char *c_deviceName() const { return m_deviceName.c_str(); }

    /**
     * Sets a new camera device. Camera must be closed.
     * @param deviceName
     */
    void setDeviceName(const string &deviceName);

    /**
     * @return I/O method currently used.
     */
    IOMethod ioMethod() const { return m_ioMethod; }

    /**
     * Sets a new I/O method. Camera must be closed.
     * @param ioMethod
     */
    void setIoMethod(IOMethod ioMethod);

    virtual bool open();
    virtual bool close();

    virtual bool queryCapabilities(Capabilities &caps);

    virtual bool play();
    virtual bool stop();

  private:
    typedef vector<Buffer> Buffers;

    virtual void run();

    /**
     * @internal
     * Shortcut for ::ioctl()
     * @param request
     * @param argument
     * @return
     */
    int ioctl(int request, void *argument);

    bool openDevice();
    bool initDevice();
    bool initMmap();
    bool initRead(unsigned size);

    bool startCapturing();
    bool stopCapturing();

    bool uninitDevice();
    bool closeDevice();

    bool captureFrame();

    string m_deviceName;
    IOMethod m_ioMethod;
    int m_file;
    Buffers m_buffers;
};

}

#endif
