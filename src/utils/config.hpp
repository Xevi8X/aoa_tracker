#pragma once

#include <cstdint>

struct Configuration
{
    // RTL SDR
    struct {
        uint32_t frequency = 100000000;
        uint32_t sample_rate = 2048000;
        int gain = 20;
        int buffer_num = 0;
        size_t buffer_size = 1024*4;
    } rtl_sdr;

    // Detect harmonics
    struct
    {
        uint32_t base_frequency = 500000;
        float magnitude_reference = 4.0f;
        float threshold_rising = 0.8f;
        float threshold_falling = 0.2f;
        float deadzone = 0.1f;
        uint16_t tuning_loops = 10;
    } synchronization;
};
