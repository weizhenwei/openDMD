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
 Description : concrete implementation file of capture device on platform.
 ============================================================================
 */

#include "CDmdCaptureDeviceLinux.h"

#include "utils/DmdLog.h"

namespace opendmd {

CDmdCaptureDeviceLinux::CDmdCaptureDeviceLinux() : m_pV4L2_info(NULL) {
}

CDmdCaptureDeviceLinux::~CDmdCaptureDeviceLinux() {
    if (m_pV4L2_info) {
        delete m_pV4L2_info;
    }
}

void CDmdCaptureDeviceLinux::releaseV4L2() {
    CHECK_NOTNULL(m_pV4L2_info);
    // TODO(weizhenwei): release m_pV4L2_info;
}

DMD_S_RESULT CDmdCaptureDeviceLinux::init() {
    if (m_pV4L2_info) {
        releaseV4L2();
    }
    return DMD_S_OK;
}

DMD_S_RESULT CDmdCaptureDeviceLinux::setDevice() {
    return DMD_S_OK;
}

} /* namespace opendmd */
