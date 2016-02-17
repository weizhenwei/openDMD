/*
 ============================================================================
 Name        : DmdClientThreads.cpp
 Author      : weizhenwei, <weizhenwei1988@gmail.com>
 Date           :2016.02.16
 Copyright   :
 * Copyright (c) 2016, weizhenwei
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
 * Description : thread model implementation of client program;
 ============================================================================
 */

#include <assert.h>
#include <pthread.h>
#include <signal.h>

#include "DmdLog.h"
#include "CDmdCaptureThread.h"

#include "thread/DmdThreadUtils.h"
#include "thread/DmdThread.h"
#include "thread/DmdThreadManager.h"

namespace opendmd {

bool g_bMainThreadRunning = true;

void InitSignal() {
    int ret = -1;
    sigset_t blockedSignalSet;
    sigemptyset(&blockedSignalSet);
    sigaddset(&blockedSignalSet, SIGINT);  // block SIGINT for sigwait;
    if (0 != (ret = pthread_sigmask(SIG_BLOCK, &blockedSignalSet, NULL))) {
        DMD_LOG_WARNING("initSignal(), call pthread_sigmask error:"
                        << ret);
    }
}

void *SignalManagerThreadRoutine(void *param) {
    int ret = -1;
    int sig = -1;
    DMD_LOG_INFO("At the beginning of signal manager thread function");

    sigset_t sigwaitSet;
    sigemptyset(&sigwaitSet);
    sigaddset(&sigwaitSet, SIGINT);  // wait SIGINT;
    while (1) {
        ret = sigwait(&sigwaitSet, &sig);
        if (ret == 0) {
            assert(sig == SIGINT);
            DMD_LOG_INFO("SignalManagerThreadRoutine(), "
                         "receive signal " << sig);
            g_bCaptureThreadRunning = false;
            g_bMainThreadRunning = false;
            break;
        }
    }  // while

    DMD_LOG_INFO("SignalManagerThreadRoutine(), "
                 << "signal manager thread is exiting");
    return NULL;
}

void CreateAndSpawnThreads() {
    // create signal manager thread;
    DmdThreadType eSignalManagerThread = DMD_THREAD_SIGMGR;
    DmdThreadRoutine pSigMgrRoutine = SignalManagerThreadRoutine;
    g_ThreadManager->addThread(eSignalManagerThread, pSigMgrRoutine);

    // create capture thread;
    DmdThreadType eCaptureThread = DMD_THREAD_CAPTURE;
    DmdThreadRoutine pCaptureRoutine = CaptureThreadRoutine;
    g_ThreadManager->addThread(eCaptureThread, pCaptureRoutine);

    // spawn all working thread;
    g_ThreadManager->spawnAllThreads();
}

void ExitAndCleanThreads() {
    // send signal to all threads;
    g_ThreadManager->killAllThreads();

    // clean all working thread;
    g_ThreadManager->cleanAllThreads();
}

}  // namespace opendmd

