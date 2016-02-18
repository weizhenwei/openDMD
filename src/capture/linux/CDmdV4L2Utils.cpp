/*
 ============================================================================
 * Name        : CDmdV4L2Utils.cpp
 * Author      : weizhenwei, <weizhenwei1988@gmail.com>
 * Date        : 2016.02.05
 *
 * Copyright (c) 2016, weizhenwei
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
 * Description : utils of V4L2 capture implementation on linux platform.
 ============================================================================
 */

#include <sys/ioctl.h>
#include <linux/videodev2.h>

#include <sstream>
#include <string>

#include "DmdLog.h"
#include "IDmdDatatype.h"

#include "CDmdV4L2Utils.h"

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

// enum v4l2_colorspace {
//     /* ITU-R 601 -- broadcast NTSC/PAL */
//     V4L2_COLORSPACE_SMPTE170M     = 1,
//
//     /* 1125-Line (US) HDTV */
//     V4L2_COLORSPACE_SMPTE240M     = 2,
//
//     /* HD and modern captures. */
//     V4L2_COLORSPACE_REC709        = 3,
//
//     /* broken BT878 extents (601, luma range 16-253 instead of 16-235) */
//     V4L2_COLORSPACE_BT878         = 4,
//
//     /* These should be useful.  Assume 601 extents. */
//     V4L2_COLORSPACE_470_SYSTEM_M  = 5,
//     V4L2_COLORSPACE_470_SYSTEM_BG = 6,
//
//     /* I know there will be cameras that send this.  So, this is
//      * unspecified chromaticities and full 0-255 on each of the
//      * Y'CbCr components
//      */
//     V4L2_COLORSPACE_JPEG          = 7,
//
//     /* For RGB colourspaces, this is probably a good start. */
//     V4L2_COLORSPACE_SRGB          = 8,
// };
string v4l2ColorspaceToString(uint8_t colorspace) {
    switch (colorspace) {
        case V4L2_COLORSPACE_SMPTE170M:
            return "smpte170m";
        case V4L2_COLORSPACE_SMPTE240M:
            return "smpte240m";
        case V4L2_COLORSPACE_REC709:
            return "rec709";
        case V4L2_COLORSPACE_BT878:
            return "bt878";
        case V4L2_COLORSPACE_470_SYSTEM_M:
            return "470_system_m";
        case V4L2_COLORSPACE_470_SYSTEM_BG:
            return "470_system_bg";
        case V4L2_COLORSPACE_JPEG:
            return "jpeg";
        case V4L2_COLORSPACE_SRGB:
            return "srgb";
        default:
            return "Unknown colorspace";
    }
}

string v4l2PixFmtToString(uint32_t pixfmt) {
    switch (pixfmt) {
        // RGB formats:
        case V4L2_PIX_FMT_RGB332:
            return "rgb332";
        case V4L2_PIX_FMT_RGB444:
            return "rgb444";
        case V4L2_PIX_FMT_RGB555:
            return "rgb555";
        case V4L2_PIX_FMT_RGB565:
            return "rgb565";
        case V4L2_PIX_FMT_RGB555X:
            return "rgb555x";
        case V4L2_PIX_FMT_RGB565X:
            return "rgb565x";
        case V4L2_PIX_FMT_BGR666:
            return "rgb666";
        case V4L2_PIX_FMT_BGR24:
            return "bgr24";
        case V4L2_PIX_FMT_RGB24:
            return "rgb24";
        case V4L2_PIX_FMT_BGR32:
            return "bgr32";
        case V4L2_PIX_FMT_RGB32:
            return "rgb32";
        /* Luminance+Chrominance formats */
        case V4L2_PIX_FMT_YVU410:
            return "yvu410";
        case V4L2_PIX_FMT_YVU420:
            return "yvu420";
        case V4L2_PIX_FMT_YUYV:
            return "yuyv";
        case V4L2_PIX_FMT_YYUV:
            return "yyuv";
        case V4L2_PIX_FMT_YVYU:
            return "yvyu";
        case V4L2_PIX_FMT_UYVY:
            return "uyvy";
        case V4L2_PIX_FMT_VYUY:
            return "vyuy";
        case V4L2_PIX_FMT_YUV422P:
            return "yuv422p";
        case V4L2_PIX_FMT_YUV411P:
            return "yuv411p";
        case V4L2_PIX_FMT_Y41P:
            return "y41p";
        case V4L2_PIX_FMT_YUV444:
            return "yuv444";
        case V4L2_PIX_FMT_YUV555:
            return "yuv555";
        case V4L2_PIX_FMT_YUV565:
            return "yuv565";
        case V4L2_PIX_FMT_YUV32:
            return "yuv32";
        case V4L2_PIX_FMT_YUV410:
            return "yuv410";
        case V4L2_PIX_FMT_YUV420:
            return "yuv420";
        case V4L2_PIX_FMT_HI240:
            return "hi240";
        case V4L2_PIX_FMT_HM12:
            return "hm12";
        case V4L2_PIX_FMT_M420:
            return "m420";
        /* two planes -- one Y, one Cr + Cb interleaved  */
        case V4L2_PIX_FMT_NV12:
            return "nv12";
        case V4L2_PIX_FMT_NV21:
            return "nv21";
        case V4L2_PIX_FMT_NV16:
            return "nv16";
        case V4L2_PIX_FMT_NV61:
            return "nv61";
        case V4L2_PIX_FMT_NV24:
            return "nv24";
        case V4L2_PIX_FMT_NV42:
            return "nv42";
        default:
            return "Unknown pixfmt";
    }
}

// enum v4l2_field {
//     V4L2_FIELD_ANY           = 0, /* driver can choose from none, top,
//                                      bottom, interlaced depending on
//                                      whatever it thinks
//                                      is approximate ... */
//     V4L2_FIELD_NONE          = 1, /* this device has no fields ... */
//     V4L2_FIELD_TOP           = 2, /* top field only */
//     V4L2_FIELD_BOTTOM        = 3, /* bottom field only */
//     V4L2_FIELD_INTERLACED    = 4, /* both fields interlaced */
//     V4L2_FIELD_SEQ_TB        = 5, /* both fields sequential into one
//                                      buffer, top-bottom order */
//     V4L2_FIELD_SEQ_BT        = 6, /* same as above + bottom-top order*/
//     V4L2_FIELD_ALTERNATE     = 7, /* both fields alternating into
//                                      separate buffers */
//     V4L2_FIELD_INTERLACED_TB = 8, /* both fields interlaced, top field
//                                      first and the top field is
//                                      transmitted first */
//     V4L2_FIELD_INTERLACED_BT = 9, /* both fields are interlaced, top
//                                      field first and the bottom field
//                                      is transmitted first */
// };
string v4l2FieldToString(uint32_t field) {
    switch (field) {
        case V4L2_FIELD_ANY:
            return "any";
        case V4L2_FIELD_NONE:
            return "none";
        case V4L2_FIELD_TOP:
            return "top";
        case V4L2_FIELD_BOTTOM:
            return "bottom";
        case V4L2_FIELD_INTERLACED:
            return "interlaced";
        case V4L2_FIELD_SEQ_TB:
            return "seq_tb";
        case V4L2_FIELD_SEQ_BT:
            return "seq_bt";
        case V4L2_FIELD_ALTERNATE:
            return "alternate";
        case V4L2_FIELD_INTERLACED_TB:
            return "interlaced_tb";
        case V4L2_FIELD_INTERLACED_BT:
            return "interlaced_bt";
        default:
            return "Unknown field";
    }
}

/*
 * typedef enum {
 *     DmdUnknown = 0,
 * 
 *     // yuv color space;
 *     DmdI420,    // Y'CbCr 4:2:0 - 420v, NV12 actually;
 *     DmdYUYV,    // Y'CbCr 4:2:2 - yuvs packed;
 *     DmdUYVY,    // Y'CbCr 4:2:2 - uyvy packed;
 *     DmdNV12,    // Y'CbCr 4:2:0 - nv12 planar;
 *     DmdNV21,    // Y'CbCr 4:2:0 - nv21 planar;
 * 
 *     // RGB color space;
 *     DmdRGB24,
 *     DmdBGR24,
 *     DmdRGBA32,
 *     DmdBGRA32,
 * } DmdVideoType;
 */
uint32_t v4l2DmdVideoTypeToPixelFormat(DmdVideoType videoType) {
    uint32_t pixelFormat = 0;
    switch (videoType) {
        // YUV color space;
        case DmdI420:
            pixelFormat = V4L2_PIX_FMT_YUV420;
            break;
        case DmdYUYV:
            pixelFormat = V4L2_PIX_FMT_YUYV;
            break;
        case DmdUYVY:
            pixelFormat = V4L2_PIX_FMT_UYVY;
            break;
        case DmdNV12:
            pixelFormat = V4L2_PIX_FMT_NV12;
            break;
        case DmdNV21:
            pixelFormat = V4L2_PIX_FMT_NV21;
            break;

        // RGB color space;
        case DmdRGB24:
            pixelFormat = V4L2_PIX_FMT_RGB24;
            break;
        case DmdBGR24:
            pixelFormat = V4L2_PIX_FMT_BGR24;
            break;
        case DmdRGBA32:
            pixelFormat = V4L2_PIX_FMT_RGB32;
            break;
        case DmdBGRA32:
            pixelFormat = V4L2_PIX_FMT_BGR32;
            break;

        default:
            pixelFormat = 0;  // unsupported pixel format;
            break;
    }

    return pixelFormat;
}

/*
 *  Flags for 'capability' and 'capturemode' fields
 *  #define V4L2_MODE_HIGHQUALITY 0x0001  //  High quality imaging mode
 *  #define V4L2_CAP_TIMEPERFRAME 0x1000  //  timeperframe field is supported
 */
string v4l2StreamParamToString(uint32_t streamparam) {
    ostringstream os;
    string strParam;

    if (0x0 == streamparam) {
        os << "0";
    } else {
        if (streamparam & V4L2_MODE_HIGHQUALITY) {
            os << "support hight quality mode ";
        }

        if (streamparam & V4L2_CAP_TIMEPERFRAME) {
            os << "support timeperframe capture ";
        }
    }

    strParam = os.str();

    return strParam;
}

}  // namespace opendmd

