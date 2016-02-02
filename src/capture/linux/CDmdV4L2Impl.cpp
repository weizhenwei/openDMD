/*
 ============================================================================
 Name        : CDmdV4L2Impl.cpp
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
 Description : source file of V4L2 capture implementation on linux platform.
 ============================================================================
 */

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "DmdLog.h"

#include "CDmdV4L2Impl.h"

namespace opendmd {

CDmdV4L2Impl::CDmdV4L2Impl() {
    memset(&m_v4l2Param, 0, sizeof(m_v4l2Param));
}

CDmdV4L2Impl::~CDmdV4L2Impl() {
}

int CDmdV4L2Impl::_v4l2IOCTL(int fd, int request, void *arg) {
    int r;
    do {
        r = ioctl(fd, request, arg);
    } while (-1 == r && EINTR == errno);

    return r;
}

DMD_RESULT CDmdV4L2Impl::Init(const DmdCaptureVideoFormat &videoFormat) {
    memcpy(&m_v4l2Param.videoFormat, &videoFormat, sizeof(videoFormat));
    m_v4l2Param.mmap_reqcount = MMAP_REQCOUNT;
    m_v4l2Param.mmap_reqbuffers = (struct mmap_buffer *)
        malloc(m_v4l2Param.mmap_reqcount * sizeof(struct mmap_buffer));
    if (NULL == m_v4l2Param.mmap_reqbuffers) {
        DMD_LOG_ERROR("CDmdV4L2Impl::Init(), "
                << "malloc m_v4l2Param.mmap_reqbuffers failed.");
        return DMD_S_FAIL;
    }

    return DMD_S_OK;
}

DMD_RESULT CDmdV4L2Impl::Uninit() {
    if (m_v4l2Param.mmap_reqbuffers) {
        free(m_v4l2Param.mmap_reqbuffers);
        m_v4l2Param.mmap_reqbuffers = NULL;
    }
    memset(&m_v4l2Param, 0, sizeof(m_v4l2Param));

    return DMD_S_OK;
}

DMD_RESULT CDmdV4L2Impl::StartCapture() {
    DMD_RESULT ret = DMD_S_OK;
    ret = _v4l2OpenCaptureDevice();
    if (ret != DMD_S_OK) {
        return ret;
    }

    ret =  _v4l2QueryCapability();

    return ret;
}

DMD_RESULT CDmdV4L2Impl::StopCapture() {
    DMD_RESULT ret = DMD_S_OK;
    ret = _v4l2CloseCaptureDevice();

    return ret;
}


DMD_RESULT CDmdV4L2Impl::_v4l2OpenCaptureDevice() {
    int fd = -1;
    DMD_RESULT ret = DMD_S_OK;
    DMD_LOG_INFO("CDmdV4L2Impl::_v4l2OpenCaptureDevice(), "
            << "open video capture device:"
            << m_v4l2Param.videoFormat.sVideoDevice);
    if ((fd = open(m_v4l2Param.videoFormat.sVideoDevice, O_RDWR)) == -1) {
        DMD_LOG_ERROR("CDmdV4L2Impl::_v4l2OpenCaptureDevice(), "
                << "open video capture device error:" << strerror(errno));
        ret = DMD_S_FAIL;
    }

    m_v4l2Param.video_device_fd = fd;
    return ret;
}

DMD_RESULT CDmdV4L2Impl::_v4l2CloseCaptureDevice() {
    DMD_RESULT ret = DMD_S_OK;
    DMD_LOG_INFO("CDmdV4L2Impl::_v4l2CloseCaptureDevice(), "
            << "close video capture device:"
            << m_v4l2Param.videoFormat.sVideoDevice);
    if (close(m_v4l2Param.video_device_fd) == -1) {
        DMD_LOG_ERROR("CDmdV4L2Impl::_v4l2CloseCaptureDevice(), "
                << "close video capture device error:" << strerror(errno));
        ret = DMD_S_FAIL;
    }

    m_v4l2Param.video_device_fd = -1;
    return ret;
}

/*
 * DRIVER CAPABILITIES
 *
 * struct v4l2_capability {
 *     __u8    driver[16];    // i.e. "bttv"
 *     __u8    card[32];      // i.e. "Hauppauge WinTV"
 *     __u8    bus_info[32];  // "PCI:" + pci_name(pci_dev)
 *     __u32   version;       // should use KERNEL_VERSION()
 *     __u32   capabilities;  // Device capabilities,
 *                            // for more details, see linux/videodev2.h
 *     __u32   reserved[4];
 * };
 *
 * query video device's capability
 */
DMD_RESULT CDmdV4L2Impl::_v4l2QueryCapability() {
    DMD_RESULT ret = DMD_S_OK;

    // get the device capability.
    struct v4l2_capability capture = m_v4l2Param.cap;
    if (ioctl(m_v4l2Param.video_device_fd, VIDIOC_QUERYCAP, &capture)  == -1) {
        DMD_LOG_ERROR("CDmdV4L2Impl::_v4l2QueryCapability(), "
                << "query video capture device capability error:"
                << strerror(errno));
        ret = DMD_S_FAIL;
        return ret;
    }

    DMD_LOG_INFO("Video Capture Device Capability information summary:"
            << "driver:" << capture.driver << ", "
            << "bus_info:" << capture.bus_info << ", "
            << "version:" << capture.version << ", "
            << "capabilities: " << capture.capabilities << ", ");

    if (capture.capabilities & V4L2_CAP_VIDEO_CAPTURE) {
        DMD_LOG_INFO("Capture capability is supported");
    } else {
        DMD_LOG_INFO("Capture capability is not supported");
    }

    if (capture.capabilities & V4L2_CAP_VIDEO_OUTPUT) {
        DMD_LOG_INFO("Output capability is supported");
    } else {
        DMD_LOG_INFO("Output capability is not supported");
    }

    if (capture.capabilities & V4L2_CAP_STREAMING) {
        DMD_LOG_INFO("Streaming capability is supported");
    } else {
        DMD_LOG_INFO("Streaming capability is not supported");
    }

    return ret;
}

DMD_RESULT CDmdV4L2Impl::_v4l2QueryFormat() {
    return DMD_S_OK;
}


DMD_RESULT CDmdV4L2Impl::_v4l2SetupFormat() {
    return DMD_S_OK;
}

DMD_RESULT CDmdV4L2Impl::_v4l2QueryFPS() {
    return DMD_S_OK;
}

DMD_RESULT CDmdV4L2Impl::_v4l2SetupFPS() {
    return DMD_S_OK;
}

DMD_RESULT CDmdV4L2Impl::_v4l2CreateRequestBuffers() {
    return DMD_S_OK;
}

DMD_RESULT CDmdV4L2Impl::_v4l2mmap() {
    return DMD_S_OK;
}

DMD_RESULT CDmdV4L2Impl::_v4l2StreamON() {
    return DMD_S_OK;
}

DMD_RESULT CDmdV4L2Impl::_v4l2StreamOFF() {
    return DMD_S_OK;
}

DMD_RESULT CDmdV4L2Impl::_v4l2unmmap() {
    return DMD_S_OK;
}

}  // namespace opendmd

