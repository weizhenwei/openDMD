/*
 ============================================================================
 Name        : DmdThread.cpp
 Author      : weizhenwei, <weizhenwei1988@gmail.com>
 Date           :2016.02.13
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
 Description : thread implementation file.
 ============================================================================
 */

#include "DmdLog.h"

#include "DmdThread.h"

namespace opendmd {

DmdThread::DmdThread() : m_eThreadType(DMD_THREAD_UNKNOWN),
    m_pThreadRoutine(NULL), m_ulThreadHandler(0), bSpawned(false) {
}

DmdThread::DmdThread(DmdThreadType eType, DmdThreadRoutine pThreadRoutine)
    : m_eThreadType(eType), m_pThreadRoutine(pThreadRoutine),
    m_ulThreadHandler(0), bSpawned(false) {
}

DmdThread::~DmdThread() {
    if (bSpawned) {
        pthread_exit(NULL);
    }
}

DMD_RESULT DmdThread::spawnThread() {
    DMD_RESULT ret = DMD_S_OK;
    if (bSpawned) {
        DMD_LOG_ERROR("DmdThread::spawnThread(), "
                      << "thread with type " << dmdThreadType[m_eThreadType]
                      << "is already spawned");
        ret = DMD_S_FAIL;
        return ret;
    }

    int val = pthread_create(&m_ulThreadHandler, NULL, m_pThreadRoutine, NULL);
    if (val != 0) {
        DMD_LOG_ERROR("DmdThread::spawnThread(), call pthread_create() failed, "
                      << "error number:" << val);
        ret = DMD_S_FAIL;
        return ret;
    }

    bSpawned = true;
    DMD_LOG_INFO("DmdThread::spawnThread(), "
                 << "thread with type " << dmdThreadType[m_eThreadType]
                 << "spawned");
    return ret;
}

}  // namespace opendmd

