#pragma once

#include <cstdint>

struct Configuration
{
    // RTL SDR
    uint32_t frequency = 100000000;
    uint32_t sample_rate = 2048000;
    int gain = 20;
    int buffer_num = 0;
    int buffer_size = 1024*4;
};
