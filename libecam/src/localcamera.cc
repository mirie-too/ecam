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
#include <ecam/localcamera.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <linux/videodev2.h>
\
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

#define CLEAR(x) memset(&(x), 0, sizeof(x))

namespace ecam {

static const char kDevRoot[] = "/dev/";

/**
 * @brief Filter function for scandir().
 *
 * See man 3 scandir for details.
 *
 * @todo Implement better handling of symlinks.
 */
static int videofilter(const struct dirent *d)
{
  bool hasValidName = (strncmp(d->d_name, "video", 5) == 0);
  bool hasValidType = (d->d_type == DT_CHR || d->d_type == DT_LNK);
  return  (hasValidName && hasValidType) ? 1 : 0;
}

vector<string> LocalCamera::devices()
{
  vector<string> dd;

  dirent **namelist;
  int numDevices = scandir(kDevRoot, &namelist, videofilter, alphasort);
  if (numDevices > 0) {
    string devRoot(kDevRoot);
    for (int i = 0; i < numDevices; i++) {
      dd.push_back(devRoot + namelist[i]->d_name);
      free(namelist[i]);
    }
    free(namelist);
  } else if (numDevices < 0) {
    perror("scandir()");
  }

  return dd;
}

LocalCamera::LocalCamera() :
  m_ioMethod(MMAP),
  m_file(-1)
{
}

void LocalCamera::setDeviceName(const string &deviceName)
{
  if (m_file > 0) {
    fprintf(stderr, "%s: Device is open!\n", __PRETTY_FUNCTION__);
    return;
  }

  m_deviceName = deviceName;
}

void LocalCamera::setIoMethod(IOMethod ioMethod)
{
  if (m_file > 0) {
    fprintf(stderr, "%s: Device is open!\n", __PRETTY_FUNCTION__);
    return;
  }

  m_ioMethod = ioMethod;
}

bool LocalCamera::open()
{
  if (m_file > 0)
    return true;

  if (!openDevice())
    return false;

  if (!initDevice()) {
    closeDevice();
    return false;
  }

  return true;
}

bool LocalCamera::close()
{
  if (m_file == -1)
    return true;

  return uninitDevice() && closeDevice();
}

bool LocalCamera::queryCapabilities(Capabilities &caps)
{
  v4l2_capability cap;
  if (ioctl(VIDIOC_QUERYCAP, &cap) == -1) {
    fprintf(stderr, "%s: Cannot query capabilities: %d (%s)!\n",
            m_deviceName.c_str(), errno, strerror(errno));
    return false;
  }

  caps.m_isCamera = ((cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) != 0);
  if (!caps.m_isCamera)
    return true;

  caps.m_name = reinterpret_cast<const char *>(cap.card);
  caps.m_driver = reinterpret_cast<const char *>(cap.driver);
  caps.m_bus = reinterpret_cast<const char *>(cap.bus_info);

  v4l2_input input;
  input.index = 0;
  while (ioctl(VIDIOC_ENUMINPUT, &input) == 0)
    input.index++;

  caps.m_numInputs = input.index;

  vector<string> pixelFormats;
  v4l2_fmtdesc fmtdesc;
  memset(&fmtdesc, 0, sizeof(fmtdesc));
  fmtdesc.index = 0;
  fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  while (ioctl(VIDIOC_ENUM_FMT, &fmtdesc) == 0) {
    char tmp[5];
    tmp[0] = fmtdesc.pixelformat & 0xFF;
    tmp[1] = (fmtdesc.pixelformat >> 8) & 0xFF;
    tmp[2] = (fmtdesc.pixelformat >> 16) & 0xFF;
    tmp[3] = (fmtdesc.pixelformat >> 24) & 0xFF;
    tmp[4] = '\0';
    pixelFormats.push_back(tmp);
    fmtdesc.index++;
  }

  caps.m_pixelFormats = pixelFormats;

  return true;
}

bool LocalCamera::play()
{
  return startCapturing() ? Camera::play() : false;
}

bool LocalCamera::stop()
{
  return Camera::stop() ? stopCapturing() : false;
}

int LocalCamera::ioctl(int request, void *argument)
{
  assert(m_file > 0);

  int retVal;
  do {
    retVal = ::ioctl(m_file, request, argument);
  } while (retVal == -1 && errno == EINTR);
  return retVal;
}

bool LocalCamera::openDevice()
{
  struct stat st;

  if (stat(c_deviceName(), &st) == -1) {
    fprintf(stderr, "Cannot identify '%s': %d, %s\n",
            c_deviceName(), errno, strerror(errno));
    return false;
  }

  if (!S_ISCHR(st.st_mode)) {
    fprintf(stderr, "%s is no device\n", c_deviceName());
    return false;
  }

  m_file = ::open(c_deviceName(), O_RDWR | O_NONBLOCK, 0);

  if (m_file == -1) {
    fprintf(stderr, "Cannot open '%s': %d, %s\n",
            c_deviceName(), errno, strerror(errno));
    return false;
  }

  return true;
}

bool LocalCamera::initDevice()
{
  struct v4l2_capability cap;
  struct v4l2_cropcap cropcap;
  struct v4l2_crop crop;
  struct v4l2_format fmt;

  if (ioctl(VIDIOC_QUERYCAP, &cap) == -1) {
    if (EINVAL == errno)
      fprintf(stderr, "%s is no V4L2 device\n", c_deviceName());
    else
      perror("VIDIOC_QUERYCAP");
    return false;
  }

  if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
    fprintf(stderr, "%s is no video capture device\n", c_deviceName());
    return false;
  }

  switch (m_ioMethod) {
  case READ:
    if (!(cap.capabilities & V4L2_CAP_READWRITE)) {
      fprintf(stderr, "%s does not support read i/o\n", c_deviceName());
      return false;
    }
    break;

  case MMAP:
    if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
      fprintf(stderr, "%s does not support streaming i/o\n",
              c_deviceName());
      return false;
    }
    break;
  }

  /* Select video input, video standard and tune here. */

  CLEAR(cropcap);

  cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

  if (ioctl(VIDIOC_CROPCAP, &cropcap) == 0) {
    crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    crop.c = cropcap.defrect; /* reset to default */

    if (ioctl(VIDIOC_S_CROP, &crop) == -1) {
      switch (errno) {
      case EINVAL:
        /* Cropping not supported. */
        break;
      default:
        /* Errors ignored. */
        break;
      }
    }
  } else {
    /* Errors ignored. */
  }


  CLEAR(fmt);

  /**
     * @todo rework setting of input format
     * */
  fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (ioctl(VIDIOC_G_FMT, &fmt) == -1) {
    perror("VIDIOC_G_FMT");
    return false;
  }

  fmt.fmt.pix.width = frameWidth();
  fmt.fmt.pix.height = frameHeight();
  fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
  fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

  if (ioctl(VIDIOC_S_FMT, &fmt) == -1) {
    perror("VIDIOC_S_FMT");
    return false;
  }

  /* Note VIDIOC_S_FMT may change width and height. */


  if (ioctl(VIDIOC_G_FMT, &fmt) == -1) {
    perror("VIDIOC_G_FMT");
    return false;
  }

  /* Buggy driver paranoia. */
  unsigned min;
  min = fmt.fmt.pix.width * 2;
  if (fmt.fmt.pix.bytesperline < min)
    fmt.fmt.pix.bytesperline = min;
  min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
  if (fmt.fmt.pix.sizeimage < min)
    fmt.fmt.pix.sizeimage = min;

  bool ok = false;
  switch (m_ioMethod) {
  case READ:
    ok = initRead(fmt.fmt.pix.sizeimage);
    break;

  case MMAP:
    ok = initMmap();
    break;
  }

  if (!ok)
    uninitDevice();

  return ok;
}

bool LocalCamera::initMmap()
{
  struct v4l2_requestbuffers req;

  CLEAR(req);

  req.count = 4;
  req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  req.memory = V4L2_MEMORY_MMAP;

  if (ioctl(VIDIOC_REQBUFS, &req) == -1) {
    if (EINVAL == errno) {
      fprintf(stderr, "%s does not support memory mapping\n",
              c_deviceName());
      return false;
    } else {
      perror("VIDIOC_REQBUFS");
      return false;
    }
  }

  if (req.count < 2) {
    fprintf(stderr, "Insufficient buffer memory on %s\n", c_deviceName());
    return false;
  }

  m_buffers.resize(req.count);
  /**
      * @todo user shrink_to_fit in c++11 mode
      */

  if (m_buffers.empty()) {
    fprintf(stderr, "Out of memory\n");
    return false;
  }

  for (unsigned i = 0; i < m_buffers.size(); ++i) {
    struct v4l2_buffer buf;

    CLEAR(buf);

    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = i;

    if (ioctl(VIDIOC_QUERYBUF, &buf) == -1) {
      perror("VIDIOC_QUERYBUF");
      return false;
    }

    m_buffers[i].size = buf.length;
    m_buffers[i].data = reinterpret_cast<uint8_t *>
                        (mmap(NULL, buf.length, PROT_READ | PROT_WRITE,
                              MAP_SHARED, m_file, buf.m.offset));

    if (m_buffers[i].data == MAP_FAILED) {
      perror("mmap");
      return false;
    }
  }

  return true;
}

bool LocalCamera::initRead(unsigned size)
{
  m_buffers.resize(1);
  /**
      * @todo user shrink_to_fit in c++11 mode
      */

  if (m_buffers.empty()) {
    fprintf(stderr, "Out of memory\n");
    return false;
  }

  m_buffers[0].size = size;
  m_buffers[0].data = reinterpret_cast<uint8_t *>(malloc(size));

  if (!m_buffers[0].data) {
    fprintf(stderr, "Out of memory\n");
    return false;
  }

  return true;
}

bool LocalCamera::startCapturing()
{
  enum v4l2_buf_type type;

  switch (m_ioMethod) {
  case READ:
    /* Nothing to do. */
    break;

  case MMAP:
    for (unsigned i = 0; i < m_buffers.size(); ++i) {
      struct v4l2_buffer buf;

      CLEAR(buf);
      buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      buf.memory = V4L2_MEMORY_MMAP;
      buf.index = i;

      if (ioctl(VIDIOC_QBUF, &buf) == -1) {
        perror("VIDIOC_QBUF");
        return false;
      }
    }
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(VIDIOC_STREAMON, &type) == -1) {
      perror("VIDIOC_STREAMON");
      return false;
    }
    break;
  }

  return true;
}

bool LocalCamera::stopCapturing()
{
  enum v4l2_buf_type type;

  switch (m_ioMethod) {
  case READ:
    /* Nothing to do. */
    break;

  case MMAP:
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(VIDIOC_STREAMOFF, &type) == -1) {
      perror("VIDIOC_STREAMOFF");
      return false;
    }
    break;
  }

  return true;
}

bool LocalCamera::uninitDevice()
{
  switch (m_ioMethod) {
  case READ:
    free(m_buffers[0].data);
    break;

  case MMAP:
    for (unsigned i = 0; i < m_buffers.size(); i++) {
      if (m_buffers[i].data == NULL && m_buffers[i].data != MAP_FAILED) {
        if (munmap(m_buffers[i].data, m_buffers[i].size) == -1) {
          perror("munmap");
          return false;
        }
      }
    }
    break;
  }

  m_buffers.clear();
  return true;
}

bool LocalCamera::closeDevice()
{
  if (::close(m_file) == -1) {
    perror("close");
    return false;
  }

  m_file = -1;
  return true;
}

void LocalCamera::run()
{
  while (isRunning()) {
    while (true) {
      struct timeval tv;

      /* Timeout. */
      tv.tv_sec = 2;
      tv.tv_usec = 0;

      fd_set fds;
      FD_ZERO(&fds);
      FD_SET(m_file, &fds);

      int retVal = select(m_file + 1, &fds, NULL, NULL, &tv);

      if (retVal == -1) {
        if (EINTR == errno)
          continue;
        perror("select");
      }

      if (retVal == 0) {
        fprintf(stderr, "select timeout\n");
        return;
      }

      if (captureFrame())
        break;

      /* EAGAIN - continue select loop. */
    }
  }
}

bool LocalCamera::captureFrame()
{
  struct v4l2_buffer buf;

  switch (m_ioMethod) {
  case READ:
    if (::read(m_file, m_buffers[0].data, m_buffers[0].size) == -1) {
      switch (errno) {
      case EAGAIN:
        return true;

      case EIO:
        /* Could ignore EIO, see spec. */
        /* fall through */

      default:
        perror("read");
        return false;
      }
    }
    processFrame(m_buffers[0].data, m_buffers[0].size);
    break;

  case MMAP:
    CLEAR(buf);

    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;

    if (ioctl(VIDIOC_DQBUF, &buf) == -1) {
      switch (errno) {
      case EAGAIN:
        return true;

      case EIO:
        /* Could ignore EIO, see spec. */
        /* fall through */

      default:
        perror("VIDIOC_DQBUF");
        return false;
      }
    }

    assert(buf.index < m_buffers.size());

    processFrame(m_buffers[buf.index].data, buf.bytesused);

    if (ioctl(VIDIOC_QBUF, &buf) == -1) {
      perror("VIDIOC_QBUF");
      return false;
    }
    break;
  }

  return true;
}

}
