#pragma once

#include <atomic>
#include <complex>
#include <functional>
#include <iostream>
#include <rtl-sdr.h>
#include <thread>
#include <vector>

#include "utils/config.hpp"

class RTLSDR
{
public:
    RTLSDR(const Configuration &configuration) : configuration(configuration), dev(nullptr) {}
    ~RTLSDR() { _close(); };

    bool init()
    {
        int device_count = rtlsdr_get_device_count();
        if (device_count < 1)
        {
            std::cerr << "No RTL-SDR devices found." << std::endl;
            return false;
        }

        if (rtlsdr_open(&dev, 0) != 0)
        {
            std::cerr << "Failed to open RTL-SDR device." << std::endl;
            return false;
        }

        if (rtlsdr_set_sample_rate(dev, configuration.sample_rate) != 0)
        {
            std::cerr << "Failed to set sample rate." << std::endl;
            return false;
        }

        if (rtlsdr_set_center_freq(dev, configuration.frequency) != 0)
        {
            std::cerr << "Failed to set center frequency." << std::endl;
            return false;
        }

        if (configuration.gain >= 0)
        {
            if (rtlsdr_set_tuner_gain_mode(dev, 1) != 0 ||
                rtlsdr_set_tuner_gain(dev, configuration.gain) != 0)
            {
                std::cerr << "Failed to set manual gain." << std::endl;
                return false;
            }
        }
        else
        {
            rtlsdr_set_tuner_gain_mode(dev, 0); // auto gain
        }

        if (rtlsdr_reset_buffer(dev) != 0)
        {
            std::cerr << "Failed to reset buffer." << std::endl;
            return false;
        }

        return true;
    }

    void set_callback(std::function<void(uint8_t *, uint32_t)> callback)
    {
        this->callback = callback;
    }

    void run()
    {
        if (!dev)
        {
            std::cerr << "Device not initialized." << std::endl;
            return;
        }

        running = true;

        thread = std::thread([this]() {
            _job();
        });

    }

private:
    const Configuration &configuration;

    rtlsdr_dev_t *dev;
    std::function<void(uint8_t *, uint32_t)> callback;

    std::atomic<bool> running{false};
    std::thread thread;


    void _job()
    {
        rtlsdr_read_async(dev, [](uint8_t *buf, uint32_t len, void *ctx) {
            static_cast<RTLSDR*>(ctx)->_callback(buf, len, ctx);
        },
        this,
        configuration.buffer_num,
        configuration.buffer_size * 2);
    }

    void _callback(uint8_t *buf, uint32_t len, void *ctx)
    {
        auto* sdr = static_cast<RTLSDR*>(ctx);

        if (!sdr->running) {
            rtlsdr_cancel_async(sdr->dev);
            return;
        }

        sdr->callback(buf, len);
    }

    void _close()
    {
        running = false;
        if (thread.joinable())
        {
            thread.join();
        }
        if (dev)
        {
            rtlsdr_close(dev);
            dev = nullptr;
        }
    }
};