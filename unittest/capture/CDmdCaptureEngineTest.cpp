/*
 ============================================================================
 Name        : CDmdCaptureEngineTest.cpp
 Author      : weizhenwei, <weizhenwei1988@gmail.com>
 Date           :2015.11.25
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
 Description : test class of CDmdVideoCaptureEngine interface.
 ============================================================================
 */

#include <string.h>

#include "gtest/gtest.h"

#include "IDmdDatatype.h"
#include "IDmdCaptureEngine.h"
#include "CDmdCaptureEngine.h"

using namespace opendmd;

class CDmdCaptureEngineTest : public testing::Test {
public:
    CDmdCaptureEngineTest() {
        pCaptureEngine = NULL;
        CreateVideoCaptureEngine(&pCaptureEngine);
    }

    virtual ~CDmdCaptureEngineTest() {
        if (pCaptureEngine) {
            ReleaseVideoCaptureEngine(&pCaptureEngine);
            pCaptureEngine = NULL;
        }
    }

    virtual void SetUp() {}
    virtual void TearDown() {}

public:
    IDmdCaptureEngine *pCaptureEngine;
};

TEST_F(CDmdCaptureEngineTest, Init) {
    DmdCaptureVideoFormat capVideoFormat = {DmdUnknown, 0, 0, 0, {0}};
    capVideoFormat.eVideoType = DmdI420;
    capVideoFormat.iWidth = 1280;
    capVideoFormat.iHeight = 720;
    capVideoFormat.fFrameRate = 30;
    char *pDeviceName = GetDeviceName();
    strncpy(capVideoFormat.sVideoDevice, pDeviceName, strlen(pDeviceName));

    EXPECT_EQ(DMD_S_OK, pCaptureEngine->Init(capVideoFormat));
}

