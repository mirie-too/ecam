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
#ifndef LIBECAM_CONFIG_H_
#define LIBECAM_CONFIG_H_

//#define LIBECAM_VERBOSE_DEBUG
//#define LIBECAM_USE_CPP11

#include <string>

#ifdef LIBECAM_VERBOSE_DEBUG
#include <iostream>
using std::cout;
using std::cerr;
using std::clog;
using std::endl;
#endif

#include <assert.h>
#include <stdint.h>

namespace ecam {

using std::string;

/**
 * @param major If non-NULL, MAJOR is stored in *major.
 * @param minor If non-NULL, MINOR is stored in *minor.
 * @return String containing the lirary version in "MAJOR.MINOR" format.
 */
string version(int *major = NULL, int *minor = NULL);

}

#endif
