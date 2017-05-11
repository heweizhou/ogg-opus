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

#import <Foundation/Foundation.h>

@protocol RecordingDelegate <NSObject>

//开始录音
- (void)recordingStart;
//录音失败
- (void)recordingFaile;

/**
 录音 callback

 @param origi_data wav数据
 @param power 音量
 */
- (void)recordingOriData:(NSData*)origi_data power:(Float32)power;

/**
 录音callback

 @param encode_data opus数据
 */
- (void)recordingEncodeData:(NSData*)encode_data;

/**
 录音结束

 @param pcm_header pcm头
 @param encoded_sample_rate tate
 @param encoded_channel_num num
 @param encoded_bit_per_sample sample
 */
- (void)recordingComplete:(NSData*)pcm_header encoded_sample_rate:(int)encoded_sample_rate encoded_channel_num:(int)encoded_channel_num encoded_bit_per_sample:(int)encoded_bit_per_sample;

@end

@interface RecorderManager : NSObject

@property (nonatomic, weak)  id<RecordingDelegate> delegate;

+ (RecorderManager *)sharedManager;

//wav to opus
+ (int)encode_wav_file:(NSString*)wav_path opus_path:(NSString *)opus_path;

//opus to wav
+ (int)decode_opus_file:(NSString*)opus_path wav_path:(NSString*)wav_path;

//开始录音
- (void)startRecording;

//停止录音
- (void)stopRecording;

@end
