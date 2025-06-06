#pragma once

#include "detect_haramonics/detect_harmonics.hpp"
#include "dsp/hysteresis.hpp"
#include "utils/config.hpp"
#include "utils/iq_data.hpp"

#include <iostream>

class ChannelDeterminer
{
public:
    ChannelDeterminer(const Configuration& configuration) :
        configuration(configuration),
        hysteresis{configuration.synchronization.threshold_rising,configuration.synchronization.threshold_falling},
        detect_harmonics{configuration.rtl_sdr.sample_rate, configuration.rtl_sdr.buffer_size, configuration.synchronization.magnitude_reference}
    {
        detect_harmonics.set_frequencies(
            configuration.synchronization.base_frequency,
            configuration.rtl_sdr.frequency,
            configuration.rtl_sdr.sample_rate
        );
    }

    enum class State
    {
        INITIALIZING,
        TUNING,
        RUNNING,
        WAITING_FOR_SYNC,
    };

    /// @brief Determine the channel based on the provided IQ data buffer.
    /// @param buffer IQ data.
    /// @return the channel index or -1 if the channel cannot be determined. 0 is the synchronization channel.
    int determine(const IQData& buffer)
    {
        const float sync_signal_probabilility = detect_harmonics.process(buffer.buffer.get());
        return determine(buffer.time, sync_signal_probabilility);
    }

    int determine(uint64_t time, float sync_signal_probabilility)
    {
        const auto sync_signal_detected = hysteresis.process(sync_signal_probabilility);
        return _determinate(time, sync_signal_detected);
    }

    State get_state() const
    {
        return state;
    }

private:
    const Configuration& configuration;
    Hysteresis hysteresis;
    DetectHarmonicsTriangular detect_harmonics;

    State state{State::INITIALIZING};

    int channel_num{-1};
    uint64_t sync_time{0ULL};
    uint64_t window_size{0ULL};
    uint64_t deadzone{0ULL};

    struct 
    {
        uint16_t count{0U};
        uint64_t total_cycle_time{0ULL};
        uint64_t total_window_size{0ULL};
    } tuning;

    int _determinate(uint64_t time, HysteresisState sync_signal_state)
    {
        switch (state)
        {
        case State::INITIALIZING:
            return _initializing(time, sync_signal_state);

        case State::TUNING:
            return _tuning(time, sync_signal_state);
        
        case State::RUNNING:
            return _running(time, sync_signal_state);

        case State::WAITING_FOR_SYNC:
            return _sync(time, sync_signal_state);
        }
        return -1;
    }

    int _initializing(uint64_t time, HysteresisState sync_signal_state)
    {
        if (sync_signal_state != HysteresisState::RISING)
        {
            return -1;
        }

        std::cout << "First sync signal" << std::endl;

        sync_time = time;
        state = State::TUNING;
        tuning.count = 0U;
        tuning.total_cycle_time = 0ULL;
        tuning.total_window_size = 0ULL;
        return -1;
    }

    int _tuning(uint64_t time, HysteresisState sync_signal_state)
    {
        uint64_t from_sync = time - sync_time;

        if (sync_signal_state == HysteresisState::FALLING)
        {
            std::cout << "Falling: " << from_sync << " us." << std::endl;
            tuning.total_window_size += from_sync;
        }

        if (sync_signal_state == HysteresisState::RISING)
        {
            std::cout << "Rising: " << from_sync << " us." << std::endl;
            sync_time = time;
            tuning.count++;
            tuning.total_cycle_time += from_sync;

            if (tuning.count >= configuration.synchronization.tuning_loops)
            {
                uint64_t avg_cycle_time = tuning.total_cycle_time / tuning.count;

                window_size = tuning.total_window_size / tuning.count;
                deadzone = static_cast<uint64_t>(window_size * configuration.synchronization.deadzone);

                channel_num = static_cast<int>((avg_cycle_time + window_size / 2)  / window_size);

                state = State::RUNNING;

                std::cout << "===========================" << std::endl;
                std::cout << "Tuning completed. " << std::endl;
                std::cout << "Channel number: " << channel_num << std::endl;
                std::cout << "Window size: " << window_size << " us." << std::endl;
                std::cout << "Deadzone: " << deadzone << " us." << std::endl;
                std::cout << "===========================" << std::endl;
            }
        }

        return -1; // Still tuning
    }

    int _running(uint64_t time, HysteresisState sync_signal_state)
    {
        uint64_t from_sync = time - sync_time;
        int channel = static_cast<int>(from_sync/ window_size);
        uint64_t in_window = from_sync - channel * window_size;

        if (sync_signal_state == HysteresisState::RISING)
        {
            return _sync(time, sync_signal_state);
        }

        if(channel >= channel_num)
        {
            state = State::WAITING_FOR_SYNC;
            return -1;
        }

        if (in_window < deadzone)
        {
            return -1;
        }

        if (in_window > window_size - deadzone)
        {
            if (channel == channel_num - 1)
            {
                state = State::WAITING_FOR_SYNC;
            }
            return -1;
        }

        return channel;
    }

    int _sync(uint64_t time, HysteresisState sync_signal_state)
    {
        if (sync_signal_state == HysteresisState::RISING)
        {
            uint64_t time_diff = time - sync_time;
            window_size = time_diff / channel_num;
            sync_time = time;
            state = State::RUNNING;
        }
        return -1;
    }

};