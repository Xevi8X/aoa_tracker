#pragma once

#include <mutex>
#include <queue>

#include "detect_haramonics/detect_harmonics.hpp"
#include "dsp/fft.hpp"
#include "dsp/goertzel.hpp"
#include "dsp/hysteresis.hpp"
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

    GoertzelFilter goertzel_filter{configuration.rtl_sdr.sample_rate, configuration.rtl_sdr.buffer_size};
    FFT fft{configuration.rtl_sdr.sample_rate, configuration.rtl_sdr.buffer_size};

    DetectHarmonicsTriangular detect_harmonics{configuration.rtl_sdr.sample_rate,
                                               configuration.rtl_sdr.buffer_size,
                                               configuration.synchronization.magnitude_reference};
    Hysteresis hysteresis{configuration.synchronization.threshold_rising,
                          configuration.synchronization.threshold_falling};
};