/*
 ============================================================================
 Name        : DmdCmdlineParameter.cpp
 Author      : weizhenwei, <weizhenwei1988@gmail.com>
 Date           :2015.07.03
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
 Description : implementation of cmdline parameter operation.
 ============================================================================
 */

#include "DmdCmdlineParameter.h"

#include <stdio.h>
#include <getopt.h>
#include <string>

#include <glog/logging.h>


namespace opendmd {

DmdCmdlineParameter *DmdCmdlineParameter::s_pCmdlineParameter
    = new DmdCmdlineParameter();

DmdCmdlineParameter::DmdCmdlineParameter() : m_bValidParameter(true),
        m_bShowHelp(false), m_bShowVersion(false), m_bDaemonize(false),
        m_sProgramName(new std::string("openDMD")), m_sPid_file(NULL),
        m_sCfg_file(NULL) {
}

DmdCmdlineParameter::~DmdCmdlineParameter() {
    if (m_sProgramName) {
        delete m_sProgramName;
        m_sProgramName = NULL;
    }
    if (m_sPid_file) {
        delete m_sPid_file;
        m_sPid_file = NULL;
    }
    if (m_sCfg_file) {
        delete m_sCfg_file;
        m_sCfg_file = NULL;
    }
}

DmdCmdlineParameter *DmdCmdlineParameter::singleton() {
    CHECK_NOTNULL(s_pCmdlineParameter);
    return s_pCmdlineParameter;
}

void DmdCmdlineParameter::showHelp() {
    fprintf(stdout, "Usage:%s [OPTION...]\n", m_sProgramName->c_str());
    fprintf(stdout, "  -p, --pid-file=FILE,    Use specified pid file\n");
    fprintf(stdout, "  -f, --cfg-file=FILE,    Use specified config file\n");
    fprintf(stdout, "  -d, --daemonize,        Run opendmd in daemon mode\n");
    fprintf(stdout, "  -v, --version,          Display the version number\n");
    fprintf(stdout, "  -h, --help,             Display this help message\n");
}

void DmdCmdlineParameter::showVersion() {
    // TODO(weizhenwei): Add more copyright message here.
    fprintf(stdout, "openDMD 0.0.1\n");
}

void DmdCmdlineParameter::parseCmdlineParameter(int argc, char *argv[]) {
    if (m_sProgramName) {
        delete m_sProgramName;
        m_sProgramName = NULL;
    }
    m_sProgramName = new std::string(argv[0]);

    struct option long_options[] = {
        {"pid-file",           required_argument,  0, 'p'},
        {"cfg-file",           required_argument,  0, 'f'},
        {"daemonize",          no_argument,        0, 'd'},
        {"version",            no_argument,        0, 'v'},
        {"help",               no_argument,        0, 'h'},
        {0, 0, 0, 0},
    };

    // man 3 getopt_long for more information about getopt_long;
    int c;
    while ((c = getopt_long(argc, argv, "p:f:dvh", long_options, NULL))
            != EOF) {
        switch (c) {
            case 'v':
                m_bShowVersion = true;
                break;
            case 'h':
                m_bShowHelp = true;
                break;
            case 'd':
                m_bDaemonize = true;
                break;
            case 'p':
                m_sPid_file = new std::string(optarg);
                break;
            case 'f':
                m_sCfg_file = new std::string(optarg);
                break;
            default:
                m_bValidParameter = false;
                break;
        }
    }  // while
}

}  // namespace opendmd
