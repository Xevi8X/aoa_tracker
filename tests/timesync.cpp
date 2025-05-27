#include "config.hpp"
#include "rtl_sdr.hpp"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <numeric>
#include <thread>

// 0.003 %

int main() {
    Configuration config;

    std::atomic<uint64_t> t = 0ULL;

    RTLSDR rtl_sdr{config};
    rtl_sdr.set_callback([&](uint8_t *buf, uint32_t len) {
        t += 1000000 / 2 * len / config.sample_rate;
    });

    if (!rtl_sdr.init()) {
        std::cerr << "Failed to initialize RTL-SDR." << std::endl;
        return 1;
    }
    auto start_time = std::chrono::high_resolution_clock::now();
    rtl_sdr.run();

    while(true) {
        std::cout << "Time: " << t.load() << " microseconds" << std::endl;
        auto elapsed = std::chrono::high_resolution_clock::now() - start_time;
        auto elapsed_seconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

        std::cout << "Diff: " << static_cast<int64_t>(t.load()) - elapsed_seconds << " microseconds" << std::endl;
        
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
