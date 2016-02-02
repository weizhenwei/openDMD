/*
 ============================================================================
 Name        : CDmdCaptureEngineMac.cpp
 Author      : weizhenwei, <weizhenwei1988@gmail.com>
 Date           :2015.07.14
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
 Description : implementation file of capture engine on mac platform.
 ============================================================================
 */

#import <Cocoa/Cocoa.h>
#import <Foundation/Foundation.h>
#import <AVFoundation/AVCaptureSession.h>
#import <AVFoundation/AVFoundation.h>
#import <Foundation/Foundation.h>
#import <Foundation/NSString.h>
#import <CoreVideo/CVPixelBuffer.h>

#import <string.h>

#import "CDmdCaptureEngineMac.h"
#import "CDmdCaptureSessionMac.h"

#include "DmdLog.h"
#include "DmdMutex.h"
#include "IDmdDatatype.h"

namespace opendmd {

CDmdCaptureEngineMac::CDmdCaptureEngineMac() : m_pVideoCapSession(nil) {
    memset(&m_capVideoFormat, 0, sizeof(m_capVideoFormat));
    memset(&m_capSessionFormat, 0, sizeof(m_capSessionFormat));
}

CDmdCaptureEngineMac::~CDmdCaptureEngineMac() {
}


// setup AVCapSession paramters;
DMD_RESULT CDmdCaptureEngineMac::setupAVCaptureDevice() {
    DMD_CHECK_NOTNULL(m_capVideoFormat.sVideoDevice);
    NSString *capDeviceID = [NSString stringWithUTF8String:
        m_capVideoFormat.sVideoDevice];
    m_capSessionFormat.capDevice =
        [AVCaptureDevice deviceWithUniqueID:capDeviceID];
    if (m_capSessionFormat.capDevice == nil) {
        DMD_LOG_FATAL("CDmdCaptureEngineMac::setupAVCaptureDevice(), "
                << "failed to create AVCaptureDevice.");
        return DMD_S_FAIL;
    }

    return DMD_S_OK;
}

DMD_RESULT CDmdCaptureEngineMac::setupAVCaptureDeviceFormat() {
    DmdVideoType eVideoType = m_capVideoFormat.eVideoType;
    if (eVideoType != DmdI420 && eVideoType != DmdYUYV
        && eVideoType != DmdUYVY) {
        DMD_LOG_FATAL("CDmdCaptureEngineMac::setupAVCaptureDeviceFormat(), "
                      << "Unsupported capture video type on Mac platform.");
        return DMD_S_FAIL;
    }

    NSString *strPrefix = nil;
    if (eVideoType == DmdI420) {
        strPrefix = @"Y'CbCr 4:2:0 - 420v, ";
    } else if (eVideoType == DmdYUYV) {
        strPrefix = @"Y'CbCr 4:2:2 - yuvs, ";
    } else if (eVideoType == DmdUYVY) {
        strPrefix = @"Y'CbCr 4:2:2 - uyvy, ";
    }

    NSString *strSuffix = [NSString stringWithFormat:@"%d x %d",
                           m_capVideoFormat.iWidth,
                           m_capVideoFormat.iHeight];
    if (![strSuffix isEqualToString:@"1280 x 720"]
        && ![strSuffix isEqualToString:@"640 x 480"]
        && ![strSuffix isEqualToString:@"320 x 240"]) {
        DMD_LOG_FATAL("CDmdCaptureEngineMac::setupAVCaptureDeviceFormat(), "
                      << "Unsupported capture video resolution "
                      << [strSuffix cStringUsingEncoding:NSUTF8StringEncoding]
                      << " on Mac platform.");
        return DMD_S_FAIL;
    }

    NSString *strCaptureFormat = [strPrefix stringByAppendingString:strSuffix];
    DMD_LOG_INFO("CDmdCaptureEngineMac::setupAVCaptureDeviceFormat(), "
            << "get AVCaptureDeviceFormat:"
            << [strCaptureFormat cStringUsingEncoding:NSUTF8StringEncoding]);

    bool bDefault = false;
    bool bSupport = false;
    AVCaptureDeviceFormat *supportFormat = nil;
    AVCaptureDeviceFormat *defaultFormat = nil;
    NSString *defaultStrFormat = @"Y'CbCr 4:2:0 - 420v, 1280 x 720";
    for (AVCaptureDeviceFormat *format in
            [m_capSessionFormat.capDevice formats]) {
        NSString *formatName =
            (NSString *)CMFormatDescriptionGetExtension(
                    [format formatDescription],
                    kCMFormatDescriptionExtension_FormatName);
        CMVideoDimensions dimensions = CMVideoFormatDescriptionGetDimensions(
                (CMVideoFormatDescriptionRef)[format formatDescription]);
        NSString *videoformat = [NSString stringWithFormat:@"%@, %d x %d",
                                 formatName, dimensions.width,
                                 dimensions.height];
        if ([videoformat isEqualToString:strCaptureFormat]) {
            bSupport = true;
            supportFormat = format;
            break;
        }

        if (!bDefault && [videoformat isEqualToString:defaultStrFormat]) {
            bDefault = true;
            defaultFormat = format;
        }
    }

    // set video format;
    if (bSupport) {
        m_capSessionFormat.capFormat = supportFormat;
    } else if (bDefault) {
        m_capSessionFormat.capFormat = defaultFormat;
    } else {
        m_capSessionFormat.capFormat = nil;
        DMD_LOG_FATAL("CDmdCaptureEngineMac::setupAVCaptureDeviceFormat(), "
                << "Unsupported AVCaptureDeviceFormat on Mac platform.");
        return DMD_S_FAIL;
    }

    return DMD_S_OK;
}

DMD_RESULT CDmdCaptureEngineMac::setupAVCaptureSessionPreset() {
    /*
    NSArray *arrayPresets = [NSArray arrayWithObjects:
                             AVCaptureSessionPresetLow,
                             AVCaptureSessionPresetMedium,
                             AVCaptureSessionPresetHigh,
                             AVCaptureSessionPreset320x240,
                             AVCaptureSessionPreset352x288,
                             AVCaptureSessionPreset640x480,
                             AVCaptureSessionPreset960x540,
                             AVCaptureSessionPreset1280x720,
                             AVCaptureSessionPresetPhoto,
                             nil];
     */
    NSString *strResolution = [NSString stringWithFormat:@"%dx%d",
                           m_capVideoFormat.iWidth,
                           m_capVideoFormat.iHeight];
    if (![strResolution isEqualToString:@"1280x720"]
        && ![strResolution isEqualToString:@"640x480"]
        && ![strResolution isEqualToString:@"320x240"]) {
        DMD_LOG_FATAL("CDmdCaptureEngineMac::setupAVCaptureSessionPreset(), "
                << "Unsupported capture video resolution "
                << [strResolution cStringUsingEncoding:NSUTF8StringEncoding]
                << " on Mac platform.");
        return DMD_S_FAIL;
    }

    NSString *strPrefix = @"AVCaptureSessionPreset";
    NSString *strPreset = [strPrefix stringByAppendingString:strResolution];
    if ([m_capSessionFormat.capDevice
            supportsAVCaptureSessionPreset:strPreset]) {
        m_capSessionFormat.capSessionPreset = strPreset;
        DMD_LOG_INFO("CDmdCaptureEngineMac::setupAVCaptureSessionPreset(), "
                << "set AVCaptureSessionPreset to "
                << [strPreset cStringUsingEncoding:NSUTF8StringEncoding]);
    } else {
        m_capSessionFormat.capSessionPreset = nil;
        DMD_LOG_FATAL("CDmdCaptureEngineMac::setupAVCaptureSessionPreset(), "
                << "unsupported AVCaptureSessionPreset to "
                << [strPreset cStringUsingEncoding:NSUTF8StringEncoding]);
        return DMD_S_FAIL;
    }

    return DMD_S_OK;
}

DMD_RESULT CDmdCaptureEngineMac::setupAVCaptureSessionFPS() {
    float fMinFPS = 0, fMaxFPS = 0.0f;
    NSArray *ranges = [m_capSessionFormat.capFormat
        videoSupportedFrameRateRanges];
    if (ranges == nil || [ranges count] <= 0) {
        DMD_LOG_FATAL("CDmdCaptureEngineMac::setupAVCaptureSessionFPS(), "
                      << "m_capSessionFormat.capFormat "
                      << "has no effective AVFrameRageRanges.");
        return DMD_S_FAIL;
    }

    AVFrameRateRange *firstRange = [ranges firstObject];
    fMinFPS = [firstRange minFrameRate];
    fMaxFPS = [firstRange maxFrameRate];
    for (int i = 1; i < [ranges count]; i++) {
        AVFrameRateRange *range = [ranges objectAtIndex:i];
        if (fMaxFPS < [range maxFrameRate]) {
            fMaxFPS = [range maxFrameRate];
        }
        if (fMinFPS > [range minFrameRate]) {
            fMinFPS = [range minFrameRate];
        }
    }

    m_capSessionFormat.capFPS = fMaxFPS;

    return DMD_S_OK;
}

DMD_RESULT CDmdCaptureEngineMac::setupAVCaptureSession() {
    DMD_RESULT ret =setupAVCaptureDevice();
    if (ret != DMD_S_OK) {
        return ret;
    }
    ret = setupAVCaptureDeviceFormat();
    if (ret != DMD_S_OK) {
        return ret;
    }
    ret = setupAVCaptureSessionPreset();
    if (ret != DMD_S_OK) {
        return ret;
    }
    ret = setupAVCaptureSessionFPS();
    if (ret != DMD_S_OK) {
        return ret;
    }

    return ret;
}

DMD_RESULT
CDmdCaptureEngineMac::Init(const DmdCaptureVideoFormat &capVideoFormat) {
    DMD_LOG_INFO("CDmdCaptureEngineMac::Init()"
            << ", capVideoFormat.eVideoType = "
            << DMDVIDEOTYPE(capVideoFormat.eVideoType)
            << ", capVideoFormat.iWidth = " << capVideoFormat.iWidth
            << ", capVideoFormat.iHeight = " << capVideoFormat.iHeight
            << ", capVideoFormat.fFrameRate = " << capVideoFormat.fFrameRate
            << ", capVideoFormat.sVideoDevice = "
            << capVideoFormat.sVideoDevice);
    memcpy(&m_capVideoFormat, &capVideoFormat, sizeof(capVideoFormat));

    DMD_RESULT ret = DMD_S_OK;
    ret = setupAVCaptureSession();
    if (ret != DMD_S_OK) {
        return ret;
    }

    if (nil == m_pVideoCapSession) {
        m_pVideoCapSession = [[CDmdAVVideoCapSession alloc] init];
    }
    if (nil == m_pVideoCapSession) {
        DMD_LOG_FATAL("CDmdCaptureEngineMac::init(), "
                << "couldn't init CDmdAVVideoCapSession.");
        return DMD_S_FAIL;
    }

    [m_pVideoCapSession setSink:this];
    [m_pVideoCapSession setCapSessionFormat:m_capSessionFormat];

    return DMD_S_OK;
}

DMD_RESULT CDmdCaptureEngineMac::Uninit() {
    memset(&m_capVideoFormat, 0, sizeof(m_capVideoFormat));
    memset(&m_capSessionFormat, 0, sizeof(m_capSessionFormat));

    [m_pVideoCapSession setSink:nil];
    [m_pVideoCapSession setCapSessionFormat:m_capSessionFormat];
    DMDRelease(m_pVideoCapSession);
    DMDRelease(m_capSessionFormat.capDevice);

    return DMD_S_OK;
}

DMD_RESULT CDmdCaptureEngineMac::StartCapture() {
    if (YES == [m_pVideoCapSession isRunning]) {
        DMD_LOG_ERROR("CDmdCaptureEngineMac::StartCapture(), "
                      << "CDmdAVVideoCapSession is already running.");
        return DMD_S_FAIL;
    }

    if ([m_pVideoCapSession startRun:m_capSessionFormat] != DMD_S_OK) {
        DMD_LOG_ERROR("CDmdCaptureEngineMac::StartCapture(), "
                      << "AVCaptureSession start failed!");
        return DMD_S_FAIL;
    }

    return DMD_S_OK;
}

DMD_BOOL CDmdCaptureEngineMac::IsCapturing() {
    return [m_pVideoCapSession isRunning];
}

DMD_RESULT CDmdCaptureEngineMac::StopCapture() {
    return [m_pVideoCapSession stopRun];
}

DMD_RESULT CDmdCaptureEngineMac::DeliverVideoData(
        CMSampleBufferRef sampleBuffer) {
    CVImageBufferRef imageBuffer =
    CMSampleBufferGetImageBuffer(sampleBuffer);
    DmdVideoRawData packet;
    memset(&packet, 0, sizeof(packet));
    if (kCVReturnSuccess == CVPixelBufferLockBaseAddress(imageBuffer, 0)) {
        if (DMD_S_OK == CVImageBuffer2VideoRawPacket(imageBuffer, packet)) {
            // DeliverVideoData(&packet);
        }
        CVPixelBufferUnlockBaseAddress(imageBuffer, 0);
    }

    return DMD_S_OK;
}

DMD_RESULT CVImageBuffer2VideoRawPacket(CVImageBufferRef imageBuffer,
        DmdVideoRawData& packet) {
    packet.ulRotation = 0;
    OSType pixelFormat = CVPixelBufferGetPixelFormatType(imageBuffer);
    packet.fmtVideoFormat.eVideoType = DmdUnknown;
    packet.fmtVideoFormat.iWidth = CVPixelBufferGetWidth(imageBuffer);
    packet.fmtVideoFormat.iHeight = CVPixelBufferGetHeight(imageBuffer);
    packet.fmtVideoFormat.fFrameRate = 0;
    packet.fmtVideoFormat.ulTimestamp = [[NSDate date] timeIntervalSince1970];
    packet.ulPlaneCount = CVPixelBufferGetPlaneCount(imageBuffer);
    if (kCVPixelFormatType_422YpCbCr8_yuvs == pixelFormat) {
        packet.fmtVideoFormat.eVideoType = DmdYUYV;
        packet.pSrcData[0] =
            (unsigned char *)CVPixelBufferGetBaseAddress(imageBuffer);
        packet.ulDataLen = CVPixelBufferGetBytesPerRow(imageBuffer)
            * packet.fmtVideoFormat.iHeight;
    } else if (kCVPixelFormatType_422YpCbCr8 == pixelFormat) {
        packet.fmtVideoFormat.eVideoType = DmdUYVY;
        packet.pSrcData[0] =
            (unsigned char *)CVPixelBufferGetBaseAddress(imageBuffer);
        packet.ulDataLen = CVPixelBufferGetBytesPerRow(imageBuffer)
            * packet.fmtVideoFormat.iHeight;
    } else if (kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange
            == pixelFormat) {  // NV12 actually;
        packet.fmtVideoFormat.eVideoType = DmdNV12;
        for (int i = 0; i < packet.ulPlaneCount; i++) {
            unsigned char *pPlane = nil;
            pPlane = (unsigned char *)
                CVPixelBufferGetBaseAddressOfPlane(imageBuffer, i);
            packet.pSrcData[i] = pPlane;
            size_t bytesPerRow = 0;
            bytesPerRow = CVPixelBufferGetBytesPerRowOfPlane(imageBuffer, i);
            packet.ulSrcStride[i] = bytesPerRow;
            size_t height = 0;
            height = CVPixelBufferGetHeightOfPlane(imageBuffer, i);
            packet.ulSrcDatalen[i] = bytesPerRow * height;
            packet.ulDataLen += packet.ulSrcDatalen[i];
        }
    }

    return DMD_S_OK;
}



// public interface implementation defined at CDmdCaptureEngine.h
const char *GetDeviceName() {
    static char deviceName[maxDeviceNameLength];
    static bool bGotDevice = false;
    if (bGotDevice) {
        return deviceName;
    } else {
        AVCaptureDevice *device =
            [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo];
        if (nil == device) {
            DMD_LOG_ERROR("GetDeviceName(), could not get video device");
            return NULL;
        }

        const char *pDeviceName = [[device uniqueID] UTF8String];
        if (strlen(pDeviceName) > maxDeviceNameLength) {
            DMD_LOG_ERROR("GetDeviceName, video device name length > 256");
            return NULL;
        }

        strncpy(deviceName, pDeviceName, strlen(pDeviceName));
        bGotDevice = true;

        return deviceName;
    }
}

DMD_RESULT CreateVideoCaptureEngine(IDmdCaptureEngine **ppVideoCapEngine) {
    if (NULL == ppVideoCapEngine) {
        return DMD_S_FAIL;
    }
    CDmdCaptureEngineMac *pMacVideoCapEngine = new CDmdCaptureEngineMac();
    DMD_CHECK_NOTNULL(pMacVideoCapEngine);
    *ppVideoCapEngine = pMacVideoCapEngine;

    return DMD_S_OK;
}

DMD_RESULT ReleaseVideoCaptureEngine(IDmdCaptureEngine **ppVideoCapEngine) {
    if ((ppVideoCapEngine == NULL) || (*ppVideoCapEngine == NULL)) {
        DMD_LOG_ERROR("ReleaseVideoCaptureEngine(), invalid videoCapEngine.");
        return DMD_S_FAIL;
    }

    delete (*ppVideoCapEngine);
    *ppVideoCapEngine = NULL;

    return DMD_S_OK;
}

}  // namespace opendmd

