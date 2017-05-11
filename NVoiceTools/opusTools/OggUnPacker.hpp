//
//  OggUnPacker.hpp
//  NVoiceTools
//
//  Created by 周贺伟 on 2017/1/22.
//  Copyright © 2017年 zhw. All rights reserved.
//

#ifndef OggUnPacker_hpp
#define OggUnPacker_hpp

#include <stdio.h>
#include <vector>
#include <map>
#include <string>
#include "ogg.h"



class COggUnPacker
{
public:
    typedef struct tag_ogg_page_info{
        std::vector<std::string> packets_in_page;
    }ogg_page_info;
public:
    COggUnPacker();
    COggUnPacker(const COggUnPacker& ref);
    ~COggUnPacker();
public:
    void                    add_data(char* buffer, unsigned long buffer_size);
    int                     get_page_counts();
    const ogg_page_info*    get_page_info(int index);
    void                    reset();
private:
    ogg_sync_state  m_ogg_sync_state;
    std::map<int, ogg_page_info>    m_all_data;
};

#endif /* OggUnPacker_hpp */
