#pragma once

#include <mutex>
#include <queue>

#include "dsp/fft.hpp"
#include "dsp/goertzel.hpp"
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

    GoertzelFilter goertzel_filter{configuration.sample_rate, configuration.buffer_size};
    FFT fft{configuration.sample_rate, configuration.buffer_size};
};