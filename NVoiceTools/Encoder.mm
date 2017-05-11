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

#import "Encoder.h"
#import "EncoderOperation.h"

#include "defines.h"
#include "OggPacker.hpp"
#include "opus_header.h"
#include "OggProtocol.hpp"
#include "UtilityTools.hpp"

@interface Encoder()<EncoderOperationDelegate, EncoderStartOperationDelegate, EncoderStopOperationDelegate, EncoderDispathOperationDelegate>
@property (nonatomic, weak) id<EncoderDelegate> delegate;

@property (nonatomic, strong)NSOperationQueue*  operation_queue;
@property (nonatomic, strong)NSMutableArray*    encode_opration_array;
@property (nonatomic, strong)NSMutableData *    temp_pcm_data;

@property (nonatomic, assign)int                frame_count;
@property (nonatomic, assign)int                frame_size;

@property (nonatomic, assign) CCodecAdapter*    codec;
@property (nonatomic, assign) OpusHeader*       header;
@property (nonatomic, assign) COggPacker*       packer;
@property (nonatomic, assign) int               sample_rate;
@end

@implementation Encoder

- (instancetype)init_with_delegate:(id<EncoderDelegate>)delegate sampleRate:(int)_sampleRate channels:(int)_channels depth:(int)_depth
{
    self = [super init];
    if (self) {
        self.delegate = delegate;
        
        self.sample_rate = _sampleRate;
        self.frame_size = _sampleRate * FRAME_SIZE_DUR;
        [self init_evn:_channels sampleRate:_sampleRate depth:_depth];
        [self init_opus_header:_channels sampleRate:_sampleRate];
        
    }
    return self;
}

- (void)dealloc
{
    delete self.codec;
    delete self.packer;
    free(self.header);
}

- (void)cancel
{
    [self.operation_queue cancelAllOperations];
    @synchronized (self.encode_opration_array) {
        [self.encode_opration_array removeAllObjects];
    }
}

- (void)start_encoding
{
    EncoderStartOperation* start = [[EncoderStartOperation alloc] init_with_delegate:self];
    [self.operation_queue addOperation:start];
    
//    std::string page0_data = COggProtocol::serialize_opus_header_to_ogg_page(self.packer, self.header);
//    std::string commnet = self.codec->get_codec_version();
//    std::string page1_data = COggProtocol::serialize_opus_comment_to_ogg_page(self.packer, commnet, 255);
//    std::string title_data = page0_data + page1_data;
//    
//    EncoderDispathOperation* dispath_data = [[EncoderDispathOperation alloc] init_with_delegate:self data:[[NSData alloc] initWithBytes:title_data.data() length:title_data.length()]];
//    [self.operation_queue addOperation:dispath_data];
}

- (void)stop_encoding
{
    EncoderStopOperation* operation = [[EncoderStopOperation alloc] init_with_delegate:self];
    [self.operation_queue addOperation:operation];
    [self.operation_queue waitUntilAllOperationsAreFinished];
}


- (void)new_pcm_data:(Byte *)buffer size:(UInt32)dataSize power:(Float32)power
{
    [self.temp_pcm_data appendBytes:buffer length:dataSize];
    
    int frame_bytes = self.frame_size * 2;
    while ([self.temp_pcm_data length] >= frame_bytes) {
        @autoreleasepool {
            NSData *pcmData = [NSData dataWithBytes:[self.temp_pcm_data bytes] length:frame_bytes];
            
            EncoderOperation* operation = [[EncoderOperation alloc] init_with_pcm_data:pcmData power:power encoder:self.codec delegate:self];
            
            @synchronized (self.encode_opration_array) {
                [self.encode_opration_array addObject:operation];
            }
            
            [self.operation_queue addOperation:operation];
            
            Byte *dataPtr = (Byte *)[self.temp_pcm_data bytes];
            dataPtr += frame_bytes;
            self.temp_pcm_data = [NSMutableData dataWithBytesNoCopy:dataPtr length:[self.temp_pcm_data length] - frame_bytes freeWhenDone:NO];
            
        }
    }

}
#pragma --mark privte

-(void)init_evn:(int)channels sampleRate:(int)sampleRate depth:(int)depth
{
    
    self.codec = new CCodecAdapter(sampleRate, channels, depth);
    self.packer = new COggPacker();
    
    self.operation_queue = [[NSOperationQueue alloc] init];
    self.encode_opration_array = [[NSMutableArray alloc] init];
    self.temp_pcm_data = [[NSMutableData alloc] init];
    
    [self.operation_queue setMaxConcurrentOperationCount:1];
}

-(void)init_opus_header:(int)channels sampleRate:(int)sampleRate
{
    self.header = (OpusHeader*)malloc(sizeof(OpusHeader));
    COggProtocol::fill_opus_header(self.header, sampleRate, channels, self.codec->get_skip_size());
}

-(void)push_encoded_data
{
    if (self.delegate) {
        if ([self.delegate respondsToSelector:@selector(encoded_data_arrive:)]) {
            std::string page_data = self.packer->get_page_data();
            [self.delegate encoded_data_arrive:[[NSData alloc] initWithBytes:page_data.data() length:page_data.length()]];
        }
    }
    
    self.frame_count = 0;
}

#pragma --mark EncoderOperationDelegate
- (void)pcm_data:(EncoderOperation*)_this pcm_data:(NSData*)pcm_data power:(Float32)power
{
    if (self.delegate) {
        if ([self.delegate respondsToSelector:@selector(pcm_data_arrive:power:)]) {
            [self.delegate pcm_data_arrive:pcm_data power:power];
        }
    }
}

- (void)encoded_data:(EncoderOperation*)_this encode_data:(NSData*)encode_data
{
    @synchronized (self.encode_opration_array) {
        [self.encode_opration_array removeObject:_this];
    }
    
    self.packer->add_packet((char*)[encode_data bytes], encode_data.length, self.sample_rate);
    self.frame_count++;
    
    if (self.frame_count % FRAME_COUNT_PER_OGG_PAGE == 0) {
        [self push_encoded_data];
    }

}

#pragma --mark EncoderStartOperation
- (void)start_record:(EncoderStartOperation *)_this
{
    if (self.delegate) {
        if ([self.delegate respondsToSelector:@selector(start_record)]) {
            [self.delegate start_record];
        }
    }
    
    std::string page0_data = COggProtocol::serialize_opus_header_to_ogg_page(self.packer, self.header);
    std::string commnet = self.codec->get_codec_version();
    std::string page1_data = COggProtocol::serialize_opus_comment_to_ogg_page(self.packer, commnet, 255);
    std::string title_data = page0_data + page1_data;
    
    if (self.delegate) {
        if ([self.delegate respondsToSelector:@selector(encoded_data_arrive:)]) {
            [self.delegate encoded_data_arrive:[[NSData alloc] initWithBytes:title_data.data() length:title_data.length()]];
        }
    }
}


#pragma --mark EncoderStopOperation
- (void)stop_record:(EncoderStopOperation *)_this
{
    [self push_encoded_data];
    
    if (self.delegate) {
        if ([self.delegate respondsToSelector:@selector(encoded_data_arrive:)]) {
            std::string page_data = COggProtocol::get_end_ogg_stream_page(self.packer, self.frame_size);
            [self.delegate encoded_data_arrive:[[NSData alloc] initWithBytes:page_data.data() length:page_data.length()]];
        }
    }

    if (self.delegate) {
        if ([self.delegate respondsToSelector:@selector(stop_record)]) {
            [self.delegate stop_record];
        }
    }
}

#pragma --mark EncoderDispathOperationDelegate
- (void)dispath_data:(EncoderDispathOperation *)_this data:(NSData *)data
{
    if (self.delegate) {
        if ([self.delegate respondsToSelector:@selector(encoded_data_arrive:)]) {
            [self.delegate encoded_data_arrive:data];
        }
    }
}

@end
