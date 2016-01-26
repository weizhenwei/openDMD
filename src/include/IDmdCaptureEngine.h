/*
 ============================================================================
 Name        : IDmdCaptureEngine.h
 Author      : weizhenwei, <weizhenwei1988@gmail.com>
 Date           :2015.06.26
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
 Description : base class interface of video capture engine..
 ============================================================================
 */

#ifndef SRC_INCLUDE_IDMDCAPTUREENGINE_H
#define SRC_INCLUDE_IDMDCAPTUREENGINE_H

#include "IDmdDatatype.h"

const unsigned int uiDeviceNameLength = 256;

namespace opendmd {

typedef struct {
    DmdVideoType    eVideoType;
    unsigned int    iWidth;
    unsigned int    iHeight;
    float           fFrameRate;
    char            sVideoDevice[uiDeviceNameLength];
} DmdCaptureVideoFormat;

class IDmdCaptureEngine {
 public:
    IDmdCaptureEngine() {}
    virtual ~IDmdCaptureEngine() {}

    virtual DMD_RESULT Init(DmdCaptureVideoFormat& capVideoFormat) = 0;
    virtual DMD_RESULT Uninit() = 0;

    virtual DMD_RESULT StartCapture() = 0;
    virtual DMD_BOOL   IsCapturing() = 0;
    virtual DMD_RESULT StopCapture() = 0;
};

class IDmdCaptureEngineSink {
    virtual DMD_RESULT DeliverVideoData(DmdVideoRawData *pVideoRawData) = 0;
};

}  // namespace opendmd

#endif  // SRC_INCLUDE_IDMDCAPTUREENGINE_H

