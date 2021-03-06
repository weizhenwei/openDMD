/*
 ============================================================================
 * Name        : DmdThreadUtils.h
 * Author      : weizhenwei, <weizhenwei1988@gmail.com>
 * Date        : 2016.02.13
 *
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
 * Description : thread util header file.
 ============================================================================
 */

#ifndef SRC_UTIL_THREAD_DMDTHREADUTILS_H
#define SRC_UTIL_THREAD_DMDTHREADUTILS_H

#include <pthread.h>
#if defined(LINUX)
#include <sys/prctl.h>
#endif

namespace opendmd {

typedef pthread_mutex_t DmdThreadMutex_t;
typedef pthread_t DmdThreadHandler;

typedef void *(*DmdThreadRoutine)(void *);

typedef enum {
    DMD_THREAD_UNKNOWN,
    DMD_THREAD_SIGMGR,   // for signal manager;
    DMD_THREAD_MAIN,     // for monitor the whole openDMD running;
    DMD_THREAD_CAPTURE,  // for capture video data;
    DMD_THREAD_ENCODE,   // for encode video data;
    DMD_THREAD_NETWORK,  // for network send/recv;
    DMD_THREAD_DECODE,   // for decode video data;
} DmdThreadType;

extern const char *dmdThreadType[];

extern void DmdThreadSetName(const char *name);

}  // namespace opendmd

#endif  // SRC_UTIL_THREAD_DMDTHREADUTILS_H

