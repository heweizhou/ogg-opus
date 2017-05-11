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
#include "CodecTools.hpp"
#include <string.h>
#include <stdlib.h>

#include "CodecAdapter.hpp"
#include "WavTools.hpp"
#include "OggProtocol.hpp"
#include "UtilityTools.hpp"
#include "defines.h"
#include "OggOpusFileAnalyser.hpp"
#include "ogg.h"

namespace CCodecTools_private {
        
    unsigned long get_wave_file_size(FILE* fp)
    {
        unsigned long curr_pos = ftell(fp);
        
        fseek(fp, 0, SEEK_END);
        unsigned long total_size = ftell(fp);
        fseek(fp, curr_pos, SEEK_SET);
        
        return total_size;
    }
}

int CCodecTools::encode_wav_file(const char *in_file_path, const char *out_file_path) {
    FILE *fin = fopen(in_file_path, "rb");
    
    if (fin == NULL || fin == 0) {
        return -1;
    }
    
    CWavTools::WAV_HEADER header;
    
    size_t header_len = fread(&header, sizeof(header), 1, fin);
    
    if (header_len == 0) {
        return -1;
    }
    
    unsigned long total_size = header.dataSize;
    int frame_size = CWavTools::get_wave_frame_size(&header);
    int frame_bytes = CWavTools::get_wave_frame_bytes(&header);
    int frame_count = (int)total_size / frame_bytes;
    
    //encode
    FILE *fout = fopen(out_file_path, "wb");
    if (fout == NULL || fout == 0) {
        return -1;
    }
    
    int err = 0;
    opus_int32 skip = 0;
    
    OpusEncoder *enc = opus_encoder_create(DEFAULT_SAMPLE_RATE, header.numChannels, OPUS_APPLICATION_VOIP, &err);
    if (err != OPUS_OK) {
        fprintf(stderr, "cannnot create opus encoder: %s\n", opus_strerror(err));
        enc = NULL;
        return -1;
    }
    
    opus_encoder_ctl(enc, OPUS_GET_LOOKAHEAD(&skip));
    opus_encoder_ctl(enc, OPUS_SET_LSB_DEPTH(header.bitsPerSample));
    
    //
    COggPacker packer;
    //prepare opus header
    OpusHeader opus_header;
    COggProtocol::fill_opus_header(&opus_header, DEFAULT_SAMPLE_RATE, header.numChannels, skip);
    std::string header_data_with_ogg_page = COggProtocol::serialize_opus_header_to_ogg_page(&packer, &opus_header);
    
    //prepare comment
    std::string comment_string = opus_get_version_string();
    std::string comment_data_with_ogg_page = COggProtocol::serialize_opus_comment_to_ogg_page(&packer, comment_string, 255);
    
    //write header and commnet
    std::string pre_header = header_data_with_ogg_page + comment_data_with_ogg_page;
    fwrite(pre_header.data(), pre_header.length(), 1, fout);
    
    
    char* in = (char*)malloc(frame_bytes);
    unsigned char *out = (unsigned char*)malloc(frame_bytes);
    
    int frame_index = 0;
    while (frame_index++ < frame_count) {
        size_t len = fread(in, frame_bytes, 1, fin);
        if (len == 0) {
            goto end;
        }
        
        int bytes = opus_encode(enc, (opus_int16*)in, frame_size, out, frame_bytes);
        if (bytes < 0) {
            continue;
        }
        
        packer.add_packet((char*)out, bytes, DEFAULT_SAMPLE_RATE);
        
        if (frame_index % FRAME_COUNT_PER_OGG_PAGE == 0) {
            std::string page_data = packer.get_page_data();
            if (page_data.length() > 0) {
                fwrite(page_data.data(), page_data.length(), 1, fout);
            }
        }
    }
    
end:
    //process remain data
    std::string page_data = packer.get_page_data();
    if (page_data.length() > 0) {
        fwrite(page_data.data(), page_data.length(), 1, fout);
    }

    free(in);
    free(out);
    fclose(fin);
    fclose(fout);
    return 0;
}

int CCodecTools::decode_opus_file(const char *in_file_path, const char *out_file_path) {
    
    OggOpusFileAnalyser opus_analyser(in_file_path);
    opus_analyser.start_analysis();
    OpusHeader opus_header;
    
    if (opus_analyser.get_opus_header(&opus_header) == false) {
        return -1;
    }
    
    FILE *fp = fopen(out_file_path, "wb");
    if (fp == NULL || fp == 0) {
        return -1;
    }
    
    CWavTools::WAV_HEADER header = CWavTools::make_wav_header(opus_header.input_sample_rate, opus_header.channels, BIT_PER_SAMPLE, 0);
    
    int err = 0;
    OpusDecoder *dec = opus_decoder_create(opus_header.input_sample_rate, opus_header.channels, &err);
    if (err != OPUS_OK) {
        fprintf(stderr, "cannnot decode opus: %s\n", opus_strerror(err));
        dec = NULL;
        return -1;
    }
    
    fwrite(&header, sizeof(CWavTools::WAV_HEADER), 1, fp);
    
    int pcm_data_size = 0;
    int frame_count = 0;
    std::vector<std::string> opus_datas = opus_analyser.get_all_opus_data();
    opus_int16* out = NULL;//(opus_int16*)malloc(opus_header.input_sample_rate * opus_header.channels * FRAME_SIZE_DUR * sizeof(opus_int16) * frame_count);

    for (auto &data : opus_datas) {
        int frames = opus_packet_get_nb_frames((const unsigned char*)data.data(), (int)data.length());
        
        if (frames > frame_count) {
            frame_count = frames;
            out = (opus_int16*)realloc(out, opus_header.input_sample_rate * opus_header.channels * FRAME_SIZE_DUR * sizeof(opus_int16) * frame_count);
        }
        
        if (out == NULL) {
            return -1;
        }
        
        int decode_len = opus_decode(dec, (const unsigned char*)data.data(), (int)data.length(), (opus_int16*)out, opus_header.input_sample_rate * FRAME_SIZE_DUR * frame_count, 0);
        if (decode_len < 0) {
            return -1;
        }
        
        fwrite(out, decode_len * sizeof(short), 1, fp);
        pcm_data_size += decode_len * sizeof(short);
    }
    
    fseek(fp, 0, SEEK_SET);
    header = CWavTools::make_wav_header(opus_header.input_sample_rate, opus_header.channels, BIT_PER_SAMPLE, pcm_data_size);
    fwrite(&header, sizeof(CWavTools::WAV_HEADER), 1, fp);
    opus_decoder_destroy(dec);
    
    free(out);
    fclose(fp);
    return 0;
}
