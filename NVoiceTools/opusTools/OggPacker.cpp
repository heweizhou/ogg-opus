//
//  OggPacker.cpp
//  NVoiceTools
//
//  Created by 周贺伟 on 2017/1/19.
//  Copyright © 2017年 zhw. All rights reserved.
//

#include "OggPacker.hpp"
#include <stdlib.h>
#include <string.h>
#include <string>

#include "UtilityTools.hpp"
#include "defines.h"
#include <assert.h>

COggPacker::COggPacker()
{
    m_index = 0;
    ogg_stream_init(&m_ogg_s, CUtilityTools::get_serino());
}

COggPacker::COggPacker(int serino)
{
    m_index = 0;
    ogg_stream_init(&m_ogg_s, serino);
}

COggPacker::COggPacker(const COggPacker& ref)
{
    
    printf("COggPacker copy struct no supported!\n");
    assert(0);
//    m_index = 0;
//    ogg_stream_init(&m_ogg_s, CUtilityTools::get_serino());
}

COggPacker::~COggPacker()
{
    ogg_stream_clear(&m_ogg_s);
}

std::string COggPacker::get_page_data()
{
    std::string ret_data;
    ogg_page page;
    int ret = 0;
    while((ret = ogg_stream_flush(&m_ogg_s, &page))){
        if(!ret)
            break;
        ret_data.append((char*)page.header, page.header_len);
        ret_data.append((char*)page.body, page.body_len);
    }
    return ret_data;
}


void COggPacker::add_start_packet(const char* data, unsigned long data_len)
{
    ogg_packet packet;
    packet.packet = (unsigned char*)data;
    packet.bytes = data_len;
    packet.b_o_s = 1;
    packet.e_o_s = 0;
    packet.granulepos = 0;
    packet.packetno = m_index++;
    ogg_stream_packetin(&m_ogg_s, &packet);
    
}

void COggPacker::add_end_packet(const char* data, unsigned long data_len)
{
    ogg_packet packet;
    packet.packet = (unsigned char*)data;
    packet.bytes = data_len;
    packet.b_o_s = 0;
    packet.e_o_s = 1;
    packet.granulepos = FRAME_SIZE_DUR * 48000 * m_index;
    packet.packetno = m_index++;
    ogg_stream_packetin(&m_ogg_s, &packet);
}

void COggPacker::add_packet(const char* data, unsigned long data_len, int sample_rate)
{
//    m_current_granulepos = sample_rate * FRAME_SIZE_DUR * 48000 / sample_rate * m_index;
    
    ogg_packet packet;
    packet.packet = (unsigned char*)data;
    packet.bytes = data_len;
    packet.b_o_s = 0;
    packet.e_o_s = 0;
    packet.granulepos = FRAME_SIZE_DUR * 48000 * m_index;
//    packet.granulepos = m_current_granulepos;
    packet.packetno = m_index++;
    ogg_stream_packetin(&m_ogg_s, &packet);
    
    
}
