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

#include "CDmdV4L2Impl.h"

namespace opendmd {

CDmdCaptureEngineLinux::CDmdCaptureEngineLinux() : m_pV4L2Impl(NULL),
        m_bStartCapture(false) {
    memset(&m_capVideoFormat, 0, sizeof(m_capVideoFormat));
}

CDmdCaptureEngineLinux::~CDmdCaptureEngineLinux() {
    if (m_pV4L2Impl) {
        delete m_pV4L2Impl;
        m_pV4L2Impl = NULL;
    }
}

DMD_RESULT CDmdCaptureEngineLinux::Init(const DmdCaptureVideoFormat
        &capVideoFormat) {
    DMD_LOG_INFO("CDmdCaptureEngineLinux::Init()"
            << ", capVideoFormat.eVideoType = "
            << dmdVideoType[capVideoFormat.eVideoType]
            << ", capVideoFormat.iWidth = " << capVideoFormat.iWidth
            << ", capVideoFormat.iHeight = " << capVideoFormat.iHeight
            << ", capVideoFormat.fFrameRate = " << capVideoFormat.fFrameRate
            << ", capVideoFormat.sVideoDevice = "
            << capVideoFormat.sVideoDevice);
    memcpy(&m_capVideoFormat, &capVideoFormat, sizeof(capVideoFormat));

    if (m_pV4L2Impl) {
        delete m_pV4L2Impl;
        m_pV4L2Impl = NULL;
    }
    m_pV4L2Impl = new CDmdV4L2Impl();
    if (!m_pV4L2Impl) {
        DMD_LOG_ERROR("CDmdCaptureEngineLinux::Init(), "
                << "create m_pV4L2Impl failed");
        return DMD_S_FAIL;
    }
    m_pV4L2Impl->Init(m_capVideoFormat);

    return DMD_S_OK;
}

DMD_RESULT CDmdCaptureEngineLinux::Uninit() {
    if (m_pV4L2Impl) {
        m_pV4L2Impl->Uninit();
    }

    return DMD_S_OK;
}

DMD_RESULT CDmdCaptureEngineLinux::StartCapture() {
    if (!m_pV4L2Impl) {
        DMD_LOG_ERROR("CDmdCaptureEngineLinux::StartCapture(), "
                << "m_pV4L2Impl == NULL");
        return DMD_S_FAIL;
    }

    DMD_RESULT result = m_pV4L2Impl->StartCapture();
    m_bStartCapture = result == DMD_S_OK ? true : false;

    // result = m_pV4L2Impl->CaptureRunloop();

    return result;
}

DMD_BOOL CDmdCaptureEngineLinux::IsCapturing() {
    return m_bStartCapture;
}

DMD_RESULT CDmdCaptureEngineLinux::StopCapture() {
    if (!m_pV4L2Impl) {
        DMD_LOG_ERROR("CDmdCaptureEngineLinux::StopCapture(), "
                << "m_pV4L2Impl == NULL");
        return DMD_S_FAIL;
    }

    DMD_RESULT result = m_pV4L2Impl->StopCapture();
    m_bStartCapture = false;

    return result;
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
    if (access(devicePath, F_OK) == -1) {
        DMD_LOG_ERROR("Video device " << devicePath << " did not exist:"
                << strerror(errno));
        return NULL;
    }

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
    *ppVideoCapEngine = pLinuxVideoCapEngine;

    return DMD_S_OK;
}

DMD_RESULT ReleaseVideoCaptureEngine(IDmdCaptureEngine **ppVideoCapEngine) {
    if ((NULL == ppVideoCapEngine) || (NULL == *ppVideoCapEngine)) {
        DMD_LOG_ERROR("ReleaseVideoCaptureEngine(), invalid videoCapEngine.");
        return DMD_S_FAIL;
    }

    delete (*ppVideoCapEngine);
    *ppVideoCapEngine = NULL;

    return DMD_S_OK;
}

}  // namespace opendmd

