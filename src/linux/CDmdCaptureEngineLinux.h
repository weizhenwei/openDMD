/*
 ============================================================================
 Name        : CDmdCaptureEngineLinux.h
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
 Description : header file of capture engine on linux platform.
 ============================================================================
 */

#ifndef SRC_LINUX_CDMDCAPTUREENGINELINUX_H
#define SRC_LINUX_CDMDCAPTUREENGINELINUX_H

#include "IDmdCaptureDevice.h"
#include "IDmdCaptureEngine.h"

namespace opendmd {

class CDmdCaptureEngineLinux : public IDmdCaptureEngine {
public:
    CDmdCaptureEngineLinux();
    ~CDmdCaptureEngineLinux();

    // IDmdCaptureDevice interface;
    DMD_S_RESULT initEngine();
    DMD_S_RESULT startCapture();
    DMD_S_RESULT stopCapture();

private:
    IDmdCaptureDevice *m_pCaptureDevice;
};

}  // namespace opendmd

#endif  // SRC_LINUX_CDMDCAPTUREENGINELINUX_H
