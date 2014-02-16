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
#include <stdio.h>

#include <ecam/localcamera.h>

using namespace ecam;

int main(int, char *[])
{
  printf("Easy Camera (version %s) usage example #1:\n", version().c_str());
  printf("  enumerate local cameras & query their capabilities\n\n");

  vector<string> devices = LocalCamera::devices();
  for (unsigned n = 0; n < devices.size(); n++) {
    printf("Device #%d (%s)\n", n, devices[n].c_str());

    LocalCamera camera;
    camera.setDeviceName(devices[n]);
    Capabilities caps;
    if (camera.open()) {
      camera.queryCapabilities(caps);
      camera.close();
    }

    if (!caps.isCamera()) {
      printf("  not a camera!\n");
      continue;
    }

    printf("  name : %s\n", caps.name().c_str());
    printf("  driver: %s\n", caps.driver().c_str());
    printf("  bus : %s\n", caps.bus().c_str());

    printf("  number of inputs: %d\n", caps.numInputs());

    printf("  supported pixel formats: ");
    for (int i = 0; i < caps.numPixelFormats(); i++)
      printf("%s ", caps.pixelFormat(i).c_str());
    printf("\n");
  }

  printf("\nend.\n");
  return 0;
}
