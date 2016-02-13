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
#include <stdarg.h>
#include <string>

#include "DmdLog.h"

namespace opendmd {

DmdThreadMutex DmdLog::s_Mutex;
DmdLog* DmdLog::s_Log = NULL;


DmdLog::DmdLog() : m_uLevel(DMD_LOG_LEVEL_INFO) {
    initGLog();
}

DmdLog::DmdLog(DMD_LOG_LEVEL_T logLevel) : m_uLevel(logLevel) {
    initGLog();
}

DmdLog::~DmdLog() {
}

void DmdLog::initGLog() {
    google::InitGoogleLogging("");
    google::SetStderrLogging(m_uLevel);
#ifndef DEBUG
    google::SetLogDestination(google::GLOG_INFO, "opendmd-log-");
#endif
    FLAGS_logbufsecs = 0;  // flush directly;
    FLAGS_max_log_size = 100;
    FLAGS_stop_logging_if_full_disk = true;
}

void DmdLog::Log(DMD_LOG_LEVEL_T level, const char *file, int line,
        const string &msg) {
    if (level < m_uLevel)
        return;

    // extend LOG(XX) macro to the following line.
    google::LogMessage(file, line, m_uLevel).stream() << msg;
}

DmdLog* DmdLog::singleton() {
    if (s_Log) {
        return s_Log;
    } else {
        s_Mutex.Lock();
#if defined(DEBUG)
        s_Log = new DmdLog(DMD_LOG_LEVEL_INFO);
#else
        s_Log = new DmdLog(DMD_LOG_LEVEL_ERROR);
#endif
        s_Mutex.Unlock();
        DMD_CHECK_NOTNULL(s_Log);
        return s_Log;
    }
}

}  // namespace opendmd

