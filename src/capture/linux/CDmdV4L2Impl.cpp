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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <sstream>
#include <string>

#include "DmdLog.h"

#include "CDmdV4L2Impl.h"

using std::ostringstream;
using std::string;


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

    ret = _v4l2QueryCapability();
    if (ret != DMD_S_OK) {
        return ret;
    }
    bool bSupportCaptureVideo =
        _v4l2CheckVideoCaptureCapability(m_v4l2Param.cap.capabilities);
    if (!bSupportCaptureVideo) {
        ret = DMD_S_FAIL;
        return ret;
    }

    ret =  _v4l2QueryInputFormat();
    if (ret != DMD_S_OK) {
        return ret;
    }

    /*
    ret =  _v4l2SetupInputFormat();
    if (ret != DMD_S_OK) {
        return ret;
    }
    */

    ret =  _v4l2Queryfmtdesc();
    if (ret != DMD_S_OK) {
        return ret;
    }

    return ret;
}

DMD_RESULT CDmdV4L2Impl::StopCapture() {
    DMD_RESULT ret = DMD_S_OK;
    ret = _v4l2CloseCaptureDevice();

    return ret;
}


DMD_RESULT CDmdV4L2Impl::_v4l2OpenCaptureDevice() {
    DMD_RESULT ret = DMD_S_OK;
    int fd = -1;
    struct stat st;
    char *devPath = m_v4l2Param.videoFormat.sVideoDevice;
    if (-1 == stat(devPath, &st)) {
        DMD_LOG_ERROR("CDmdV4L2Impl::_v4l2OpenCaptureDevice(), "
            << "could not identify " << devPath
            << ":" << strerror(errno));
        ret = DMD_S_FAIL;
        return ret;
    }

    if (!S_ISCHR(st.st_mode)) {
        DMD_LOG_ERROR("CDmdV4L2Impl::_v4l2OpenCaptureDevice(), "
            << "file " << devPath << "is not a char device"
            << ":" << strerror(errno));
        ret = DMD_S_FAIL;
        return ret;
    }

    DMD_LOG_INFO("CDmdV4L2Impl::_v4l2OpenCaptureDevice(), "
            << "open video capture device:"
            << m_v4l2Param.videoFormat.sVideoDevice);

    if (-1 == (fd = open(devPath, O_RDWR))) {
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
string CDmdV4L2Impl::_v4l2CapabilityToString(uint32_t capability) {
    ostringstream os;
    string strCap;

    char s[4096] = "";
    if (V4L2_CAP_VIDEO_CAPTURE & capability)
        os << "capture ";
    if (V4L2_CAP_VIDEO_CAPTURE_MPLANE & capability)
        os << "capture_mplane ";
    if (V4L2_CAP_VIDEO_OUTPUT & capability)
        os << "output ";
    if (V4L2_CAP_VIDEO_OUTPUT_MPLANE & capability)
        os << "output_mplane ";
    if (V4L2_CAP_VIDEO_M2M & capability)
        os << "m2m ";
    if (V4L2_CAP_VIDEO_M2M_MPLANE & capability)
        os << "m2m_mplane ";
    if (V4L2_CAP_VIDEO_OVERLAY & capability)
        os << "overlay ";
    if (V4L2_CAP_VBI_CAPTURE & capability)
        os << "vbi_capture ";
    if (V4L2_CAP_VBI_OUTPUT & capability)
        os << "vbi_output ";
    if (V4L2_CAP_SLICED_VBI_CAPTURE & capability)
        os << "sliced_vbi_capture ";
    if (V4L2_CAP_SLICED_VBI_OUTPUT & capability)
        os << "sliced_vbi_output ";
    if (V4L2_CAP_RDS_CAPTURE & capability)
        os << "rds_capture ";
    if (V4L2_CAP_RDS_OUTPUT & capability)
        os << "rds_output ";
    if (V4L2_CAP_TUNER & capability)
        os << "tuner ";
    if (V4L2_CAP_HW_FREQ_SEEK & capability)
        os << "hw_freq_seek ";
    if (V4L2_CAP_MODULATOR & capability)
        os << "modulator ";
    if (V4L2_CAP_AUDIO & capability)
        os << "audio ";
    if (V4L2_CAP_RADIO & capability)
        os << "radio ";
    if (V4L2_CAP_READWRITE & capability)
        os << "readwrite ";
    if (V4L2_CAP_ASYNCIO & capability)
        os << "asyncio ";
    if (V4L2_CAP_STREAMING & capability)
        os << "streaming ";
    if (V4L2_CAP_DEVICE_CAPS & capability)
        os << "device_caps ";

    strCap = os.str();

    return strCap;
}

bool CDmdV4L2Impl::_v4l2CheckVideoCaptureCapability(uint32_t capability) {
    bool bSupportVideoCapture = true;

    if ((capability & V4L2_CAP_VIDEO_CAPTURE) == 0) {
        DMD_LOG_ERROR("CDmdV4L2Impl::_v4l2CheckVideoCaptureCapability(), "
                << "Video capture capability is not supported");
        bSupportVideoCapture = false;
    }

    if ((capability & V4L2_CAP_STREAMING) == 0) {
        DMD_LOG_ERROR("CDmdV4L2Impl::_v4l2CheckVideoCaptureCapability(), "
                << "Streaming capability is not supported");
        bSupportVideoCapture = false;
    }

    return bSupportVideoCapture;
}

DMD_RESULT CDmdV4L2Impl::_v4l2QueryCapability() {
    DMD_RESULT ret = DMD_S_OK;

    // get the device capability.
    if (-1 == _v4l2IOCTL(m_v4l2Param.video_device_fd, VIDIOC_QUERYCAP,
                &m_v4l2Param.cap)) {
        DMD_LOG_ERROR("CDmdV4L2Impl::_v4l2QueryCapability(), "
                << "query video capture device capability error:"
                << strerror(errno));
        ret = DMD_S_FAIL;
        return ret;
    }

    struct v4l2_capability capture = m_v4l2Param.cap;
    DMD_LOG_INFO("CDmdV4L2Impl::_v4l2QueryCapability(), capability summary:"
            << "driver:" << capture.driver << ", "
            << "bus_info:" << capture.bus_info << ", "
            << "version:" << capture.version << ", "
            << "capabilities: "
            << _v4l2CapabilityToString(capture.capabilities));

    return ret;
}


/*
 *    VIDEO   INPUTS
 *
 * struct v4l2_input {
 *     __u32         index;     //  Which input
 *     __u8          name[32];  //  Label
 *     __u32         type;      //  Type of input
 *     __u32         audioset;  //  Associated audios (bitfield)
 *     __u32         tuner;     //  Associated tuner
 *     v4l2_std_id   std;
 *     __u32         status;
 *     __u32         capabilities;
 *     __u32         reserved[3];
 * }
 *
 * query current video input format
 */
DMD_RESULT CDmdV4L2Impl::_v4l2QueryInputFormat() {
    DMD_RESULT ret = DMD_S_OK;
    int fd = m_v4l2Param.video_device_fd;

    // first, get current video input index;
    m_v4l2Param.input.index = 0;
    if (-1 == _v4l2IOCTL(fd, VIDIOC_G_INPUT, &m_v4l2Param.input.index)) {
        DMD_LOG_ERROR("CDmdV4L2Impl::_v4l2QueryInputFormat(), "
                << "call ioctl VIDIOC_G_INPUT error:" << strerror(errno));
        ret = DMD_S_FAIL;
        return ret;
    }

    // then Query current video input's info;
    if (-1 == _v4l2IOCTL(fd, VIDIOC_ENUMINPUT, &m_v4l2Param.input)) {
        DMD_LOG_ERROR("CDmdV4L2Impl::_v4l2QueryInputFormat(), "
                << "call ioctl VIDIOC_ENUMINPUT error:" << strerror(errno));
        ret = DMD_S_FAIL;
        return ret;
    }

    DMD_LOG_INFO("CDmdV4L2Impl::_v4l2QueryInputFormat(), "
            << "input name:" << m_v4l2Param.input.name << ", "
            << "input index:" << m_v4l2Param.input.index << ", "
            << "input type:" << m_v4l2Param.input.type);

    return ret;
}

DMD_RESULT CDmdV4L2Impl::_v4l2SetupInputFormat() {
    DMD_RESULT ret = DMD_S_OK;
    int fd = m_v4l2Param.video_device_fd;
    int index = 0;  // the input index to be set;

    if (-1 == _v4l2IOCTL(fd, VIDIOC_S_INPUT, &index)) {
        DMD_LOG_ERROR("CDmdV4L2Impl::_v4l2SetupInputFormat(), "
                << "call ioctl VIDIOC_S_INPUT error:" << strerror(errno));
        ret = DMD_S_FAIL;
        return ret;
    }

    return ret;
}


/*
 *    FORMAT   ENUMERATION
 *
 * struct v4l2_fmtdesc {
 *     __u32               index;             // Format number      
 *     enum v4l2_buf_type  type;              // buffer type        
 *     __u32               flags;
 *     __u8                description[32];   // Description string 
 *     __u32               pixelformat;       // Format fourcc      
 *     __u32               reserved[4];
 * };
 *
 * traverse video stream format, query video format this video device support.
 */
string CDmdV4L2Impl::_v4l2BUFTypeToString(uint32_t type) {
    /*
     * enum v4l2_buf_type {
     *     V4L2_BUF_TYPE_VIDEO_CAPTURE        = 1,
     *     V4L2_BUF_TYPE_VIDEO_OUTPUT         = 2,
     *     V4L2_BUF_TYPE_VIDEO_OVERLAY        = 3,
     *     V4L2_BUF_TYPE_VBI_CAPTURE          = 4,
     *     V4L2_BUF_TYPE_VBI_OUTPUT           = 5,
     *     V4L2_BUF_TYPE_SLICED_VBI_CAPTURE   = 6,
     *     V4L2_BUF_TYPE_SLICED_VBI_OUTPUT    = 7,
     *     V4L2_BUF_TYPE_VIDEO_OUTPUT_OVERLAY = 8,
     *     V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE = 9,
     *     V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE  = 10,
     *     V4L2_BUF_TYPE_PRIVATE              = 0x80,
     * };
    */

    switch (type) {
        case V4L2_BUF_TYPE_VIDEO_CAPTURE:
            return "video_capture";
        case V4L2_BUF_TYPE_VIDEO_OUTPUT:
            return "video_output";
        case V4L2_BUF_TYPE_VIDEO_OVERLAY:
            return "video_overlay";
        case V4L2_BUF_TYPE_VBI_CAPTURE:
            return "vbi_capture";
        case V4L2_BUF_TYPE_VBI_OUTPUT:
            return "vbi_output";
        case V4L2_BUF_TYPE_SLICED_VBI_CAPTURE:
            return "sliced_vbi_capture";
        case V4L2_BUF_TYPE_SLICED_VBI_OUTPUT:
            return "sliced_vbi_output";
        case V4L2_BUF_TYPE_VIDEO_OUTPUT_OVERLAY:
            return "video_output_overlay";
        case V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE:
            return "video_capture_mplane";
        case V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE:
            return "video_output_mplane";
        case V4L2_BUF_TYPE_PRIVATE:
            return "private";
        default:
            return "Unknown Buffer Type";
    }

    return "Unknown Buffer Type";
}

DMD_RESULT CDmdV4L2Impl::_v4l2Queryfmtdesc() {
    DMD_RESULT ret = DMD_S_OK;
    int i = 0, ok = 0;
    int fd = m_v4l2Param.video_device_fd;
    enum v4l2_buf_type targetType = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    memset(&m_v4l2Param.fmtdesc, 0, sizeof(m_v4l2Param.fmtdesc));
    for (i = 0; ok == 0; i++) {
        m_v4l2Param.fmtdesc.index = i;
        m_v4l2Param.fmtdesc.type = targetType;
        ok = _v4l2IOCTL(fd, VIDIOC_ENUM_FMT, &m_v4l2Param.fmtdesc);
        if ((ok = _v4l2IOCTL(fd, VIDIOC_ENUM_FMT, &m_v4l2Param.fmtdesc)) < 0) {
            ok = errno;
            break;
        }

        DMD_LOG_INFO("CDmdV4L2Impl::_v4l2Queryfmtdesc(), "
                << "index:" << m_v4l2Param.fmtdesc.index << ", "
                << "type:"
                << _v4l2BUFTypeToString(m_v4l2Param.fmtdesc.type) << ", "
                << "flags:" << m_v4l2Param.fmtdesc.flags << ", "
                << "description:" << m_v4l2Param.fmtdesc.description << ", "
                << "pixelformat:0x" << (m_v4l2Param.fmtdesc.pixelformat & 0xff)
                << ((m_v4l2Param.fmtdesc.pixelformat >> 8) & 0xff)
                << ((m_v4l2Param.fmtdesc.pixelformat >> 16) & 0xff)
                << ((m_v4l2Param.fmtdesc.pixelformat >> 24) & 0xff));
    }  // for

    if (i < 0 && ok != -EINVAL) {
        DMD_LOG_ERROR("CDmdV4L2Impl::_v4l2Queryfmtdesc(), "
                << "call ioctl VIDIOC_ENUM_FMT error:" << strerror(errno));
        ret = DMD_S_FAIL;
    }

    return ret;
}

DMD_RESULT _v4l2Setupfmtdesc() {
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

