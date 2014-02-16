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
#include <ecam/config.h>

#include <sstream>

// --- 1MMMmmm, where MMM = major, mmm = minor
static const int kVersion = 1000001;

namespace ecam {

std::string version(int *major, int *minor)
{
  int tmp = kVersion - 1000000;
  int MMM = tmp / 1000;
  tmp %= 1000;
  int mmm = tmp % 1000;

  if (major != NULL)
    *major = MMM;
  if (minor != NULL)
    *minor = mmm;

  std::stringstream stream;
  stream << MMM << '.' << mmm;
  return stream.str();
}

}
