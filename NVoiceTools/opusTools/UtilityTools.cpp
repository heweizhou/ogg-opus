//
//  UtilityTools.cpp
//  NVoiceTools
//
//  Created by 周贺伟 on 2017/1/22.
//  Copyright © 2017年 zhw. All rights reserved.
//

#include "UtilityTools.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <string>

//int CUtilityTools::get_coding_rate(int sample_rate)
//{
//    int coding_rate = 48000;
//    if(sample_rate > 24000)
//        coding_rate = 48000;
//    else if(sample_rate > 16000)
//        coding_rate = 24000;
//    else if(sample_rate > 12000)
//        coding_rate = 16000;
//    else if(sample_rate > 8000)
//        coding_rate = 12000;
//    else
//        coding_rate = 8000;
//    
//    return coding_rate;
//}

//int CUtilityTools::get_frame_size(int sample_rate)
//{
//    return sample_rate * 0.02 * (48000.0 / get_coding_rate(sample_rate));
//}
int CUtilityTools::get_serino(){
    time_t start_time = time(NULL);
    srand(((getpid()&65535)<<15)^(int)start_time);
    return rand();
}

