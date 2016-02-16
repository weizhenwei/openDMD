/*
 ============================================================================
 Name        : DmdThreadManager.h
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
 * Description : thread manager header file.
 ============================================================================
 */

#ifndef SRC_UTIL_THREAD_DMDTHREADMANAGER_H
#define SRC_UTIL_THREAD_DMDTHREADMANAGER_H

#include <pthread.h>
#include <list>

#include "IDmdDatatype.h"

#include "DmdThread.h"
#include "DmdThreadMutex.h"
#include "DmdThreadUtils.h"

namespace opendmd {

class DmdThreadManager;
extern DmdThreadManager *g_ThreadManager;

class DmdThreadManager {
public:
    DmdThreadManager();
    ~DmdThreadManager();

    DMD_RESULT addThread(DmdThreadType eType, DmdThreadRoutine pRoutine);
    DmdThread *getThread(DmdThreadType eType);
    DMD_RESULT spawnThread(DmdThreadType eType);
    DMD_RESULT spawnAllThreads();

    DMD_RESULT killThread(DmdThreadType eType);
    DMD_RESULT killAllThreads();

    void cleanThread(DmdThreadType eType);
    void cleanAllThreads();

    static DmdThreadManager *singleton();

private:
    static DmdThreadManager *s_ThreadManager;

    typedef std::list<DmdThread*> DmdThreadList;
    typedef std::list<DmdThread*>::iterator DmdThreadListIterator;

    DmdThreadList m_listThreadList;
    DmdThreadMutex m_mtxThreadManagerMutex;
};

}  // namespace opendmd

#endif  // SRC_UTIL_THREAD_DMDTHREADMANAGER_H

