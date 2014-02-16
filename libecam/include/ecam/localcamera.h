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

class LocalCamera : public Camera
{
  public:
    enum IOMethod { READ, MMAP };

    static vector<string> devices();

    LocalCamera();

    const string &deviceName() const { return m_deviceName; }
    const char *c_deviceName() const { return m_deviceName.c_str(); }
    void setDeviceName(const string &deviceName);

    IOMethod ioMethod() const { return m_ioMethod; }
    void setIoMethod(IOMethod ioMethod);

    virtual bool open();
    virtual bool close();

    virtual bool queryCapabilities(Capabilities &caps);

    virtual bool play();
    virtual bool stop();

  private:
    typedef vector<Buffer> Buffers;

    virtual void run();

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
