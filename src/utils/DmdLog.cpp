/*
 ============================================================================
 Name        : DmdLog.cpp
 Author      : weizhenwei, <weizhenwei1988@gmail.com>
 Date           :2015.06.28
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
 Description : log util implementation file.
 ============================================================================
 */

#include <stdio.h>
#include <tr1/stdarg.h>  // c++ standard header file;
#include "DmdLog.h"

namespace opendmd {

// TODO(weizhenwei): confirm that is this initialized at compile stage?
DmdMutex *DmdLog::s_Mutex = new DmdMutex();
DmdLog *DmdLog::s_Log = new DmdLog(DMD_LOG_LEVEL_INFO);

DmdLog::DmdLog() : m_uLevel(DMD_LOG_LEVEL_INFO) {
}

DmdLog::DmdLog(DMD_LOG_LEVEL_T logLevel) : m_uLevel(logLevel) {
}

DmdLog::~DmdLog() {
}

const char *DmdLog::getLevel(DMD_LOG_LEVEL_T level) {
    switch (level) {
    case DMD_LOG_LEVEL_EMERG:
        return "emerg";
    case DMD_LOG_LEVEL_ALERT:
        return "alert";
    case DMD_LOG_LEVEL_CRIT:
        return "crit";
    case DMD_LOG_LEVEL_ERR:
        return "err";
    case DMD_LOG_LEVEL_WARNING:
        return "warning";
    case DMD_LOG_LEVEL_NOTICE:
        return "notice";
    case DMD_LOG_LEVEL_INFO:
        return "info";
    case DMD_LOG_LEVEL_DEBUG:
        return "debug";
    default:
        return "NONE";
    }

    return "NONE";
}
void DmdLog::Log(DMD_LOG_LEVEL_T level, const char *format, ...) {
    if (level > m_uLevel)
        return;

    s_Mutex->Lock();
    va_list var_list;
    va_start(var_list, format);
    va_end(var_list);
    fprintf(stdout, "[%s]: ", getLevel(level));
    vfprintf(stdout, format, var_list);
    s_Mutex->Unlock();
}

DmdLog* DmdLog::singleton() {
    if (!s_Log) {
        s_Mutex->Lock();
        s_Log = new DmdLog(DMD_LOG_LEVEL_INFO);
        s_Mutex->Unlock();
        return s_Log;
    } else {
        return s_Log;
    }
}

}  // namespace opendmd
