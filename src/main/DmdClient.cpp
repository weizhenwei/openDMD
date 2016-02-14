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

#include <assert.h>
#include <pthread.h>

#include "DmdLog.h"
#include "IDmdDatatype.h"
#include "IDmdCaptureEngine.h"
#include "CDmdCaptureEngine.h"
#include "DmdSignal.h"

#include "thread/DmdThreadUtils.h"
#include "thread/DmdThread.h"
#include "thread/DmdThreadManager.h"

#include "DmdClient.h"
#include "main.h"

namespace opendmd {

static bool g_bMainThreadRunning = true;

void initGlobal() {
    g_ThreadManager = DmdThreadManager::singleton();
}

void *captureThreadRoutine(void *param) {
    DMD_LOG_INFO("At the beginning of capture thread function");

    DmdCaptureVideoFormat capVideoFormat = {DmdUnknown, 0, 0, 0, {0}};
    capVideoFormat.eVideoType = DmdI420;
    capVideoFormat.iWidth = 1280;
    capVideoFormat.iHeight = 720;
    capVideoFormat.fFrameRate = 30.0f;

    char *pDeviceName = GetDeviceName();
    if (NULL == pDeviceName) {
        DMD_LOG_ERROR("captureThreadRoutine(), "
                << "could not get capture device name");
        pthread_exit(NULL);
    }

    DMD_LOG_INFO("captureThreadRoutine(), "
            << "Get video device name = " << pDeviceName);
    strncpy(capVideoFormat.sVideoDevice, pDeviceName, strlen(pDeviceName));

    IDmdCaptureEngine *pVideoCapEngine = NULL;
    CreateVideoCaptureEngine(&pVideoCapEngine);
    pVideoCapEngine->Init(capVideoFormat);
    pVideoCapEngine->StartCapture();
    // while (1);  // capture data test;
    pVideoCapEngine->StopCapture();
    pVideoCapEngine->Uninit();
    ReleaseVideoCaptureEngine(&pVideoCapEngine);
    pVideoCapEngine = NULL;
}

static void DmdSIGINTHandler(int signal) {
    assert(signal == SIGINT);

    DMD_LOG_INFO("DmdSIGINTHandler(), SIGINT processing");
    g_bMainThreadRunning = false;
}

int client_main(int argc, char *argv[]) {
    DMD_LOG_INFO("At the beginning of client_main function");

    initGlobal();
    DmdRegisterDefaultSignal();
    DmdSignalHandler pHandler = DmdSIGINTHandler;
    DmdRegisterSignalHandler(SIGINT, pHandler);

    // create capture thread;
    DmdThreadType eCaptureThread = DMD_THREAD_CAPTURE;
    DmdThreadRoutine pRoutine = captureThreadRoutine;
    g_ThreadManager->addThread(eCaptureThread, pRoutine);

    // spawn all working thread;
    // g_ThreadManager->spawnThread(eCaptureThread);

    // while (g_bMainThreadRunning) {
    //     DMD_LOG_INFO("client_main(), main thread is running");
    // }

    // clean all working thread;
    // g_ThreadManager->cleanThread(eCaptureThread);

    g_ThreadManager->cleanAllThread();

    return DMD_S_OK;
}

}  // namespace opendmd

