/*
 ============================================================================
 Name        : DmdClient.cpp
 Author      : weizhenwei, <weizhenwei1988@gmail.com>
 Date           :2015.11.24
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
 Description : main entry of client program;
 ============================================================================
 */

#include <string.h>
#include <map>
#include <utility>

#include "DmdLog.h"
#include "IDmdCaptureEngine.h"
#include "CDmdCaptureEngine.h"

#include "main.h"

using namespace opendmd;
using std::map;
using std::pair;

static void CDmdCaptureEngineTest_Init() {
    IDmdCaptureEngine *pVideoCapEngine = NULL;
    CreateVideoCaptureEngine(&pVideoCapEngine);

    DmdCaptureVideoFormat capVideoFormat = {DmdUnknown, 0, 0, 0, {0}};
    char *pDeviceName = GetDeviceName();
    if (pDeviceName == NULL) {
        ReleaseVideoCaptureEngine(&pVideoCapEngine);
        pVideoCapEngine = NULL;
        return;
    }

    DmdVideoType arrVideoTypes[3] = {
        DmdI420, DmdYUYV, DmdUYVY,
    };
    map<int, int> mapResolutions;
    pair<int, int> elem = pair<int, int>(1280, 720);
    mapResolutions.insert(elem);
    elem = pair<int, int>(640, 480);
    mapResolutions.insert(elem);
    elem = pair<int, int>(320, 240);
    mapResolutions.insert(elem);

    for (map<int, int>::iterator iter = mapResolutions.begin();
         iter != mapResolutions.end(); iter++) {
        memset(&capVideoFormat, 0, sizeof(capVideoFormat));
        capVideoFormat.iWidth = iter->first;
        capVideoFormat.iHeight = iter->second;
        capVideoFormat.fFrameRate = 30;
        strncpy(capVideoFormat.sVideoDevice, pDeviceName, strlen(pDeviceName));
        for (DmdVideoType eVideoType : arrVideoTypes) {
            capVideoFormat.eVideoType = eVideoType;
            pVideoCapEngine->Init(capVideoFormat);
            pVideoCapEngine->Uninit();
        }
    }

    ReleaseVideoCaptureEngine(&pVideoCapEngine);
    pVideoCapEngine = NULL;
}

int opendmd::client_main(int argc, char *argv[]) {
    DMD_LOG_INFO("At the beginning of client_main function.\n");

    DmdCaptureVideoFormat capVideoFormat = {DmdUnknown, 0, 0, 0, {0}};
    capVideoFormat.eVideoType = DmdI420;
    capVideoFormat.iWidth = 1280;
    capVideoFormat.iHeight = 720;
    capVideoFormat.fFrameRate = 30;

    char *pDeviceName = GetDeviceName();
    if (pDeviceName == NULL) {
        return DMD_S_FAIL;
    }
    DMD_LOG_INFO("Get video device name = " << pDeviceName);
    strncpy(capVideoFormat.sVideoDevice, pDeviceName, strlen(pDeviceName));

    IDmdCaptureEngine *pVideoCapEngine = NULL;
    CreateVideoCaptureEngine(&pVideoCapEngine);
    pVideoCapEngine->Init(capVideoFormat);
    pVideoCapEngine->StartCapture();
    // while (1);  // capture data test;
    pVideoCapEngine->StopCapture();
    ReleaseVideoCaptureEngine(&pVideoCapEngine);
    pVideoCapEngine = NULL;

    return DMD_S_OK;
}

