/*
 ============================================================================
 Name        : IDmdDatatype.h
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
 Description : header file to define various data type.
 ============================================================================
 */

#ifndef SRC_INCLUDE_IDMDDATATYPE_H
#define SRC_INCLUDE_IDMDDATATYPE_H

#include <stdint.h>  // uint64_t series is defined in this header file;
#include <stddef.h>  // "NULL" is defined in this header file!

namespace opendmd {

#define DMD_BOOL bool

typedef enum {
    DMD_S_OK = 0x0,
    DMD_S_FAIL = 0x1,
} DMD_RESULT;

typedef enum {
    DmdUnknown = 0,

    // yuv color space;
    DmdI420,    // Y'CbCr 4:2:0 - 420v, NV12 actually;
    DmdYUYV,    // Y'CbCr 4:2:2 - yuvs packed;
    DmdUYVY,    // Y'CbCr 4:2:2 - uyvy packed;
    DmdNV12,    // Y'CbCr 4:2:0 - nv12 planar;
    DmdNV21,    // Y'CbCr 4:2:0 - nv21 planar;

    // rgb color space;
    DmdRGB24,
    DmdBGR24,
    DmdRGBA32,
    DmdBGRA32,
} DmdVideoType;

static const char *dmdVideoType[] = {
    "DmdUnknown",

    // yuv color space;
    "DmdI420",    // Y'CbCr 4:2:0 - 420v, NV12 actually;
    "DmdYUYV",    // Y'CbCr 4:2:2 - yuvs;
    "DmdUYVY",    // Y'CbCr 4:2:2 - uyvy;
    "DmdNV12",    // Y'CbCr 4:2:0 - nv12 planar;
    "DmdNV21",    // Y'CbCr 4:2:0 - nv21 planar;

    // rgb color space;
    "DmdRGB24",
    "DmdBGR24",
    "DmdRGBA32",
    "DmdBGRA32",
};
#define DMDVIDEOTYPE(type) dmdVideoType[type]


typedef struct {
    DmdVideoType    eVideoType;
    unsigned int    iWidth;
    unsigned int    iHeight;
    float           fFrameRate;
    uint64_t        ulTimestamp;
} DmdVideoFormat;

#define MAX_PLANE_COUNT 3
#define MAX_PLANAR_NUM 4

typedef struct {
    unsigned char   *pSrcData[MAX_PLANAR_NUM];
    size_t          ulSrcStride[MAX_PLANAR_NUM];
    size_t          ulSrcDatalen[MAX_PLANAR_NUM];
    DmdVideoFormat  fmtVideoFormat;
    size_t          ulPlaneCount;
    unsigned int    ulRotation;
    size_t          ulDataLen;
} DmdVideoRawData;

}  // namespace opendmd

#endif  // SRC_INCLUDE_IDMDDATATYPE_H

