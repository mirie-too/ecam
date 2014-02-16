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
#include <signal.h>
#include <stdio.h>

#include <ecam/decoder.h>
#include <ecam/localcamera.h>

using namespace ecam;

bool forever = true;

void sigint_handler(int)
{
  printf("\r");
  forever = false;
}

void printUsage()
{
  printf("Usage: ex2 [device]\n"
         "If device is not specified the first available one will be used.\n"
         "\nExample:\n"
         "  ex2 /dev/video2\n");
}

class FrameCounter : public Decoder {
  public:
    FrameCounter()
      : m_numFrames(0)
    {}

    int numFrames() const { return m_numFrames; }

    bool decode(const uint8_t *data, unsigned size, Frame &frame)
    {
      (void)data;
      (void)size;
      (void)frame;
      m_numFrames++;
      return true;
    }

  private:
    int m_numFrames;
};

int main(int argc, char *argv[])
{
  printf("Easy Camera (version %s) usage example #2\n", version().c_str());

  if (argc > 2 || (argc > 1 && (std::string(argv[1]) == "--help"))) {
    printUsage();
    return 1;
  }

  string device;

  if (argc == 2) {
    device = argv[1];
  } else {
    vector<string> devices = LocalCamera::devices();
    if (!devices.empty()) {
      device = devices.front();
    } else {
      printf("Cameras not found!\n");
      return 2;
    }
  }

  printf("Using %s...\n", device.c_str());

  signal(SIGINT, &sigint_handler);

  LocalCamera localCamera;
  localCamera.setDeviceName(device);
  localCamera.setIoMethod(LocalCamera::MMAP);

  Camera *camera = &localCamera;
  if (camera->open()) {
    printf("Camera opened successfully\n");
    printf("Start capturing, press Control-C to stop...\n");
    camera->setDecoder(new FrameCounter(), true);
    camera->play();
    time_t t0 = time(NULL);
    while (forever);
    time_t t1 = time(NULL);
    FrameCounter *frameCounter =
        reinterpret_cast<FrameCounter *>(camera->decoder());
    int nf = frameCounter->numFrames();
    double dt = difftime(t1, t0);
    printf("Captured %d frames in %0.2f sec, FPS = %0.2f\n",
           nf, dt, nf / dt);
    camera->stop();
    camera->close();
  } else {
    printf("Can't open device %s!\n", device.c_str());
  }

  printf("end.\n");
  return 0;
}
