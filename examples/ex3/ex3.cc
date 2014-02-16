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
  printf("Usage: ex3 [device]\n"
         "If device is not specified the first available one will be used.\n"
         "\nExample:\n"
         "  ex3 /dev/video2\n");
}

class YUYVDecoder : public Decoder {
  public:
    YUYVDecoder()
      : m_numFrames(0)
    {}

    int numFrames() const { return m_numFrames; }

    bool decode(const uint8_t *data, unsigned size, Frame &frame)
    {
      const uint8_t *src = data;
      uint8_t *dst = frame.data();

      for (int i = 0; i < size; i += 4, src += 4) {
        double Y1 = static_cast<double>(src[0]);
        double cb = static_cast<double>(src[1]) - 128.0;
        double Y2 = static_cast<double>(src[2]);
        double cr = static_cast<double>(src[3]) - 128.0;

        uint8_t r1 = uint8_t(Y1 + 1.402 * cr);
        uint8_t g1 = uint8_t(Y1 - 0.344 * cb - 0.714 * cr);
        uint8_t b1 = uint8_t(Y1 + 1.772 * cb);
        uint8_t r2 = uint8_t(Y2 + 1.402 * cr);
        uint8_t g2 = uint8_t(Y2 - 0.344 * cb - 0.714 * cr);
        uint8_t b2 = uint8_t(Y2 + 1.772 * cb);

        int j = i << 1;
        dst[j + 3] = 0xFF;
        dst[j + 2] = r1;
        dst[j + 1] = g1;
        dst[j + 0] = b1;
        dst[j + 7] = 0xFF;
        dst[j + 6] = r2;
        dst[j + 5] = g2;
        dst[j + 4] = b2;
      }

      m_numFrames++;

      if (m_numFrames % 10 == 0)
        saveToTGA(frame);

      printf(".");
      fflush(stdout);

      return true;
    }

  private:
    void saveToTGA(const Frame &frame)
    {
      char fileName[16];
      sprintf(fileName, "frame_%03d.tga", m_numFrames);
      FILE *file = fopen(fileName, "wb");

      if (file == NULL)
        return;

      // --- header
      fputc(0, file);
      fputc(0, file);
      fputc(2, file);
      fputc(0, file);
      fputc(0, file);
      fputc(0, file);
      fputc(0, file);
      fputc(0, file);
      fputc(0, file);
      fputc(0, file);
      fputc(0, file);
      fputc(0, file);
      uint16_t w = frame.width();
      uint16_t h = frame.height();
      fputc(w & 0x00FF, file);
      fputc((w & 0xFF00) / 256, file);
      fputc(h & 0x00FF, file);
      fputc((h & 0xFF00) / 256, file);
      fputc(32, file);
      fputc(32, file);

      // --- data
      for (int i = 0; i < frame.dataSize(); i++)
        fputc(frame.data()[i], file);

      fclose(file);
    }

    int m_numFrames;
};

int main(int argc, char *argv[])
{
  printf("Easy Camera (version %s) usage example #3\n", version().c_str());

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
    camera->setDecoder(new YUYVDecoder(), true);
    camera->play();
    time_t t0 = time(NULL);
    while (forever);
    time_t t1 = time(NULL);
    YUYVDecoder *decoder =
        reinterpret_cast<YUYVDecoder *>(camera->decoder());
    int nf = decoder->numFrames();
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
