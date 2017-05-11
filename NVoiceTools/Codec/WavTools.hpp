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

#ifndef WavTools_hpp
#define WavTools_hpp

#include <stdio.h>

class CWavTools {
public:
    typedef struct WAV_HEADER_S
    {
        char            riffType[4];    //4byte,资源交换文件标志:RIFF
        unsigned int    riffSize;       //4byte,从下个地址到文件结尾的总字节数
        char            waveType[4];    //4byte,wave文件标志:WAVE
        char            formatType[4];  //4byte,波形文件标志:FMT
        unsigned int    formatSize;     //4byte,音频属性(compressionCode,numChannels,sampleRate,bytesPerSecond,blockAlign,bitsPerSample)所占字节数
        unsigned short  compressionCode;//2byte,编码格式(1-线性pcm-WAVE_FORMAT_PCM,WAVEFORMAT_ADPCM)
        unsigned short  numChannels;    //2byte,通道数
        unsigned int    sampleRate;     //4byte,采样率
        unsigned int    bytesPerSecond; //4byte,传输速率
        unsigned short  blockAlign;     //2byte,数据块的对齐
        unsigned short  bitsPerSample;  //2byte,采样精度
        char            dataType[4];    //4byte,数据标志:data
        unsigned int    dataSize;       //4byte,从下个地址到文件结尾的总字节数，即除了wav header以外的pcm data length
    }WAV_HEADER;
public:
    static WAV_HEADER make_wav_header(int sampleRate, int numChannels, int bitsPerSample, int data_len);
    static int get_wave_frame_size(WAV_HEADER* header);
    static int get_wave_frame_bytes(WAV_HEADER* header);
};

#endif /* WavTools_hpp */
