#include "application.hpp"

#include <fstream>


void Application::init()
{
    rtl_sdr.set_callback([this](uint8_t *buf, uint32_t len) {
        const uint64_t time_diff = 1000000 / 2 * len / configuration.sample_rate;

        if (len/2 != configuration.buffer_size)
        {
            std::cerr << "Buffer size mismatch: expected " << configuration.buffer_size << ", got " << len/2 << std::endl;
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
}

void Application::run()
{
    rtl_sdr.run();

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

        detect_wave.process(data.buffer.get());

        fft.process(data.buffer.get());
        auto results = fft.get_result();

        std::ofstream out;
        out.open("fft.csv", std::ios::out | std::ios::trunc);


        out << "freq,abs1,abs2,arg1,arg2" << std::endl;
        for (size_t i = 0; i < configuration.buffer_size; ++i)
        {
            goertzel_filter.set_frequency(results.frequency[i]);
            goertzel_filter.process(data.buffer.get());
            auto res = goertzel_filter.get_result();
            out << results.frequency[i] << "," << std::abs(results.value[i]) << "," << std::abs(res) << "," << std::arg(results.value[i]) << "," << std::arg(res) << std::endl;
        }


        return;
    }
}