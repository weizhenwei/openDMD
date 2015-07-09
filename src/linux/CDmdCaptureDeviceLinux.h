/*
 ============================================================================
 Name        : CDmdCaptureDeviceLinux.h
 Author      : weizhenwei, <weizhenwei1988@gmail.com>
 Date           :2015.07.06
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
 Description : header file of capture device on linux platform.
 ============================================================================
 */


#ifndef SRC_LINUX_CDMDCAPTUREDEVICELINUX_H
#define SRC_LINUX_CDMDCAPTUREDEVICELINUX_H

#include <linux/videodev2.h>

#include <string>

#include "IDmdCaptureDevice.h"

using std::string;

namespace opendmd {

struct v4l2_device_info {
    int video_device_fd;                                 // video device fd;
    string video_device_path;                      // video device path;
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


class CDmdCaptureDeviceLinux : public IDmdCaptureDevice {
public:
    CDmdCaptureDeviceLinux();
     ~CDmdCaptureDeviceLinux();

     DMD_S_RESULT init(const char *deviceName);

     // IDmdCaptureDevice interface
     DMD_S_RESULT setDeviceName(const char *deviceName);
     DMD_S_RESULT getDeviceName(char *deviceName);
     DMD_S_RESULT initDevice();

private:
     void releaseV4L2();
     struct v4l2_device_info m_pV4L2_info;
};

}  // namespace opendmd

#endif  // SRC_LINUX_CDMDCAPTUREDEVICELINUX_H
