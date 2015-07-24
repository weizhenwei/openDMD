/*
 ============================================================================
 Name        : CDmdCaptureDeviceMac.mm
 Author      : weizhenwei, <weizhenwei1988@gmail.com>
 Date           :2015.07.07
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
 Description : concrete implementation file of capture device on mac platform.
 ============================================================================
 */

#import "CDmdCaptureDeviceMac.h"

namespace opendmd {

CDmdCaptureDeviceMac::CDmdCaptureDeviceMac(): m_sDeviceName(nil), m_idDevice(nil) {
    // TODO(weizhenwei): Auto-generated constructor stub
}

CDmdCaptureDeviceMac::~CDmdCaptureDeviceMac() {
    // TODO(weizhenwei): Auto-generated destructor stub
}

DMD_S_RESULT CDmdCaptureDeviceMac::init(const char *deviceName) {
//    m_sDeviceName = [NSString stringWithUTF8String:deviceName];

    return DMD_S_OK;
}

DMD_S_RESULT CDmdCaptureDeviceMac::setDeviceName(const char *deviceName) {
    return DMD_S_OK;
}
DMD_S_RESULT CDmdCaptureDeviceMac::getDeviceName(char *deviceName) {
    return DMD_S_OK;
}
DMD_S_RESULT CDmdCaptureDeviceMac::initDevice(const char *deviceName) {
    return DMD_S_OK;
}

// TODO(weizhenwei): get device name in mac way.
const char *GetDeviceName() {
    const char *devicePath = "/dev/video0";
    int fd = -1;
    if ((fd = open(devicePath, O_RDWR)) == -1) {
        DMD_LOG(DMD_LOG_LEVEL_ERROR, "Video device " << devicePath << " is not available.");
        return NULL;
    } else {
        return devicePath;
    }
}

}  // namespace opendmd
