/*
 ============================================================================
 Name        : CDmdV4L2Impl.h
 Author      : weizhenwei, <weizhenwei1988@gmail.com>
 Date           :2015.07.29
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
 Description : header file of V4L2 capture implementation on linux platform.
 ============================================================================
 */

#ifndef SRC_CAPTURE_LINUX_CDMDV4L2IMPL_H
#define SRC_CAPTURE_LINUX_CDMDV4L2IMPL_H

#include "IDmdDatatype.h"

namespace opendmd {

/*
struct v4l2_device_info {
    int video_device_fd;                                 // video device fd;
    const char * video_device_path;                      // video device path;
    struct v4l2_capability cap;                   // video device capabilities;
    struct v4l2_input input;                       // video input;
    struct v4l2_fmtdesc fmtdesc;            // video format enumeration;
    struct v4l2_format format;                // video stream data format;
    struct v4l2_requestbuffers reqbuffers;   // mmap buffers;
    struct v4l2_buffer buffer;                             // video buffer;

    int reqbuffer_count;                                      // req.count;
    struct mmap_buffer *buffers;                  // mmap buffers;

    int width;                               // picture width;
    int height;                              // picture height;
};
*/

class CDmdV4L2Impl {
public:
    CDmdV4L2Impl() {}
    ~CDmdV4L2Impl() {}

    static DMD_RESULT v4l2OpenDevice(struct v4l2_device_info *deviceInfo);
    static DMD_RESULT v4l2QueryCapability(struct v4l2_device_info
            *deviceInfo);
    static DMD_RESULT v4l2QueryFormat(struct v4l2_device_info *deviceInfo);
    static DMD_RESULT v4l2SetupFormat(struct v4l2_device_info *deviceInfo);
    static DMD_RESULT v4l2QueryFPS(struct v4l2_device_info *deviceInfo);
    static DMD_RESULT v4l2SetupFPS(struct v4l2_device_info *deviceInfo);
    static DMD_RESULT v4l2CreateRequestBuffers(struct v4l2_device_info
            *deviceInfo);
    static DMD_RESULT v4l2mmap(struct v4l2_device_info *deviceInfo);
    static DMD_RESULT v4l2StreamON(struct v4l2_device_info *deviceInfo);
    static DMD_RESULT v4l2StreamOFF(struct v4l2_device_info *deviceInfo);
    static DMD_RESULT v4l2unmmap(struct v4l2_device_info *deviceInfo);
    static DMD_RESULT v4l2CloseDevice(struct v4l2_device_info *deviceInfo);
};

}  // namespace opendmd

#endif  // SRC_CAPTURE_LINUX_CDMDV4L2IMPL_H

