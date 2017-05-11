//
//  OggProtocol.hpp
//  NVoiceTools
//
//  Created by 周贺伟 on 2017/1/20.
//  Copyright © 2017年 zhw. All rights reserved.
//

#ifndef OggProtocol_hpp
#define OggProtocol_hpp

#include <stdio.h>
#include <string>
#include "OggPacker.hpp"
#include "opus_header.h"

class COggProtocol
{
public:
    static void fill_opus_header(OpusHeader* header, int sample_rate, int channel_num, int opus_skip);
    static std::string get_end_ogg_stream_page(COggPacker* packer, int frame_size);
    static std::string serialize_opus_comment_to_ogg_page(COggPacker* packer, std::string commnet, int padding);
    static std::string serialize_opus_header_to_ogg_page(COggPacker* packer, OpusHeader* header);
};

#endif /* OggProtocol_hpp */
