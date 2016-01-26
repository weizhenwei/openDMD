/*
 ============================================================================
 Name        : CDmdCaptureSessionMac.mm
 Author      : weizhenwei, <weizhenwei1988@gmail.com>
 Date           :2015.11.19
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
 Description : implementation file of capture session on mac platform.
 ============================================================================
 */

#import <Cocoa/Cocoa.h>
#import <AVFoundation/AVCaptureSession.h>
#import <AVFoundation/AVFoundation.h>
#import <CoreVideo/CVPixelBuffer.h>
#import <Foundation/Foundation.h>
#import <Foundation/NSString.h>

#include "DmdLog.h"
#import "CDmdCaptureSessionMac.h"

@class CDmdAVVideoCapSession;

static void capture_cleanup(void* p) {
    CDmdAVVideoCapSession* capSession = (__bridge CDmdAVVideoCapSession *)p;
    [capSession captureCleanup];
}

@implementation CDmdAVVideoCapSession

- (id)init {
    m_captureSession = nil;
    m_videoCaptureInput = nil;
    m_videoCaptureDataOutput = nil;
    memset(&m_format, 0, sizeof(m_format));
    m_sink = nil;
    m_sinkLock = [[NSRecursiveLock alloc] init];

    self = [super init];
    if (nil != self) {
        m_captureSession = [[AVCaptureSession alloc] init];
        if (nil == m_captureSession) {
            DMD_LOG_ERROR("CDmdAVVideoCapSession::init(), "
                          << "couldn't init AVCaptureSession.");
            return nil;
        }
    } else {
        DMD_LOG_ERROR("CDmdAVVideoCapSession::init(), [super init] failed.");
        return nil;
    }

    return self;
}

- (void)dealloc {
    m_sink = nil;
    DMDRelease(m_sinkLock);
    DMDRelease(m_captureSession);

    [super dealloc];
}


- (AVCaptureSession *)getAVCaptureSesion {
    return m_captureSession;
}

- (DMD_RESULT)setCapSessionFormat:(MacCaptureSessionFormat&)format {
    m_format = format;

    return DMD_S_OK;
}

- (DMD_RESULT)getCapSessionFormat:(MacCaptureSessionFormat&)format {
    format = m_format;

    return DMD_S_OK;
}

- (void)setSink:(IDmdMacAVVideoCapSessionSink*)sink {
    [m_sinkLock lock];
    m_sink = sink;
    [m_sinkLock unlock];
}

- (DMD_RESULT)createVideoInputAndOutput {
    if (nil == m_captureSession) {
        DMD_LOG_ERROR("CDmdAVVideoCapSession::createVideoInputAndOutput(), "
                << "AVCaptureSession is nil.");
        return DMD_S_FAIL;;
    }

    DMD_RESULT result = DMD_S_OK;
    [m_captureSession beginConfiguration];
    do {
        NSError *error = nil;

        if (nil != m_videoCaptureInput) {
            [m_captureSession removeInput:m_videoCaptureInput];
            DMDRelease(m_videoCaptureInput);
        }

        m_videoCaptureInput =
        [[AVCaptureDeviceInput alloc] initWithDevice:m_format.capDevice
            error:&error];
        if (nil != m_videoCaptureInput) {
            [m_captureSession addInput:m_videoCaptureInput];
        } else {
            NSString *errorString = [[NSString alloc]
                initWithFormat:@"%@", error];
            DMD_LOG_ERROR("CDmdAVVideoCapSession::createVideoInputAndOutput():"
                    << [errorString UTF8String]);
            [errorString release];
            result = DMD_S_FAIL;
            break;
        }

        if (nil == m_videoCaptureDataOutput) {
            m_videoCaptureDataOutput = [[AVCaptureVideoDataOutput alloc] init];
            if (nil != m_videoCaptureDataOutput) {
                dispatch_queue_t macAVCaptureQueue =
                    dispatch_queue_create("DmdMacAVCaptureQueue", nil);
                dispatch_set_context(macAVCaptureQueue, [self retain]);
                dispatch_set_finalizer_f(macAVCaptureQueue, capture_cleanup);

                [m_videoCaptureDataOutput setSampleBufferDelegate:self
                    queue:macAVCaptureQueue];
                dispatch_release(macAVCaptureQueue);
                [m_captureSession addOutput:m_videoCaptureDataOutput];
            } else {
                break;
            }
        }
    } while (0);

    [m_captureSession commitConfiguration];

    return result;
}

- (DMD_RESULT)destroyVideoInputAndOutput {
    [m_captureSession beginConfiguration];

    [m_captureSession removeInput:m_videoCaptureInput];
    DMDRelease(m_videoCaptureInput);
    [m_captureSession removeOutput:m_videoCaptureDataOutput];
    [m_videoCaptureDataOutput setSampleBufferDelegate:nil queue:nil];
    DMDRelease(m_videoCaptureDataOutput);

    [m_captureSession commitConfiguration];

    return DMD_S_OK;
}

- (DMD_BOOL)isRunning {
    return [m_captureSession isRunning];
}

- (DMD_RESULT)startRun:(MacCaptureSessionFormat&)format {
    m_format = format;
    if (nil == m_captureSession) {
        DMD_LOG_ERROR("CDmdAVVideoCapSession::startRun(),"
                << "AVCaptureSession is nil.");
        return DMD_S_FAIL;
    }
    if (YES == [m_captureSession isRunning]) {
        DMD_LOG_ERROR("CDmdAVVideoCapSession::startRun(),"
                << "AVCaptureSession is already running.");
        return DMD_S_FAIL;
    }

    DMD_RESULT result = [self createVideoInputAndOutput];
    if (DMD_S_OK != result) {
        return result;
    }

    result = [self updateVideoFormat];
    if (DMD_S_OK != result) {
        return result;
    }

    [m_captureSession startRunning];
    if (NO == [m_captureSession isRunning]) {
        [self destroyVideoInputAndOutput];
        DMD_LOG_ERROR("CDmdAVVideoCapSession::startRun(), "
                << "AVCaptureSession couldn't start running.");
        return DMD_S_FAIL;
    }

    return DMD_S_OK;
}

- (DMD_RESULT)stopRun {
    if (NO == [m_captureSession isRunning]) {
        return DMD_S_FAIL;
    }

    [m_captureSession stopRunning];
    [self destroyVideoInputAndOutput];
    if (YES == [m_captureSession isRunning]) {
        DMD_LOG_ERROR("CDmdAVVideoCapSession::stopRun(), "
                << "AVCaptureSession couldn't stop running.");
        return DMD_S_FAIL;
    }

    return DMD_S_OK;
}

- (void)captureCleanup {
    [self release];
}

- (DMD_RESULT)updateAVCaptureDeviceFormat:(AVCaptureDeviceFormat *)format {
    m_format.capFormat = format;

    [m_captureSession commitConfiguration];
    NSError *error = nil;
    if ([m_format.capDevice lockForConfiguration:&error]) {
        [m_format.capDevice setActiveFormat:m_format.capFormat];
        [m_format.capDevice unlockForConfiguration];
    } else {
        NSString *errorString = [[NSString alloc] initWithFormat:@"%@", error];
        DMD_LOG_ERROR("CDmdAVVideoCapSession::updateAVCaptureDeviceFormat(): "
                << [errorString UTF8String]);
        [errorString release];
        return DMD_S_FAIL;
    }

    MacVideoOutputFormat videoOutputFormat = [self getVideoOutputFormat];
    [m_videoCaptureDataOutput
        setVideoSettings:[NSDictionary dictionaryWithObjectsAndKeys:
        [NSNumber numberWithInt: videoOutputFormat.video_type],
        kCVPixelBufferPixelFormatTypeKey,
        [NSNumber numberWithInt: videoOutputFormat.width],
        (id)kCVPixelBufferWidthKey,
        [NSNumber numberWithInt: videoOutputFormat.height],
        (id)kCVPixelBufferHeightKey,
        AVVideoScalingModeFit,
        (id)AVVideoScalingModeKey,
        nil]];

    [m_captureSession commitConfiguration];

    return DMD_S_OK;
}

- (DMD_RESULT)updateAVCaptureSessionPreset:(NSString *)preset {
    m_format.capSessionPreset = preset;
    [m_captureSession beginConfiguration];
    [m_captureSession setSessionPreset: m_format.capSessionPreset];
    [m_captureSession commitConfiguration];

    return DMD_S_OK;
}

- (DMD_RESULT)updateAVCaptureSessionFPS:(float)fps {
    m_format.capFPS = fps;
    [m_captureSession beginConfiguration];
    NSArray *connections = [m_videoCaptureDataOutput connections];
    NSUInteger connectionCount = [connections count];
    if (connectionCount > 0) {
        id connection = [connections objectAtIndex:0];
        if (YES == [connection isVideoMinFrameDurationSupported]) {
            [connection setVideoMinFrameDuration:
                CMTimeMakeWithSeconds(1.0 / m_format.capFPS, 10000)];
        }
    }
    [m_captureSession commitConfiguration];

    return DMD_S_OK;
}

// Y'CbCr 4:2:2 - yuvs: kCVPixelFormatType_422YpCbCr8_yuvs
// Y'CbCr 4:2:2 - uyuv: kCVPixelFormatType_422YpCbCr8
// Y'CbCr 4:2:0 - 420v: kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange;
- (MacVideoOutputFormat)getVideoOutputFormat {
    MacVideoOutputFormat videoOutputFormat;
    NSString *formatName = (NSString *)CMFormatDescriptionGetExtension(
                            [m_format.capFormat formatDescription],
                            kCMFormatDescriptionExtension_FormatName);
    if ([formatName isEqualToString:@"Y'CbCr 4:2:2 - yuvs"]) {
        videoOutputFormat.video_type =
            (MacVideoType)kCVPixelFormatType_422YpCbCr8_yuvs;
    } else if ([formatName isEqualToString:@"Y'CbCr 4:2:2 - uyvy"]) {
        videoOutputFormat.video_type =
            (MacVideoType)kCVPixelFormatType_422YpCbCr8;
    } else if ([formatName isEqualToString:@"Y'CbCr 4:2:0 - 420v"]) {
        videoOutputFormat.video_type =
            (MacVideoType)kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange;
    } else {
        videoOutputFormat.video_type = MacUnknown;
    }

    CMVideoDimensions dimensions = CMVideoFormatDescriptionGetDimensions(
            (CMVideoFormatDescriptionRef)
            [m_format.capFormat formatDescription]);
    videoOutputFormat.width = dimensions.width;
    videoOutputFormat.height = dimensions.height;

    [formatName release];
    return videoOutputFormat;
}

- (DMD_RESULT)updateVideoFormat {
    if (nil == m_captureSession
        || nil == m_videoCaptureDataOutput) {
        DMD_LOG_ERROR("CDmdAVVideoCapSession::updateVideoFormat(), "
            << "m_captureSession == nil || m_videoCaptureDataOutput == nil.");
    }

    [m_captureSession beginConfiguration];

    // set max frame rate
    NSArray *connections = [m_videoCaptureDataOutput connections];
    NSUInteger connectionCount = [connections count];
    if (connectionCount > 0) {
        id connection = [connections objectAtIndex:0];
        if (YES == [connection isVideoMinFrameDurationSupported]) {
            [connection setVideoMinFrameDuration:
                CMTimeMakeWithSeconds(1.0 / m_format.capFPS, 10000)];
        }
    }
    [m_captureSession setSessionPreset: m_format.capSessionPreset];

    NSError *error = nil;
    if ([m_format.capDevice lockForConfiguration:&error]) {
        [m_format.capDevice setActiveFormat:m_format.capFormat];
        [m_format.capDevice unlockForConfiguration];
    } else {
        NSString *errorString = [[NSString alloc] initWithFormat:@"%@", error];
        DMD_LOG_ERROR("CDmdAVVideoCapSession::updateVideoFormat():"
                << [errorString UTF8String]);
        [errorString release];
        return DMD_S_FAIL;
    }

    MacVideoOutputFormat videoOutputFormat = [self getVideoOutputFormat];
    [m_videoCaptureDataOutput setVideoSettings:
        [NSDictionary dictionaryWithObjectsAndKeys:
        [NSNumber numberWithInt: videoOutputFormat.video_type],
        kCVPixelBufferPixelFormatTypeKey,
        [NSNumber numberWithInt: videoOutputFormat.width],
        (id)kCVPixelBufferWidthKey,
        [NSNumber numberWithInt: videoOutputFormat.height],
        (id)kCVPixelBufferHeightKey,
        AVVideoScalingModeFit,
        (id)AVVideoScalingModeKey,
        nil]];

    [m_captureSession commitConfiguration];

    return DMD_S_OK;
}

// Notes: the call back function will be called from capture thread.
- (void)captureOutput:(AVCaptureOutput *)captureOutput
    didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer
    fromConnection:(AVCaptureConnection *)connection {
    if (NO == [m_captureSession isRunning]) {
        return;
    }

    if (m_sink) {
        m_sink->DeliverVideoData(sampleBuffer);
    }
}

@end

