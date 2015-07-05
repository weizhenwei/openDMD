/*
 ============================================================================
 Name        : DmdLog.h
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
 Description : log util header file.
 ============================================================================
 */

#ifndef SRC_UTILS_DMDLOG_H
#define SRC_UTILS_DMDLOG_H

#include <string>
#include <sstream>

#include "utils/DmdMutex.h"

using std::string;
using std::stringstream;


namespace opendmd {
typedef enum {
    DMD_LOG_LEVEL_INFO = 0,
    DMD_LOG_LEVEL_WARNING = 1,
    DMD_LOG_LEVEL_ERROR = 2,
    DMD_LOG_LEVEL_FATAL = 3,
} DMD_LOG_LEVEL_T;

class DmdLog {
public:
    void Log(DMD_LOG_LEVEL_T level, const char *file, int line,
            const string &msg);

    static DmdLog* singleton();

private:
    // constructor and destructor as private to make DmdLog Singleton.
    DmdLog();
    explicit DmdLog(DMD_LOG_LEVEL_T logLevel);
    virtual ~DmdLog();

    DMD_LOG_LEVEL_T m_uLevel;
    void initGLog();

    static DmdMutex s_Mutex;
    static DmdLog* s_Log;
};

#define DMD_LOG(level, msg) \
    do { \
        stringstream strstream; \
        strstream << msg; \
        DmdLog::singleton()->Log(level, __FILE__, __LINE__, strstream.str()); \
    } while (0)

#define DMD_LOG_INFO(msg) DMD_LOG(DMD_LOG_LEVEL_INFO, msg)
#define DMD_LOG_WARNING(msg) DMD_LOG(DMD_LOG_LEVEL_WARNING, msg)
#define DMD_LOG_ERROR(msg) DMD_LOG(DMD_LOG_LEVEL_ERROR, msg)
#define DMD_LOG_FATAL(msg) DMD_LOG(DMD_LOG_LEVEL_FATAL, msg)


}  // namespace opendmd

#endif  // SRC_UTILS_DMDLOG_H

