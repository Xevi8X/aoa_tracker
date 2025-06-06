#pragma once

#include <mutex>
#include <queue>

#include "channel_determiner/channel_determiner.hpp"
#include "dsp/fft.hpp"
#include "rtl_sdr/rtl_sdr.hpp"
#include "utils/config.hpp"
#include "utils/iq_data.hpp"

class Application
{
public:
    Application() = default;

    void init();

    void run();

private:
    Configuration configuration;
    RTLSDR rtl_sdr{configuration};

    struct
    {
        uint64_t time{0};
        std::mutex mtx;
        std::queue<IQData> buffer;
    } input;


    ChannelDeterminer channel_determiner{configuration};
    FFT fft{configuration.rtl_sdr.sample_rate, configuration.rtl_sdr.buffer_size};
};