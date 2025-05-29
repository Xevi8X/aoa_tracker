#include "application.hpp"

#include <fstream>


void Application::init()
{
    rtl_sdr.set_callback([this](uint8_t *buf, uint32_t len) {
        const uint64_t time_diff = 1000000 / 2 * len / configuration.rtl_sdr.sample_rate;

        if (len/2 != configuration.rtl_sdr.buffer_size)
        {
            std::cerr << "Buffer size mismatch: expected " << configuration.rtl_sdr.buffer_size << ", got " << len/2 << std::endl;
            return;
        }
        
        auto buffer = std::make_unique<std::complex<float>[]>(len / 2);

        if (!buffer)
        {
            std::cerr << "Failed to allocate buffer." << std::endl;
            return;
        }

        for (uint32_t i = 0; i < len / 2; ++i)
        {
            buffer[i] = std::complex<float>((buf[2 * i] - 127) / 128.0f, (buf[2 * i + 1] - 127) / 128.0f);
        }

        std::lock_guard<std::mutex> lock(input.mtx);
        input.buffer.push(IQData(input.time, std::move(buffer)));
        input.time += time_diff;
    });

    rtl_sdr.init();

    detect_harmonics.set_frequencies(
        configuration.synchronization.base_frequency,
        configuration.rtl_sdr.frequency,
        configuration.rtl_sdr.sample_rate
    );
}

void Application::run()
{
    rtl_sdr.run();

    bool last_state = false;

    while (true)
    {
        IQData data;
        {
            std::lock_guard<std::mutex> lock(input.mtx);
            if (!input.buffer.empty()) {
                data = std::move(input.buffer.front());
                input.buffer.pop();
            }
            else {
                continue;
            }
        }

        if (data.buffer == nullptr)
            continue;

        float sync_probability = detect_harmonics.process(data.buffer.get());

        bool current_state = hysteresis.process(sync_probability);
        if (current_state != last_state)
        {
            std::cout << last_state << " => " << current_state << std::endl;
            last_state = current_state;
        }
    }
}