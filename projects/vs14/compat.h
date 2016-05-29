/*
 * Correctly setup required msvc compatibility options based on which native
 * Windows compiler is in use.
 * Copyright (c) 2015 Matthew Oliver
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef SMP_COMPAT_H
#define SMP_COMPAT_H

#ifdef _MSC_VER
#   define inline __inline
#   define strcasecmp _stricmp
#   define strncasecmp _strnicmp
#   include <crtversion.h>
#   if _VC_CRT_MAJOR_VERSION >= 14
#       pragma comment(lib, "legacy_stdio_definitions.lib")
#   else
#       define snprintf _snprintf
#       define strtoll _strtoi64
#   endif
#define __attribute__(x)
#   include "asprintf.h"
#endif

#endif /* SMP_COMPAT_H */