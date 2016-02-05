/*
 ============================================================================
 Name        : CDmdV4L2Utils.cpp
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

#include <sys/ioctl.h>
#include <linux/videodev2.h>

#include <sstream>
#include <string>

#include "DmdLog.h"

using std::ostringstream;
using std::string;


namespace opendmd {

int v4l2IOCTL(int fd, int request, void *arg) {
    int r;
    do {
        r = ioctl(fd, request, arg);
    } while (-1 == r && EINTR == errno);

    return r;
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
string v4l2CapabilityToString(uint32_t capability) {
    ostringstream os;
    string strCap;

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
string v4l2BUFTypeToString(uint32_t type) {
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

}  // namespace opendmd
