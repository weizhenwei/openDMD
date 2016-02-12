/*
 ============================================================================
 Name        : CDmdV4L2Utils.h
 Author      : weizhenwei, <weizhenwei1988@gmail.com>
 Date           :2016.02.05
 Copyright   :
 * Copyright (c) 2015, weizhenwei
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of the {organization} nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 Description : utils of V4L2 capture implementation on linux platform.
 ============================================================================
 */

#ifndef SRC_CAPTURE_LINUX_CDMDV4L2UTILS_H
#define SRC_CAPTURE_LINUX_CDMDV4L2UTILS_H

#include <linux/videodev2.h>

#include <string>

#include "IDmdCaptureEngine.h"
#include "IDmdDatatype.h"

using std::string;
using opendmd::DmdVideoType;

namespace opendmd {
int v4l2IOCTL(int fd, int request, void *arg);
string v4l2CapabilityToString(uint32_t capability);
string v4l2BUFTypeToString(uint32_t type);
string v4l2ColorspaceToString(uint8_t colorspace);
string v4l2PixFmtToString(uint32_t pixfmt);
string v4l2FieldToString(uint32_t field);

uint32_t v4l2DmdVideoTypeToPixelFormat(DmdVideoType videoType);
string v4l2StreamParamToString(uint32_t streamparam);
}  // namespace opendmd

#endif  // SRC_CAPTURE_LINUX_CDMDV4L2UTILS_H

