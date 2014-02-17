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
#ifndef LIBECAM_THREAD_H_
#define LIBECAM_THREAD_H_

#include <ecam/config.h>

#ifdef LIBECAM_USE_CPP11
#else
#include <ecam/../../3rdparty/TinyThread++-1.1/source/tinythread.h>
#include <ecam/../../3rdparty/TinyThread++-1.1/source/fast_mutex.h>
#endif

namespace ecam {

/**
  * In non-c++11 mode TinyThread++ library is used for threading support.
  */

#ifdef LIBECAM_USE_CPP11
#else
using tthread::lock_guard;
using tthread::thread;
typedef tthread::fast_mutex mutex;
#endif

}


#endif
