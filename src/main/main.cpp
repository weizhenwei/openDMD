/*
 ============================================================================
 Name        : main.cpp
 Author      : weizhenwei, <weizhenwei1988@gmail.com>
 Date           :2015.06.24
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
 Description : main entry of the project.
 ============================================================================
 */

#include <stdlib.h>

#include "IDmdDatatype.h"
#include "DmdLog.h"

#include "IDmdCaptureEngine.h"
#include "CDmdCaptureEngine.h"
#include "DmdCmdlineParameter.h"

#include "main.h"

using namespace opendmd;

static void parseCmdline(int argc, char *argv[]) {
    DmdCmdlineParameter::singleton()->parseCmdlineParameter(argc, argv);

    if (DmdCmdlineParameter::singleton()->isShowHelp()) {
        DmdCmdlineParameter::singleton()->showHelp();
        exit(EXIT_SUCCESS);
    }
    if (DmdCmdlineParameter::singleton()->isShowVersion()) {
        DmdCmdlineParameter::singleton()->showVersion();
        exit(EXIT_SUCCESS);
    }
    if (!DmdCmdlineParameter::singleton()->isValidParameter()) {
        exit(EXIT_FAILURE);
    }
    if (DmdCmdlineParameter::singleton()->isDaemonize()) {
        DmdCmdlineParameter::singleton()->daemonize();
    }
}

int main(int argc, char *argv[]) {
    parseCmdline(argc, argv);

    client_main(argc, argv);

    // to test daemonize;
    // while (1);

    return 0;
}

