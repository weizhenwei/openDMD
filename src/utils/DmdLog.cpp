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

#include <glog/logging.h>

#include "DmdLog.h"

namespace opendmd {

DmdLog *DmdLog::s_Log = new DmdLog(DMD_LOG_LEVEL_INFO);

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
    // google::SetLogDestination(google::GLOG_INFO, "opendmd-");
    FLAGS_logbufsecs = 0;
    FLAGS_max_log_size = 10;
    // google::SetStderrLogging(google::GLOG_INFO);
    google::SetStderrLogging(m_uLevel);
}

void DmdLog::Log(DMD_LOG_LEVEL_T level, const char *file, int line,
        const char *format, ...) {
    if (level > m_uLevel)
        return;

    // glog library is said thread-safe, so there is no need mutex here.
    std::string msg(256, '\0');  // TODO(weizhenwei): truncate problem to fix.
    va_list var_list;
    va_start(var_list, format);
    va_end(var_list);
    vsnprintf((char *)msg.c_str(), 256, format, var_list);
    // extend LOG(XX) macro to the following line.
    google::LogMessage(file, line, m_uLevel).stream() << msg.c_str();
}

DmdLog* DmdLog::singleton() {
    CHECK_NOTNULL(s_Log);
    return s_Log;
}

}  // namespace opendmd
