//
//  OggOpusFileAnalyser.hpp
//  NVoiceTools
//
//  Created by 周贺伟 on 2017/1/22.
//  Copyright © 2017年 zhw. All rights reserved.
//

#ifndef OggOpusFileAnalyser_hpp
#define OggOpusFileAnalyser_hpp

#include <stdio.h>
#include <string>
#include "OggUnPacker.hpp"
#include "opus_header.h"
//warning big file will exception
class OggOpusFileAnalyser
{
public:
    OggOpusFileAnalyser(const char* file_path);
    ~OggOpusFileAnalyser();
public:
    bool                        start_analysis();
    bool                        get_opus_header(OpusHeader* out_header);
    std::string                 get_opus_comment();
    std::vector<std::string>    get_all_opus_data();
    std::string                 get_all_opus_const_data();
private:
    COggUnPacker*       m_unPacker;
    std::string         m_file_path;
};

#endif /* OggOpusFileAnalyser_hpp */
