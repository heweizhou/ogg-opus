//
//  OggPacker.hpp
//  NVoiceTools
//
//  Created by 周贺伟 on 2017/1/19.
//  Copyright © 2017年 zhw. All rights reserved.
//

#ifndef OggPacker_hpp
#define OggPacker_hpp

#include <stdio.h>
#include "ogg.h"
#include <string>

class COggPacker
{
public:
    COggPacker();
    COggPacker(int serino);
    COggPacker(const COggPacker& ref);
    ~COggPacker();
public:
    void        start();
    void        stop();
    
    void        add_start_packet(const char* data, unsigned long data_len);
    void        add_end_packet(const char* data, unsigned long data_len);
    void        add_packet(const char* data, unsigned long data_len, int sample_rate);
    std::string get_page_data();
private:
    int                 m_index;
    ogg_stream_state    m_ogg_s;
};

#endif /* OggPacker_hpp */
