//
//  OggOpusFileAnalyser.cpp
//  NVoiceTools
//
//  Created by 周贺伟 on 2017/1/22.
//  Copyright © 2017年 zhw. All rights reserved.
//

#include "OggOpusFileAnalyser.hpp"
#include <stdio.h>
#include <stdlib.h>

OggOpusFileAnalyser::OggOpusFileAnalyser(const char* file_path)
{
    m_file_path = file_path;
    m_unPacker = new COggUnPacker();
}

OggOpusFileAnalyser::~OggOpusFileAnalyser()
{
    delete m_unPacker;
}

bool OggOpusFileAnalyser::start_analysis()
{
    FILE *fp = fopen(m_file_path.c_str(), "rb");
    if (fp == NULL || fp == 0) {
        return false;
    }
    
    unsigned long curr_pos = ftell(fp);
    fseek(fp, 0, SEEK_END);
    unsigned long total_size = ftell(fp);
    fseek(fp, curr_pos, SEEK_SET);
    
    if (total_size <= 0) {
        return false;
    }
    
    char* buffer = (char*)malloc(sizeof(char) * total_size);
    fread(buffer, total_size, 1, fp);
    m_unPacker->add_data(buffer, total_size);
    free(buffer);
    
    if (m_unPacker->get_page_counts() == 0) {
        return false;
    }
    
    return true;
}

bool OggOpusFileAnalyser::get_opus_header(OpusHeader* out_header)
{
    if (out_header == NULL) {
        return false;
    }
    
    const COggUnPacker::ogg_page_info* info = m_unPacker->get_page_info(0);
    if (info == NULL) {
        return false;
    }
    
    if (info->packets_in_page.size() > 0) {
        std::string packet_data = info->packets_in_page[0];
        if(opus_header_parse((unsigned char*)packet_data.data(), (int)packet_data.length(), out_header) == 1) {
            return true;
        }
    }
    
    return false;
}

std::string OggOpusFileAnalyser::get_opus_comment()
{
    std::string ret;
    
    const COggUnPacker::ogg_page_info* info = m_unPacker->get_page_info(1);
    if (info == NULL) {
        return ret;
    }
    
    if (info->packets_in_page.size() > 0) {
        std::string packet_data = info->packets_in_page[0];
        if (packet_data.length() > 12) {
            std::string title = std::string((char*)packet_data.data(), 8);
            if (title == "OpusTags") {
                int* commnet_len = (int*)((char*)packet_data.data() + 8);
                
                //check length with comment
                if (packet_data.length() > 12 + *commnet_len) {
                    return std::string((char*)packet_data.data() + 12, *commnet_len);
                }
            }
        }
    }
    
    return ret;
}

std::vector<std::string> OggOpusFileAnalyser::get_all_opus_data()
{
    std::vector<std::string> ret;
    int ogg_page_count = m_unPacker->get_page_counts();
    for (int i = 2; i < ogg_page_count; ++i) {
        const COggUnPacker::ogg_page_info* info = m_unPacker->get_page_info(i);
        if (info == NULL) {
            continue;
        }
        
        ret.insert(ret.end(), info->packets_in_page.begin(), info->packets_in_page.end());
    }
    return ret;
}

std::string OggOpusFileAnalyser::get_all_opus_const_data()
{
    std::string ret;
    int ogg_page_count = m_unPacker->get_page_counts();
    for (int i = 2; i < ogg_page_count; ++i) {
        const COggUnPacker::ogg_page_info* info = m_unPacker->get_page_info(i);
        if (info == NULL) {
            continue;
        }
        
        for ( auto& packet : info->packets_in_page) {
            ret += packet;
        }
    }
    return ret;
}

