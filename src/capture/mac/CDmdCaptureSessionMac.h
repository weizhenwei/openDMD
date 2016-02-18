/*
 ============================================================================
 * Name        : CDmdCaptureSessionMac.h
 * Author      : weizhenwei, <weizhenwei1988@gmail.com>
 * Date        : 2015.11.19
 *
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
 * Description : header file of capture session on mac platform.
 ============================================================================
 */

#ifndef SRC_CAPTURE_MAC_CDMDCAPTURESESSIONMAC_H
#define SRC_CAPTURE_MAC_CDMDCAPTURESESSIONMAC_H

#import <AVFoundation/AVFoundation.h>

#import "IDmdDatatype.h"

using namespace opendmd;

#define DMDRelease(x)  \
do {                   \
    [x release];       \
    x = nil;           \
} while (0)


// Y'CbCr 4:2:2 - yuvs: kCVPixelFormatType_422YpCbCr8_yuvs
// Y'CbCr 4:2:2 - uyuv: kCVPixelFormatType_422YpCbCr8
// Y'CbCr 4:2:0 - 420v: kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange;
typedef enum {
    MacUnknown   = 0,
    Macyuyv = kCVPixelFormatType_422YpCbCr8_yuvs,
    Macuyvy = kCVPixelFormatType_422YpCbCr8,
    Mac420v = kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange,
} MacVideoType;

typedef struct _output_format {
    MacVideoType    video_type;
    int             width;
    int             height;
} MacVideoOutputFormat;

typedef struct _video_format {
    MacVideoType    video_type;
    size_t          width;
    size_t          height;
    float           frame_rate;
    NSTimeInterval  time_stamp;
} MacVideoSampleFormat;

typedef struct _capsession_format {
    AVCaptureDevice *capDevice;
    AVCaptureDeviceFormat *capFormat;
    NSString *capSessionPreset;
    float capFPS;
} MacCaptureSessionFormat;


class IDmdMacAVVideoCapSessionSink;

@interface CDmdAVVideoCapSession
    : NSObject<AVCaptureVideoDataOutputSampleBufferDelegate> {
        id m_captureSession;          // AVCaptureSession
        // id m_videoCaptureDevice;      // AVCaptureDevice
        id m_videoCaptureInput;       // AVCaptureDeviceInput
        id m_videoCaptureDataOutput;  // AVCaptureVideoDataOutput

        MacCaptureSessionFormat m_format;
        IDmdMacAVVideoCapSessionSink* m_sink;
        NSRecursiveLock* m_sinkLock;
        float m_systemVersion;
}

- (id)init;

- (AVCaptureSession *)getAVCaptureSesion;
- (void)setSink:(IDmdMacAVVideoCapSessionSink*)sink;

- (DMD_BOOL)isRunning;
- (DMD_RESULT)startRun:(MacCaptureSessionFormat&)format;
- (DMD_RESULT)stopRun;
- (void)captureCleanup;

// update series
- (DMD_RESULT)updateAVCaptureDeviceFormat:(AVCaptureDeviceFormat *)format;
- (DMD_RESULT)updateAVCaptureSessionPreset:(NSString *)preset;
- (DMD_RESULT)updateAVCaptureSessionFPS:(float)fps;

- (DMD_RESULT)setCapSessionFormat:(MacCaptureSessionFormat&)format;
- (DMD_RESULT)getCapSessionFormat:(MacCaptureSessionFormat&)format;
@end


class IDmdMacAVVideoCapSessionSink {
public:
    virtual ~IDmdMacAVVideoCapSessionSink() {}

    virtual DMD_RESULT DeliverVideoData(CMSampleBufferRef sampleBuffer) = 0;
};

#endif  // SRC_CAPTURE_MAC_CDMDCAPTURESESSIONMAC_H

