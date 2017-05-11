//Copyright (c) 2011-2017 zhw
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in
//all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//THE SOFTWARE.

#import "RecorderManager.h"
#import "AQRecorder.h"
#import <AVFoundation/AVFoundation.h>
#import <AudioToolbox/AudioToolbox.h>
#import "Encoder.h"
#import "CodecUtilityTools.h"

#include "CodecTools.hpp"
#include "WavTools.hpp"
#include "defines.h"
#include "ogg.h"

#include <stdio.h>

@interface RecorderManager()<EncoderDelegate>
@property(nonatomic, assign)AQRecorder *mAQRecorder;
@property(nonatomic, strong)Encoder *encoder;
@property(nonatomic, assign)unsigned long   pcm_data_size;
@end

@implementation RecorderManager
static RecorderManager *mRecorderManager = nil;

+ (RecorderManager *)sharedManager {
    @synchronized(self) {
        if (mRecorderManager == nil){
            mRecorderManager = [[self alloc] init];
        }
    }
    return mRecorderManager;
}

- (instancetype)init
{
    self = [super init];
    if (self){
        // to do init
    }
    
    return self;
}

- (void)startRecording
{
    if (!_mAQRecorder) {
        
        self.pcm_data_size = 0;
        if ([self.delegate respondsToSelector:@selector(recordingStart)]) {
            [self.delegate recordingStart];
        }
        

        self.mAQRecorder = new AQRecorder();
        
        AVAudioSession *audioSession = [AVAudioSession sharedInstance];
        NSError *err = nil;
        [audioSession setCategory :AVAudioSessionCategoryRecord error:&err];
        if(err){
            return;
        }
        err = nil;
        [audioSession setActive:YES error:&err];
        if(err){
            return;
        }
        
        [[AVAudioSession sharedInstance] requestRecordPermission:^(BOOL granted) {
            if (granted) {
                
                self.encoder = [[Encoder alloc] init_with_delegate:self sampleRate:DEFAULT_SAMPLE_RATE channels:CHANNEL_COUNT depth:BIT_PER_SAMPLE];
                Boolean recordingWillBegin = _mAQRecorder->StartRecord(self.encoder);
                if (!recordingWillBegin) {
                    if ([self.delegate respondsToSelector:@selector(recordingFaile)]) {
                        [self.delegate recordingFaile];
                    }
                    return;
                }
            }
        }];
        
    }
}

- (void)stopRecording {
    if (_mAQRecorder) {
        
        _mAQRecorder->StopRecord();
        delete _mAQRecorder;
        _mAQRecorder = nil;
        self.encoder = nil;
        
        [[AVAudioSession sharedInstance] setActive:NO withOptions:AVAudioSessionSetActiveOptionNotifyOthersOnDeactivation error:nil];
    }
}

+ (int)encode_wav_file:(NSString*)wav_path opus_path:(NSString *)opus_path
{
    return CCodecTools::encode_wav_file([wav_path UTF8String], [opus_path UTF8String]);
}

+ (int)decode_opus_file:(NSString*)opus_path wav_path:(NSString*)wav_path
{
    return CCodecTools::decode_opus_file([opus_path UTF8String], [wav_path UTF8String]);
}

- (void)dealloc
{
    if (_mAQRecorder) {
        delete _mAQRecorder;
    }
}

#pragma --mark EncoderDelegate impelement

- (void)pcm_data_arrive:(NSData*)pcm_data power:(Float32)power
{
    self.pcm_data_size += pcm_data.length;
    if (self.delegate) {
        if ([self.delegate respondsToSelector:@selector(recordingOriData:power:)]) {
            [self.delegate recordingOriData:pcm_data power:power];
        }
    }
}

- (void)encoded_data_arrive:(NSData*)encode_data
{
    if (self.delegate) {
        if ([self.delegate respondsToSelector:@selector(recordingEncodeData:)]) {
            [self.delegate recordingEncodeData:encode_data];
        }
    }
}

- (void)start_record
{
    
}

- (void)stop_record
{
    if (self.delegate) {
        if ([self.delegate respondsToSelector:@selector(recordingComplete:encoded_sample_rate:encoded_channel_num:encoded_bit_per_sample:)]) {
            
            CWavTools::WAV_HEADER header = CWavTools::make_wav_header(DEFAULT_SAMPLE_RATE, CHANNEL_COUNT, BIT_PER_SAMPLE, (int)self.pcm_data_size);
            
            [self.delegate recordingComplete:[[NSData alloc] initWithBytes:&header length:sizeof(header)]
                         encoded_sample_rate:DEFAULT_SAMPLE_RATE
                         encoded_channel_num:CHANNEL_COUNT
                      encoded_bit_per_sample:BIT_PER_SAMPLE];
        }
    }
}

@end
