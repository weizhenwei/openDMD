/*
 ============================================================================
 Name        : CDmdCaptureDeviceLinux.cpp
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
 Description : concrete implementation file of capture device on linux platform.
 ============================================================================
 */


#include "CDmdCaptureDeviceLinux.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <string>

#include "utils/DmdLog.h"

namespace opendmd {

CDmdCaptureDeviceLinux::CDmdCaptureDeviceLinux() {
    memset(&m_V4L2_info, '\0', sizeof(struct v4l2_device_info));
    m_strDeviceName = string("");
    m_bDeviceNameSet = false;
    m_bCaptureOn = false;
}

CDmdCaptureDeviceLinux::~CDmdCaptureDeviceLinux() {
    // TODO(weizhenwei): add more delete operation on m_pV4L2_info's members;
    if (m_V4L2_info.video_device_path) {
        delete [] m_V4L2_info.video_device_path;
        m_V4L2_info.video_device_path = NULL;
    }
}

DMD_RESULT CDmdCaptureDeviceLinux::init(const char *deviceName) {
    if (m_bCaptureOn) {
        DMD_LOG_ERROR("Capture Device " << m_strDeviceName
                << "is already running\n");
        return DMD_S_FAIL;
    }

    if (!m_strDeviceName.empty()) {
        m_strDeviceName.clear();
    }
    m_strDeviceName.assign(deviceName);

    if (m_V4L2_info.video_device_path) {
        delete m_V4L2_info.video_device_path;
    }
    size_t strLen = strlen(deviceName);
    char *pDeviceName = new char[strLen+1];
    if (!pDeviceName) {
        DMD_LOG_ERROR("new pDeviceName = NULL");
        return DMD_S_FAIL;
    }
    memset(pDeviceName, '\0', strLen + 1);
    strcpy(pDeviceName, deviceName);
    m_V4L2_info.video_device_path = pDeviceName;

    m_bCaptureOn = true;

    return DMD_S_OK;
}

DMD_RESULT CDmdCaptureDeviceLinux::setDeviceName(const char *deviceName) {
    if (NULL != deviceName) {
        m_strDeviceName = deviceName;
        m_bDeviceNameSet = true;

        return DMD_S_OK;
    } else {
        m_bDeviceNameSet = false;
        return DMD_S_FAIL;
    }
}
DMD_RESULT CDmdCaptureDeviceLinux::getDeviceName(char **deviceName) {
    if (m_bDeviceNameSet) {
        *deviceName =(char *) m_strDeviceName.c_str();
        return DMD_S_OK;
    } else {
        *deviceName = NULL;
        return DMD_S_FAIL;
    }
}
DMD_BOOL    CDmdCaptureDeviceLinux:: isDeviceNameSet() {
    return m_bDeviceNameSet;
}

// global function definition;
const char *GetDeviceName() {
    const char *devicePath = "/dev/video0";
    int fd = -1;
    if ((fd = open(devicePath, O_RDWR)) == -1) {
        DMD_LOG_ERROR("Video device " << devicePath << " is not available:"
                << strerror(errno));
        return NULL;
    } else {
        close(fd);
        return devicePath;
    }
}

}  // namespace opendmd
