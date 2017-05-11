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

#include "WavTools.hpp"
#include <string.h>
#include "defines.h"

CWavTools::WAV_HEADER CWavTools::make_wav_header(int sampleRate, int numChannels, int bitsPerSample, int data_len)
{
    // RIFF  4 bytes
    WAV_HEADER header;
    
    strcpy(header.riffType, "RIFF");
    header.riffSize = data_len + sizeof(WAV_HEADER) - sizeof(header.riffType);
    strcpy(header.waveType, "WAVE");
    strcpy(header.formatType, "fmt ");
    header.formatSize = sizeof(header.compressionCode) + sizeof(header.numChannels) + sizeof(header.sampleRate) + sizeof(header.bytesPerSecond) + sizeof(header.blockAlign) + sizeof(header.bitsPerSample);
    
    header.compressionCode = 1;
    header.numChannels = numChannels;
    header.sampleRate = sampleRate;
    header.bitsPerSample = bitsPerSample;
    header.bytesPerSecond = header.sampleRate * header.numChannels * header.bitsPerSample / 8;
    header.blockAlign = header.bitsPerSample * header.numChannels / 2;
    strcpy(header.dataType, "data");
    header.dataSize = data_len;
    
    return header;
}


int CWavTools::get_wave_frame_size(WAV_HEADER* header)
{
    return header->sampleRate * FRAME_SIZE_DUR;
}

int CWavTools::get_wave_frame_bytes(WAV_HEADER* header)
{
    return header->sampleRate * FRAME_SIZE_DUR * header->numChannels * header->bitsPerSample / 8;
}
