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

#import "EncoderOperation.h"


@interface EncoderOperation()

@property (nonatomic, weak) id<EncoderOperationDelegate>  delegate;
@property (nonatomic, strong) NSData*   pcm_data;
@property (nonatomic, assign) Float32   pcm_power;
@property (nonatomic, assign) CCodecAdapter* encoder;
@end


@implementation EncoderOperation

- (instancetype)init_with_pcm_data:(NSData*)data power:(Float32)power encoder:(CCodecAdapter*)encoder delegate:(id<EncoderOperationDelegate>)delegate
{
    self = [super init];
    if (self) {
        self.pcm_data = data;
        self.pcm_power = power;
        self.encoder = encoder;
        self.delegate = delegate;
    }
    
    return self;
}

- (void)main
{
    
    if (self.delegate) {
        if ([self.delegate respondsToSelector:@selector(pcm_data:pcm_data:power:)]) {
            [self.delegate pcm_data:self pcm_data:self.pcm_data power:self.pcm_power];
        }
        
        // to do encode
        int out_size;
        unsigned char* encode_buffer = self.encoder->encode_frame((char*)[self.pcm_data bytes], (int)[self.pcm_data length], &out_size);
        
        if (encode_buffer == NULL) {
            assert(0);
        }
        
        
        if ([self.delegate respondsToSelector:@selector(encoded_data:encode_data:)]) {
            [self.delegate encoded_data:self encode_data:[[NSData alloc] initWithBytes:encode_buffer length:out_size]];
        }
    }
    
}


@end

#pragma  --mark start
@interface EncoderStartOperation()

@property (nonatomic, weak) id<EncoderStartOperationDelegate>  delegate;
@end

@implementation EncoderStartOperation
- (instancetype)init_with_delegate:(id<EncoderStartOperationDelegate>)delegate
{
    self = [super init];
    if (self) {
        self.delegate = delegate;
    }
    
    return self;
}
- (void)main
{
    if (self.delegate) {
        if ([self.delegate respondsToSelector:@selector(stop_record:)]) {
            [self.delegate start_record:self];
        }
    }
    
}
@end

#pragma  --mark stop

@interface EncoderStopOperation()

@property (nonatomic, weak) id<EncoderStopOperationDelegate>  delegate;
@end

@implementation EncoderStopOperation
- (instancetype)init_with_delegate:(id<EncoderStopOperationDelegate>)delegate
{
    self = [super init];
    if (self) {
        self.delegate = delegate;
    }
    
    return self;
}
- (void)main
{
    if (self.delegate) {
        if ([self.delegate respondsToSelector:@selector(stop_record:)]) {
            [self.delegate stop_record:self];
        }
    }
    
}
@end

#pragma  --mark dispath
@interface EncoderDispathOperation()
@property (nonatomic, strong) NSData* data;
@property (nonatomic, weak) id<EncoderDispathOperationDelegate>  delegate;
@end

@implementation EncoderDispathOperation
- (instancetype)init_with_delegate:(id<EncoderDispathOperationDelegate>)delegate data:(NSData*)data
{
    self = [super init];
    if (self) {
        self.data = data;
        self.delegate = delegate;
    }
    
    return self;
}
- (void)main
{
    if (self.delegate) {
        if ([self.delegate respondsToSelector:@selector(stop_record:)]) {
            [self.delegate dispath_data:self data:self.data];
        }
    }
    
}
@end
