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

#ifndef CodecAdapter_hpp
#define CodecAdapter_hpp

#include <stdio.h>
#include "opus.h"
#include <string>

class CCodecAdapter
{
public:
    CCodecAdapter(int sample_rate, int channel_count, int bits_per_channel);
    ~CCodecAdapter();
public:
    unsigned char* encode_frame(char* in, int len, int* out_len);
    std::string    get_codec_version();
    int            get_skip_size();
private:
    int     m_sample_rate;
    int     m_channel_count;
    int     m_bits_per_channel;
    
    OpusEncoder*        m_encoder;
    
    unsigned long       m_encode_tmp_buffer_size;
    unsigned char*      m_encode_tmp_buffer;
};

#endif /* CodecAdapter_hpp */
