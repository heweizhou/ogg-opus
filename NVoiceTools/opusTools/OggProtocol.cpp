//
//  OggProtocol.cpp
//  NVoiceTools
//
//  Created by 周贺伟 on 2017/1/20.
//  Copyright © 2017年 zhw. All rights reserved.
//

#include "OggProtocol.hpp"
#include "UtilityTools.hpp"
#include <string.h>
#include <stdlib.h>

std::string COggProtocol::serialize_opus_comment_to_ogg_page(COggPacker* packer, std::string commnet, int padding)
{
    int comment_len = 8 + 4 + (int)commnet.length() + 4;
    char *p=(char*)malloc(comment_len);
    
    if(p==NULL){
        fprintf(stderr, "malloc failed in comment_init()\n");
        exit(1);
    }
    
    int user_comment_list_length = 0;
    
    memcpy(p, "OpusTags", 8);
    
    int* pin = (int*)(p + 8);
    *pin = (int)commnet.length();
    memcpy(p + 12, commnet.c_str(), commnet.length());
    
    pin = (int*)(p + comment_len - 4);
    *pin = user_comment_list_length;
    
    std::string commnet_string = std::string(p, comment_len);
    packer->add_packet(commnet_string.data(), commnet_string.length(), 0);
    return packer->get_page_data();
}

std::string COggProtocol::get_end_ogg_stream_page(COggPacker* packer, int frame_size)
{
    int stop_frame_bytes = frame_size * 2;
    char* end_buffer = (char*)malloc(stop_frame_bytes);
    memset(end_buffer, 0, stop_frame_bytes);
    packer->add_end_packet(end_buffer, stop_frame_bytes);
    return packer->get_page_data();
}



std::string COggProtocol::serialize_opus_header_to_ogg_page(COggPacker* packer, OpusHeader* header)
{
    unsigned char header_data[512];
    int packet_size = opus_header_to_packet(header, header_data, 512);
    std::string package_data = std::string((char*)header_data, packet_size);
    packer->add_start_packet(package_data.data(), package_data.length());
    return packer->get_page_data();
}

void COggProtocol::fill_opus_header(OpusHeader* header, int sample_rate, int channel_num, int opus_skip)
{
    memset(header, 0, sizeof(OpusHeader));
    header->version = 0;
    header->channels = channel_num;
//    header->preskip = opus_skip * (48000.0 / CUtilityTools::get_coding_rate(sample_rate));
    header->preskip = opus_skip * (48000.0 / sample_rate);
    header->input_sample_rate = sample_rate;
    header->gain = 0;
    header->channel_mapping = channel_num > 8 ? 255:channel_num > 2;
    header->nb_coupled = 0;//channel mapping needed
    header->nb_streams = 0;//channel mapping needed
}
