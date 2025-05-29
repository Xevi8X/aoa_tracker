#pragma once

#include "dsp/common.hpp"
#include "dsp/goertzel.hpp"

#include <complex>
#include <iostream>
#include <ranges>
#include <vector>

class DetectHarmonics
{
public:
    // Return the probability of a harmonics being present in the signal.
    float process(const std::complex<float>* buffer)
    {
        if (!buffer)
            throw std::invalid_argument("Buffer cannot be null.");

        if (harmonics.empty())
        {
            throw std::runtime_error("Harmonics frequencies are not set.");
        }

        auto H = _get_harmonics(buffer);
        
        return _magnitude_matching(H);
    }

protected:
    DetectHarmonics(uint32_t sample_rate, size_t buffer_size, float magnitude_reference = 1.0f) :
        goertzel_filter(sample_rate, buffer_size),
        magnitude_reference(magnitude_reference)
    {
    }

    std::vector<float> harmonics    = {}; // Hz
    std::vector<float> magnitudes   = {}; // Normalized magnitudes (0, 1>

    GoertzelFilter goertzel_filter;

    const float magnitude_reference;

    std::vector<std::complex<float>> _get_harmonics(const std::complex<float>* buffer)
    {
        std::vector<std::complex<float>> H{};
        H.reserve(harmonics.size());
        for (const auto& freq : harmonics)
        {
            goertzel_filter.set_frequency(freq);
            goertzel_filter.process(buffer);
            H.push_back(goertzel_filter.get_result());
        }
        return H;
    }

    float _magnitude_matching(const std::vector<std::complex<float>>& H)
    {
        if (magnitudes.size() != harmonics.size())
        {
            return 1.0f;
        }
        float magnitude_sum = 0.0f;

        for (size_t i = 0; i < magnitudes.size(); ++i) {
            float signal_magnitude = std::abs(H[i]);
            magnitude_sum += signal_magnitude / magnitudes[i];
        }

        const float magnitude_avg = magnitude_sum / magnitudes.size();
        // std::cout << "Magnitude average: " << magnitude_avg << std::endl;
        return magnitude_avg > magnitude_reference ? 1.0f : magnitude_avg / magnitude_reference;
    }
};

class DetectHarmonicsTriangular : public DetectHarmonics
{
public:
    DetectHarmonicsTriangular(uint32_t sample_rate, size_t buffer_size, float magnitude_reference = 1.0f)
        : DetectHarmonics(sample_rate, buffer_size, magnitude_reference)
    {
    }

    void set_frequencies(uint32_t base_frequency, uint32_t central_frequency, uint32_t sample_rate)
    {
        const uint32_t min_frequency = central_frequency - sample_rate / 2;
        const uint32_t max_frequency = central_frequency + sample_rate / 2;

        harmonics.clear();
        magnitudes.clear();

        std::vector<uint32_t> harmonics_indices;

        const uint32_t min_harmonic_index = min_frequency / base_frequency + 1;
        const uint32_t max_harmonic_index = max_frequency / base_frequency;


        std::cout << "===========================" << std::endl;
        std::cout << "Harmonics indecies: from " << min_harmonic_index << " to " << max_harmonic_index << std::endl;

        for (uint32_t i = min_harmonic_index; i <= max_harmonic_index; ++i)
        {
            if (i % 2 == 0) 
            {
                // Only odd harmonics
                continue;
            }
            harmonics_indices.push_back(i);
            harmonics.push_back(base_frequency * i);
        }

        if (harmonics.empty())
        {
            throw std::runtime_error("No harmonics found in the specified range.");
        }

        auto harmonic_mag = [](uint32_t index) -> float {
            return 1.0f / (index * index);
        };

        const auto min_harmonic_mag = harmonic_mag(harmonics_indices.front());

        for(size_t i = 0; i < harmonics.size(); ++i)
        {
            harmonics[i] = harmonics[i] - central_frequency;
            magnitudes.push_back(harmonic_mag(harmonics_indices[i]) / min_harmonic_mag); // Default normalized magnitude
            std::cout << harmonics[i] << ", " << magnitudes[i] << std::endl;
        }
        std::cout << "===========================" << std::endl;
    }
};
