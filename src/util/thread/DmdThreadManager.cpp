/*
 ============================================================================
 Name        : DmdThreadManager.cpp
 Author      : weizhenwei, <weizhenwei1988@gmail.com>
 Date           :2016.02.14
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
 * Description : thread manager implementation file.
 ============================================================================
 */

#include <signal.h>
#include <pthread.h>

#include "IDmdDatatype.h"
#include "DmdLog.h"
#include "DmdThreadUtils.h"

#include "DmdThreadManager.h"

namespace opendmd {

DmdThreadManager *g_ThreadManager = NULL;
DmdThreadManager *DmdThreadManager::s_ThreadManager = NULL;

DmdThreadManager::DmdThreadManager() {
}

DmdThreadManager::~DmdThreadManager() {
    if (0 != m_listThreadList.size()) {
        m_listThreadList.clear();
    }
}

DMD_RESULT DmdThreadManager::addThread(DmdThreadType eType,
        DmdThreadRoutine pRoutine) {
    DMD_RESULT ret = DMD_S_OK;

    DmdThread *pThread = getThread(eType);
    if (NULL != pThread) {
        DMD_LOG_ERROR("DmdThreadManager::addThread(), "
                << "thread with type " << dmdThreadType[eType]
                << " already added to thread manager");
        ret = DMD_S_FAIL;
        return ret;
    }

    pThread = new DmdThread(eType, pRoutine);

    m_mtxThreadManagerMutex.Lock();
    m_listThreadList.push_back(pThread);
    m_mtxThreadManagerMutex.Unlock();

    return ret;
}

DmdThread *DmdThreadManager::getThread(DmdThreadType eType) {
    DmdThreadListIterator iter;
    for (iter = m_listThreadList.begin(); iter != m_listThreadList.end();
            iter++) {
        if (eType == (*iter)->getThreadType()) {
            return *iter;
        }
    }

    return NULL;
}

DMD_RESULT DmdThreadManager::spawnThread(DmdThreadType eType) {
    DMD_RESULT ret = DMD_S_OK;
    DmdThread *pThread = getThread(eType);
    if (NULL == pThread) {
        DMD_LOG_ERROR("DmdThreadManager::spawnThread(), "
                << "thread with type " << dmdThreadType[eType]
                << " is not added to thread manager yet");
        ret = DMD_S_FAIL;
        return ret;
    }

    if (pThread->isThreadSpawned()) {
        DMD_LOG_ERROR("DmdThreadManager::spawnThread(), "
                << "thread with type " << dmdThreadType[eType]
                << " is already spawned");
        ret = DMD_S_FAIL;
        return ret;
    }

    ret = pThread->spawnThread();
    return ret;
}

DMD_RESULT DmdThreadManager::spawnAllThreads() {
    DMD_RESULT ret = DMD_S_OK;
    DmdThread *pThread = NULL;
    DmdThreadListIterator iter;
    for (iter = m_listThreadList.begin(); iter != m_listThreadList.end();
            iter++) {
        pThread = *iter;

        DmdThreadType eType = pThread->getThreadType();
        if (pThread->isThreadSpawned()) {
            DMD_LOG_WARNING("DmdThreadManager::spawnAllThreads(), "
                    << "thread with type " << dmdThreadType[eType]
                    << " is already spawned");
            continue;
        }

        if (DMD_S_OK != (ret = pThread->spawnThread())) {
            return ret;
        }
    }

    return ret;
}

DMD_RESULT DmdThreadManager::killThread(DmdThreadType eType) {
    DMD_RESULT ret = DMD_S_OK;
    DmdThread *pThread = getThread(eType);
    if (NULL == pThread) {
        DMD_LOG_ERROR("DmdThreadManager::killThread(), "
                << "thread with type " << dmdThreadType[eType]
                << " is not added to thread manager yet");
        ret = DMD_S_FAIL;
        return ret;
    }

    if (!pThread->isThreadSpawned()) {
        DMD_LOG_ERROR("DmdThreadManager::killThread(), "
                << "thread with type " << dmdThreadType[eType]
                << " is not spawned yet");
        ret = DMD_S_FAIL;
        return ret;
    }

    int val = -1;
    DmdThreadHandler handler = pThread->getThreadHandler();
    if (0 != (val = pthread_kill(handler, SIGUSR1))) {
        DMD_LOG_ERROR("DmdThreadManager::killThread(), "
                << "could not to send SIGUSR1 signal to "
                << "thread with type " << dmdThreadType[eType]
                << ", error number:" << val);
        ret = DMD_S_FAIL;
        return ret;
    }

    if (0 != (val = pthread_join(handler, NULL))) {
        DMD_LOG_ERROR("DmdThreadManager::killThread(), "
                << "could not call pthread_join, error number:" << val);
        ret = DMD_S_FAIL;
        return ret;
    }

    return ret;
}

DMD_RESULT DmdThreadManager::killAllThreads() {
    DMD_RESULT ret = DMD_S_OK;
    DmdThread *pThread = NULL;
    DmdThreadListIterator iter;
    for (iter = m_listThreadList.begin(); iter != m_listThreadList.end();
            iter++) {
        pThread = *iter;

        DmdThreadType eType = pThread->getThreadType();
        if (!pThread->isThreadSpawned()) {
            DMD_LOG_ERROR("DmdThreadManager::killAllThreads(), "
                    << "thread with type " << dmdThreadType[eType]
                    << " is not spawned yet");
            ret = DMD_S_FAIL;
            return ret;
        }
#if 0
        int val = -1;
        DmdThreadHandler handler = pThread->getThreadHandler();
        if (0 != (val = pthread_kill(handler, SIGUSR1))) {
            DMD_LOG_ERROR("DmdThreadManager::killAllThreads(), "
                    << "could not to send SIGINT signal to "
                    << "thread with type " << dmdThreadType[eType]
                    << ", error number:" << val);
            ret = DMD_S_FAIL;
            return ret;
        }

        if (0 != (val = pthread_join(handler, NULL))) {
            DMD_LOG_ERROR("DmdThreadManager::killAllThreads(), "
                    << "could not call pthread_join, error number:" << val);
            ret = DMD_S_FAIL;
            return ret;
        }
#endif
    }

    return ret;
}

void DmdThreadManager::cleanThread(DmdThreadType eType) {
    DmdThreadListIterator iter;
    for (iter = m_listThreadList.begin(); iter != m_listThreadList.end();
            iter++) {
        if (eType == (*iter)->getThreadType()) {
            m_listThreadList.erase(iter);
            break;
        }
    }  // for
}

void DmdThreadManager::cleanAllThreads() {
    if (0 != m_listThreadList.size()) {
        m_listThreadList.clear();
    }
}

DmdThreadManager *DmdThreadManager::singleton() {
    if (NULL == s_ThreadManager) {
        s_ThreadManager = new DmdThreadManager();
    }

    return s_ThreadManager;
}

}  // namespace opendmd

