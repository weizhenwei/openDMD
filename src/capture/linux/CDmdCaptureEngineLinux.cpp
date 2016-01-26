/*
 ============================================================================
 Name        : CDmdCaptureEngineLinux.cpp
 Author      : weizhenwei, <weizhenwei1988@gmail.com>
 Date           :2015.07.18
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
 Description : implementation file of capture engine on linux platform.
 ============================================================================
 */

#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#include "CDmdCaptureEngine.h"
#include "CDmdCaptureEngineLinux.h"
#include "DmdLog.h"

namespace opendmd {

CDmdCaptureEngineLinux::CDmdCaptureEngineLinux() {
    memset(&m_capVideoFormat, 0, sizeof(m_capVideoFormat));
}

CDmdCaptureEngineLinux::~CDmdCaptureEngineLinux() {
    Uninit();
}

DMD_RESULT CDmdCaptureEngineLinux::Init(DmdCaptureVideoFormat&
        capVideoFormat) {
    DMD_LOG_INFO("CDmdCaptureEngineLinux::Init()"
            ", capVideoFormat.eVideoType = " << capVideoFormat.eVideoType
            << ", capVideoFormat.iWidth = " << capVideoFormat.iWidth
            << ", capVideoFormat.iHeight = " << capVideoFormat.iHeight
            << ", capVideoFormat.fFrameRate = " << capVideoFormat.fFrameRate
            << ", capVideoFormat.sVideoDevice = "
            << capVideoFormat.sVideoDevice);
    memcpy(&m_capVideoFormat, &capVideoFormat, sizeof(capVideoFormat));

    return DMD_S_OK;
}

DMD_RESULT CDmdCaptureEngineLinux::Uninit() {
    return DMD_S_OK;
}

DMD_RESULT CDmdCaptureEngineLinux::StartCapture() {
    return DMD_S_OK;
}

DMD_BOOL CDmdCaptureEngineLinux::IsCapturing() {
    return false;
}

DMD_RESULT CDmdCaptureEngineLinux::StopCapture() {
    return DMD_S_OK;
}

DMD_RESULT CDmdCaptureEngineLinux::DeliverVideoData(
        DmdVideoRawData *pVideoRawData) {
    return DMD_S_OK;
}


// public interface implementation defined at CDmdCaptureEngine.h
// global function definition;
char *GetDeviceName() {
    static char deviceName[256];
    const char *devicePath = "/dev/video0";
    int fd = -1;
    if ((fd = open(devicePath, O_RDWR)) == -1) {
        DMD_LOG_ERROR("Video device " << devicePath << " is not available:"
                << strerror(errno));
        return NULL;
    } else {
        close(fd);
        strncpy(deviceName, devicePath, strlen(devicePath));
        return deviceName;
    }
}

DMD_RESULT CreateVideoCaptureEngine(IDmdCaptureEngine **ppVideoCapEngine) {
    if (NULL == ppVideoCapEngine) {
        return DMD_S_FAIL;
    }

    CDmdCaptureEngineLinux *pLinuxVideoCapEngine =
        new CDmdCaptureEngineLinux();
    DMD_CHECK_NOTNULL(pLinuxVideoCapEngine);
    *ppVideoCapEngine = (IDmdCaptureEngine *)pLinuxVideoCapEngine;

    return DMD_S_OK;
}

DMD_RESULT ReleaseVideoCaptureEngine(IDmdCaptureEngine **ppVideoCapEngine) {
    ppVideoCapEngine = NULL;
    return DMD_S_OK;
}

}  // namespace opendmd

