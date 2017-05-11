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

#include "CodecAdapter.hpp"
#include "opus.h"
#include "defines.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

CCodecAdapter::CCodecAdapter(int sample_rate, int channel_count, int bits_per_channel)
{
    m_sample_rate = sample_rate;
    m_channel_count = channel_count;
    m_bits_per_channel = bits_per_channel;
    
    int err = 0;
    m_encoder = opus_encoder_create(m_sample_rate, m_channel_count, OPUS_APPLICATION_VOIP, &err);
    
    if (err != OPUS_OK) {
        fprintf(stderr, "cannnot create opus encoder: %s\n", opus_strerror(err));
        m_encoder = NULL;
    }
    
    m_encode_tmp_buffer = (unsigned char*)malloc(m_sample_rate * FRAME_SIZE_DUR * m_channel_count * 2);
    opus_encoder_ctl(m_encoder, OPUS_SET_LSB_DEPTH(bits_per_channel));
//    opus_encoder_ctl(enc, OPUS_SET_BITRATE(BIT_RATE));
    opus_encoder_ctl(m_encoder, OPUS_SET_BITRATE(OPUS_AUTO));
    opus_encoder_ctl(m_encoder, OPUS_SET_VBR(1));
    opus_encoder_ctl(m_encoder, OPUS_SET_COMPLEXITY(10));
    opus_encoder_ctl(m_encoder, OPUS_SET_INBAND_FEC(0));
    opus_encoder_ctl(m_encoder, OPUS_SET_FORCE_CHANNELS(OPUS_AUTO));
    opus_encoder_ctl(m_encoder, OPUS_SET_DTX(0));
    opus_encoder_ctl(m_encoder, OPUS_SET_PACKET_LOSS_PERC(0));
    
}

CCodecAdapter::~CCodecAdapter()
{
    opus_encoder_destroy(m_encoder);
    free(m_encode_tmp_buffer);
}

unsigned char* CCodecAdapter::encode_frame(char* in, int len, int* out_len)
{
    *out_len = 0;
    opus_int16 *frame = (opus_int16 *)in;
    int frame_size = m_sample_rate * FRAME_SIZE_DUR;
    int nbytes = opus_encode(m_encoder, frame, frame_size, m_encode_tmp_buffer, frame_size * m_channel_count * 2);
    if (nbytes > frame_size * 2 * m_channel_count || nbytes < 0) {
        return NULL;
    }

    *out_len = nbytes;
    return m_encode_tmp_buffer;
}

std::string CCodecAdapter::get_codec_version()
{
    return opus_get_version_string();
}

int CCodecAdapter::get_skip_size()
{
    int lookahead;
    opus_encoder_ctl(m_encoder, OPUS_GET_LOOKAHEAD(&lookahead));
    return lookahead;
}

