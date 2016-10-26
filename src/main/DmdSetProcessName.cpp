/*
 ============================================================================
 * Name        : DmdSetProcessName.cpp
 * Author      : weizhenwei, <weizhenwei1988@gmail.com>
 * Date        : 2016.02.29
 *
 * Copyright   :
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
 * Description : implementation file of DmdSetProcessName.cpp
 ============================================================================
 */

#include <stdlib.h>
#include <string.h>

#include <string>

#include "DmdLog.h"
#include "DmdSetProcessName.h"

extern char **environ;

namespace opendmd {

/*
 * Inspired by nginx's ngx_setproctitle.h/.cpp code:
 *
 * To change the process title in Linux and Solaris we have to set argv[1]
 * to NULL and to copy the title to the same place where the argv[0] points to.
 * However, argv[0] may be too small to hold a new title.  Fortunately, Linux
 * and Solaris store argv[] and environ[] one after another.  So we should
 * ensure that is the continuous memory and then we allocate the new memory
 * for environ[] and copy it.  After this we could use the memory starting
 * from argv[0] for our process title.
 *
 * The Solaris's standard /bin/ps does not show the changed process title.
 * You have to use "/usr/ucb/ps -w" instead.  Besides, the UCB ps does not
 * show a new title if its length less than the origin command line length.
 * To avoid it we append to a new title the origin command line in the
 * parenthesis.
 */

DmdArgv *g_pDmdArgv = NULL;
DmdEnviron *g_pDmdEnviron = NULL;

static void releaseArgc(DmdArgv **ppArgv) {
    DmdArgv *pArgv = *ppArgv;
    if (pArgv) {
        if (pArgv->base) {
            delete [] pArgv->base;
            pArgv->base = NULL;
        }

        if (pArgv->pArgv) {
            delete [] pArgv->pArgv;
            pArgv->pArgv = NULL;
        }

        if (pArgv->pOriginArgv) {
            delete [] pArgv->pOriginArgv;
            pArgv->pOriginArgv = NULL;
        }

        delete pArgv;
        pArgv = NULL;
    }
}

static void releaseEnviron(DmdEnviron **ppEnviron) {
    DmdEnviron *pEnviron = *ppEnviron;
    if (pEnviron) {
        if (pEnviron->base) {
            delete [] pEnviron->base;
            pEnviron->base = NULL;
        }

        if (pEnviron->pEnviron) {
            delete [] pEnviron->pEnviron;
            pEnviron->pEnviron = NULL;
        }

        delete pEnviron;
        pEnviron = NULL;
    }
}

DMD_RESULT InitArgvEnviron() {
    releaseArgc(&g_pDmdArgv);
    releaseEnviron(&g_pDmdEnviron);

    g_pDmdArgv = new DmdArgv;
    if (NULL == g_pDmdArgv) {
        DMD_LOG_ERROR("InitArgvEnviron(), failed to new g_pDmdArgv");
        return DMD_S_FAIL;
    }
    g_pDmdEnviron = new DmdEnviron;
    if (NULL == g_pDmdEnviron) {
        DMD_LOG_ERROR("InitArgvEnviron(), failed to new g_pDmdEnviron");
        return DMD_S_FAIL;
    }

    return DMD_S_OK;
}

DMD_RESULT UnInitArgvEnviron() {
    releaseArgc(&g_pDmdArgv);
    releaseEnviron(&g_pDmdEnviron);

    return DMD_S_OK;
}

typedef char *charP;
DMD_RESULT SaveArgv(int argc, char *argv[]) {
    if (!g_pDmdArgv) {
        DMD_LOG_ERROR("SaveArgv(), g_pDmdArgv == NULL");
        return DMD_S_FAIL;
    }
    if (g_pDmdArgv->base) {
        delete [] g_pDmdArgv->base;
        g_pDmdArgv->base = NULL;
    }
    if (g_pDmdArgv->pArgv) {
        delete [] g_pDmdArgv->pArgv;
        g_pDmdArgv->pArgv = NULL;
    }
    if (g_pDmdArgv->pOriginArgv) {
        delete [] g_pDmdArgv->pOriginArgv;
        g_pDmdArgv->pOriginArgv = NULL;
    }

    g_pDmdArgv->base = NULL;
    g_pDmdArgv->iArgc = argc;
    g_pDmdArgv->pOriginArgv = new charP[argc];
    if (NULL == g_pDmdArgv->pOriginArgv) {
        DMD_LOG_ERROR("SaveArgv(), new g_pDmdArgv->pOriginArgv error");
        return DMD_S_FAIL;
    }
    g_pDmdArgv->pArgv = new charP[argc];
    if (NULL == g_pDmdArgv->pArgv) {
        DMD_LOG_ERROR("SaveArgv(), new g_pDmdArgv->pArgv error");
        return DMD_S_FAIL;
    }

    int totallen = 0;
    for (int i = 0; i < argc; i++) {
        totallen += strlen(argv[i]) + 1;
    }

    g_pDmdArgv->base = new char[totallen];
    if (NULL == g_pDmdArgv->base) {
        DMD_LOG_ERROR("SaveArgv(), new g_pDmdArgv->base error");
        return DMD_S_FAIL;
    }
    memset(g_pDmdArgv->base, 0, totallen);

    char *pCurrent = g_pDmdArgv->base;
    for (int i = 0; i < argc; i++) {
        strncpy(pCurrent, argv[i], strlen(argv[i]));
        g_pDmdArgv->pOriginArgv[i] = argv[i];
        g_pDmdArgv->pArgv[i] = pCurrent;
        pCurrent = pCurrent + strlen(argv[i]) + 1;
    }

#ifdef DEBUG
    for (int i = 0; i < g_pDmdArgv->iArgc; i++) {
        DMD_LOG_INFO("SaveArgv(), argv[" << i << "] = "
                << g_pDmdArgv->pArgv[i]);
    }
#endif

    return DMD_S_OK;
}

DMD_RESULT SaveEnviron() {
    if (!g_pDmdEnviron) {
        DMD_LOG_ERROR("SaveEnviron(), g_pDmdEnviron == NULL");
        return DMD_S_FAIL;
    }
    if (g_pDmdEnviron->base) {
        delete [] g_pDmdEnviron->base;
        g_pDmdEnviron->base = NULL;
    }
    if (g_pDmdEnviron->pEnviron) {
        delete [] g_pDmdEnviron->pEnviron;
        g_pDmdEnviron->pEnviron = NULL;
    }

    g_pDmdEnviron->base = NULL;
    g_pDmdEnviron->iNumOfEnviron = 0;

    int totallen = 0;
    int iNumOfEnviron = 0;
    for (iNumOfEnviron = 0; environ[iNumOfEnviron]; iNumOfEnviron++) {
        totallen += strlen(environ[iNumOfEnviron]) + 1;
    }
    g_pDmdEnviron->iNumOfEnviron = iNumOfEnviron;
    g_pDmdEnviron->pEnviron = new charP[iNumOfEnviron];
    if (NULL == g_pDmdEnviron->pEnviron) {
        DMD_LOG_ERROR("SaveEnviron(), new g_pDmdEnviron->pEnviron error");
        return DMD_S_FAIL;
    }

    g_pDmdEnviron->base = new char[totallen];
    if (NULL == g_pDmdEnviron->base) {
        DMD_LOG_ERROR("SaveEnviron(), new g_pDmdEnviron->base error");
        return DMD_S_FAIL;
    }
    memset(g_pDmdEnviron->base, 0, totallen);

    char *pCurrent = g_pDmdEnviron->base;
    for (int i = 0; i < iNumOfEnviron; i++) {
        strncpy(pCurrent, environ[i], strlen(environ[i]));
        g_pDmdEnviron->pEnviron[i] = pCurrent;
        environ[i] = pCurrent;
        pCurrent = pCurrent + strlen(environ[i]) + 1;
    }

#ifdef DEBUG
    // for (int i = 0; i < g_pDmdEnviron->iNumOfEnviron; i++) {
    //     DMD_LOG_INFO("SaveEnviron(), saved environ[" << i << "] = "
    //             << string(g_pDmdEnviron->pEnviron[i]));
    //     DMD_LOG_INFO("SaveEnviron(), environ[" << i << "] = "
    //             << environ[i]);
    // }
#endif

    return DMD_S_OK;
}

DMD_RESULT SetProcessName(char *pProcessName) {
    g_pDmdArgv->pOriginArgv[1] = NULL;
    strncpy(g_pDmdArgv->pOriginArgv[0], pProcessName, strlen(pProcessName));
    g_pDmdArgv->pOriginArgv[0][strlen(pProcessName)] = '\0';

    return DMD_S_OK;
}

}  // namespace opendmd

