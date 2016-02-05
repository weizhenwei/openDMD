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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "DmdLog.h"

#include "CDmdV4L2Utils.h"
#include "CDmdV4L2Impl.h"

namespace opendmd {

CDmdV4L2Impl::CDmdV4L2Impl() {
    memset(&m_videoFormat, 0, sizeof(m_videoFormat));
    memset(&m_v4l2Param, 0, sizeof(m_v4l2Param));
}

CDmdV4L2Impl::~CDmdV4L2Impl() {
}

DMD_RESULT CDmdV4L2Impl::Init(const DmdCaptureVideoFormat &videoFormat) {
    memcpy(&m_videoFormat, &videoFormat, sizeof(videoFormat));
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
    ret =  _v4l2SetupInputFormat();
    if (ret != DMD_S_OK) {
        return ret;
    }

    ret =  _v4l2Queryfmtdesc();
    if (ret != DMD_S_OK) {
        return ret;
    }

    ret =  _v4l2SetupFormat();
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
    char *devPath = m_videoFormat.sVideoDevice;
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
            << m_videoFormat.sVideoDevice);

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
            << m_videoFormat.sVideoDevice);
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
    if (-1 == v4l2IOCTL(m_v4l2Param.video_device_fd, VIDIOC_QUERYCAP,
                &m_v4l2Param.cap)) {
        DMD_LOG_ERROR("CDmdV4L2Impl::_v4l2QueryCapability(), "
                << "query video capture device capability error:"
                << strerror(errno));
        ret = DMD_S_FAIL;
        return ret;
    }

    struct v4l2_capability capture = m_v4l2Param.cap;
    DMD_LOG_INFO("CDmdV4L2Impl::_v4l2QueryCapability(), Capability: "
            << "driver:" << capture.driver << ", "
            << "bus_info:" << capture.bus_info << ", "
            << "version:" << capture.version << ", "
            << "capabilities: "
            << v4l2CapabilityToString(capture.capabilities));

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
    int i = 0, ok = 0;
    int fd = m_v4l2Param.video_device_fd;
    for (i = 0; ok == 0; i++) {
        m_v4l2Param.input.index = i;
        if ((ok = v4l2IOCTL(fd, VIDIOC_ENUMINPUT, &m_v4l2Param.input.index))
                    < 0) {
            ok = errno;
            break;
        }

        DMD_LOG_INFO("CDmdV4L2Impl::_v4l2QueryInputFormat(), Input Format: "
                << "input name:" << m_v4l2Param.input.name << ", "
                << "input index:" << m_v4l2Param.input.index << ", "
                << "input type:" << m_v4l2Param.input.type);
    }  // for

    if (i < 0 && ok != -EINVAL) {
        DMD_LOG_ERROR("CDmdV4L2Impl::_v4l2QueryInputFormat(), "
                << "call ioctl VIDIOC_ENUMINPUT error:" << strerror(errno));
        ret = DMD_S_FAIL;
        return ret;
    }

    return ret;
}

DMD_RESULT CDmdV4L2Impl::_v4l2SetupInputFormat() {
    DMD_RESULT ret = DMD_S_OK;
    int fd = m_v4l2Param.video_device_fd;
    int index = 0;  // the input index to be set;

    if (-1 == v4l2IOCTL(fd, VIDIOC_S_INPUT, &index)) {
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
 * traverse video frame format, query frame format this video device support.
 */
DMD_RESULT CDmdV4L2Impl::_v4l2Queryfmtdesc() {
    DMD_RESULT ret = DMD_S_OK;
    int i = 0, ok = 0;
    int fd = m_v4l2Param.video_device_fd;
    enum v4l2_buf_type targetType = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    memset(&m_v4l2Param.fmtdesc, 0, sizeof(m_v4l2Param.fmtdesc));
    for (i = 0; ok == 0; i++) {
        m_v4l2Param.fmtdesc.index = i;
        m_v4l2Param.fmtdesc.type = targetType;
        if ((ok = v4l2IOCTL(fd, VIDIOC_ENUM_FMT, &m_v4l2Param.fmtdesc)) < 0) {
            ok = errno;
            break;
        }

        DMD_LOG_INFO("CDmdV4L2Impl::_v4l2Queryfmtdesc(), fmtdesc: "
                << "index:" << m_v4l2Param.fmtdesc.index << ", "
                << "type:"
                << v4l2BUFTypeToString(m_v4l2Param.fmtdesc.type) << ", "
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


/*
 * struct v4l2_format {
 *     enum v4l2_buf_type type;
 *     union {
 *       struct v4l2_pix_format pix;           // VIDEO_CAPTURE
 *       struct v4l2_pix_format_mplane pix_mp; // VIDEO_CAPTURE_MPLANE
 *       struct v4l2_window win;               // VIDEO_OVERLAY
 *       struct v4l2_vbi_format vbi;           // VBI_CAPTURE
 *       struct v4l2_sliced_vbi_format sliced; // SLICED_VBI_CAPTURE
 *       __u8 raw_data[200];                   // user-defined
 *     } fmt;
 * };
 *
 * VIDEO IMAGE FORMAT
 * struct v4l2_pix_format {
 *     __u32    width;
 *     __u32    height;
 *     __u32    pixelformat;
 *     __u32    field;         // enum v4l2_field
 *     __u32    bytesperline;  // for padding, zero if unused
 *     __u32    sizeimage;
 *     __u32    colorspace;    // enum v4l2_colorspace
 *     __u32    priv;          // private data, depends on pixelformat
 * };
 *
 * set video stream data format
 */
DMD_RESULT CDmdV4L2Impl::_v4l2QueryFormat() {
    DMD_RESULT ret = DMD_S_OK;
    int fd = m_v4l2Param.video_device_fd;
    // bzero(&m_v4l2Param.fmt, sizeof(struct v4l2_format));
    m_v4l2Param.fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (-1 == (v4l2IOCTL(fd, VIDIOC_G_FMT, &m_v4l2Param.fmt))) {
        DMD_LOG_ERROR("CDmdV4L2Impl::_v4l2QueryFormat(), "
                << "call ioctl VIDIOC_G_FMT error:" << strerror(errno));
        ret = DMD_S_FAIL;
        return ret;
    }

    struct v4l2_format fmt = m_v4l2Param.fmt;
    DMD_LOG_INFO("CDmdV4L2Impl::_v4l2QueryFormat(), Format: "
            << "type:" << v4l2BUFTypeToString(fmt.type) << ", "
            << "width = " << fmt.fmt.pix.width << ", "
            << "height = " << fmt.fmt.pix.height << ", "
            << "pixelformat = " << fmt.fmt.pix.pixelformat << ", "
            << "field = " << fmt.fmt.pix.field << ", "
            << "bytesperline = " << fmt.fmt.pix.bytesperline << ", "
            << "sizeimage = " << fmt.fmt.pix.sizeimage << ", "
            << "colorspace = " << fmt.fmt.pix.colorspace);

    return ret;
}

DMD_RESULT CDmdV4L2Impl::_v4l2SetupFormat() {
    DMD_RESULT ret = DMD_S_OK;
    int fd = m_v4l2Param.video_device_fd;
    m_v4l2Param.fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    m_v4l2Param.fmt.fmt.pix.width = m_videoFormat.iWidth;
    m_v4l2Param.fmt.fmt.pix.height = m_videoFormat.iHeight;
    m_v4l2Param.fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    // m_v4l2Param.fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB32;
    m_v4l2Param.fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

    if (-1 == (v4l2IOCTL(fd, VIDIOC_S_FMT, &m_v4l2Param.fmt))) {
        DMD_LOG_ERROR("CDmdV4L2Impl::_v4l2SetupFormat(), "
                << "call ioctl VIDIOC_S_FMT error:" << strerror(errno));
        ret = DMD_S_FAIL;
        return ret;
    }

    _v4l2QueryFormat();
    return ret;
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

