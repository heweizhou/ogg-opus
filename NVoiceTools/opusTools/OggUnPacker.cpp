//
//  OggUnPacker.cpp
//  NVoiceTools
//
//  Created by 周贺伟 on 2017/1/22.
//  Copyright © 2017年 zhw. All rights reserved.
//

#include "OggUnPacker.hpp"
#include <string.h>
#include <assert.h>

COggUnPacker::COggUnPacker()
{
    ogg_sync_init(&m_ogg_sync_state);
}

COggUnPacker::COggUnPacker(const COggUnPacker& ref)
{
    
    printf("COggUnPacker copy struct no supported!\n");
    assert(0);
    //    m_index = 0;
    //    ogg_stream_init(&m_ogg_s, CUtilityTools::get_serino());
}

COggUnPacker::~COggUnPacker()
{
    ogg_sync_clear(&m_ogg_sync_state);
}

void COggUnPacker::add_data(char* buffer, unsigned long buffer_size)
{
    char* _buffer = ogg_sync_buffer(&m_ogg_sync_state, buffer_size);
    memcpy(_buffer, buffer, buffer_size);
    ogg_sync_wrote(&m_ogg_sync_state, buffer_size);
    
    ogg_page page;
    ogg_stream_state os;
    
    while(1){
        
        int re = ogg_sync_pageout(&m_ogg_sync_state, &page);
        
        if (re == 0) {
            return ;
        } else if (re == 1) {
            
            ogg_page_info info;
            ogg_stream_init(&os, ogg_page_serialno(&page));
            ogg_stream_pagein(&os, &page);
            ogg_packet packet;
            while (ogg_stream_packetout(&os, &packet) == 1){
                info.packets_in_page.push_back(std::string((char*)packet.packet, packet.bytes));
            }
            
            int index = (int)m_all_data.size();
            m_all_data.insert(std::pair<int, ogg_page_info>(index, info));
            break;
        }
    }
    
    while (1) {
        
        int re = ogg_sync_pageout(&m_ogg_sync_state, &page);
        
        if (re == 0) {
            break ;
        } else if (re == 1) {
            ogg_page_info info;
            ogg_stream_pagein(&os, &page);
            ogg_packet packet;
            while (ogg_stream_packetout(&os, &packet) == 1){
                info.packets_in_page.push_back(std::string((char*)packet.packet, packet.bytes));
            }
            
            int index = (int)m_all_data.size();
            m_all_data.insert(std::pair<int, ogg_page_info>(index, info));
        }
    }
end:
    ogg_stream_clear(&os);
}

int COggUnPacker::get_page_counts()
{
    return (int)m_all_data.size();
}

const COggUnPacker::ogg_page_info* COggUnPacker::get_page_info(int index)
{
    auto find_it = m_all_data.find(index);
    if (find_it != m_all_data.end()) {
        return &(find_it->second);
    }
    return NULL;
}

void COggUnPacker::reset()
{
    m_all_data.clear();
    ogg_sync_reset(&m_ogg_sync_state);
}
