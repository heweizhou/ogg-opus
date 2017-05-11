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
#include "CodecAdapter.hpp"

@class EncoderOperation;
@protocol EncoderOperationDelegate <NSObject>
- (void)pcm_data:(EncoderOperation*)_this pcm_data:(NSData*)pcm_data power:(Float32)power;
- (void)encoded_data:(EncoderOperation*)_this encode_data:(NSData*)encode_data;
@end
@interface EncoderOperation : NSOperation
- (instancetype)init_with_pcm_data:(NSData*)data power:(Float32)power encoder:(CCodecAdapter*)encoder delegate:(id<EncoderOperationDelegate>)delegate;
@end

//start
@class EncoderStartOperation;
@protocol EncoderStartOperationDelegate <NSObject>
- (void)start_record:(EncoderStartOperation*)_this;
@end
@interface EncoderStartOperation : NSOperation
- (instancetype)init_with_delegate:(id<EncoderStartOperationDelegate>)delegate;
@end

//stop
@class EncoderStopOperation;
@protocol EncoderStopOperationDelegate <NSObject>
- (void)stop_record:(EncoderStopOperation*)_this;
@end
@interface EncoderStopOperation : NSOperation
- (instancetype)init_with_delegate:(id<EncoderStopOperationDelegate>)delegate;
@end

//stop
@class EncoderDispathOperation;
@protocol EncoderDispathOperationDelegate <NSObject>
- (void)dispath_data:(EncoderDispathOperation*)_this data:(NSData*)data;
@end
@interface EncoderDispathOperation : NSOperation
- (instancetype)init_with_delegate:(id<EncoderDispathOperationDelegate>)delegate data:(NSData*)data;
@end
