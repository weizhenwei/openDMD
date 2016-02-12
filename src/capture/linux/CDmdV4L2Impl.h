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

#include <linux/videodev2.h>

#include "IDmdCaptureEngine.h"
#include "IDmdDatatype.h"

namespace opendmd {

struct mmap_buffer {
    void *start;
    unsigned int length;
};

#define REQUEST_BUFFERS_COUNT 5

typedef struct _v4l2_capture_param {
    int video_device_fd;                          // video device fd;
    struct v4l2_capability cap;                   // video device capabilities;
    struct v4l2_input input;                      // video input;
    v4l2_std_id std_id;                           // video standard id;
    struct v4l2_fmtdesc fmtdesc;                  // video format enumeration;
    struct v4l2_cropcap cropcap;                  // video cropcap;
    struct v4l2_crop crop;                        // video crop;
    struct v4l2_format fmt;                       // video stream data format;
    struct v4l2_streamparm streamparam;           // video stream param;
    struct v4l2_requestbuffers reqbuffers;        // video request buffers;
    unsigned int request_buffers_count;           // request buffers count;
    struct mmap_buffer *mmap_reqbuffers;          // mmap buffers;
} v4l2_capture_param;

class CDmdV4L2Impl {
public:
    CDmdV4L2Impl();
    ~CDmdV4L2Impl();

    DMD_RESULT Init(const DmdCaptureVideoFormat &videoFormat);
    DMD_RESULT Uninit();

    // start/stop capture;
    DMD_RESULT StartCapture();
    DMD_RESULT StopCapture();

private:
    DMD_RESULT _v4l2OpenCaptureDevice();
    DMD_RESULT _v4l2CloseCaptureDevice();

    // device capability, v4l2_capability;
    DMD_RESULT _v4l2QueryCapability();
    bool _v4l2CheckVideoCaptureCapability(uint32_t capability);

    // input format, v4l2_input
    DMD_RESULT _v4l2EnumInputFormat();
    DMD_RESULT _v4l2SetupInputFormat();

    // standard, v4l2_std_id, v4l2_standard;
    DMD_RESULT _v4l2EnumStandard();
    DMD_RESULT _v4l2SetupStandard();

    // frame format, v4l2_fmtdesc;
    DMD_RESULT _v4l2Enumfmtdesc();

    // cropcap, v4l2_cropcap, v4l2_crop;
    DMD_RESULT _v4l2QueryCropcap();
    DMD_RESULT _v4l2QueryCrop();
    DMD_RESULT _v4l2SetupCrop();

    // format, v4l2_format;
    DMD_RESULT _v4l2QueryFormat();
    DMD_RESULT _v4l2SetupFormat();

    // stream param, v4l2_streamparm;
    DMD_RESULT _v4l2QueryStreamParam();
    DMD_RESULT _v4l2SetupStreamParam();

    // mmap/munmap, v4l2_requestbuffers;
    DMD_RESULT _v4l2MMAPRequestBuffers();
    DMD_RESULT _v4l2MUNMAPRequestBuffers();

    // stream on/off;
    DMD_RESULT _v4l2StreamON();
    DMD_RESULT _v4l2StreamOFF();

private:
    DmdCaptureVideoFormat m_videoFormat;
    v4l2_capture_param m_v4l2Param;
};

}  // namespace opendmd

#endif  // SRC_CAPTURE_LINUX_CDMDV4L2IMPL_H

