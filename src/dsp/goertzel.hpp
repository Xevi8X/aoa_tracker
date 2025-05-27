#pragma once

#include <complex>
#include <cmath>
#include <memory>
#include <numbers>
#include <stdexcept>

class GoertzelFilter 
{
public:
    GoertzelFilter(uint32_t sample_rate, size_t buffer_size)
        : sample_rate(sample_rate), N(buffer_size)
    {
        if (N == 0)
            throw std::invalid_argument("Buffer size must be greater than 0.");
    }

    float set_frequency(float frequency) {
        float K = std::floor(0.5f + (N * frequency / sample_rate));
        true_frequency = (sample_rate * K) / N;
        omega = (2.0f * std::numbers::pi_v<float> * K) / static_cast<float>(N);
        coeff = 2.0f * std::cos(omega);
        return true_frequency;
    }

    void reset() {
        s_prev = {0.0f, 0.0f};
        s_prev2 = {0.0f, 0.0f};
    }

    void process(const std::complex<float>* buffer) {
        reset();
        for (size_t i = 0; i < N; ++i) {
            _process_sample(buffer[i]);
        }
    }

    float get_frequency() const {
        return true_frequency;
    }

    std::complex<float> get_result() const {
        auto correction = std::polar(1.0f, omega * (N - 1));
        std::complex<float> twiddle = std::polar(1.0f, -omega);
        return (s_prev - s_prev2 * twiddle) * correction;
    }

private:
    uint32_t sample_rate;
    size_t N;
    float true_frequency;
    float omega;
    float coeff;

    std::complex<float> s_prev{};
    std::complex<float> s_prev2{};

    void _process_sample(std::complex<float> sample) {
        std::complex<float> s = sample + coeff * s_prev - s_prev2;
        s_prev2 = s_prev;
        s_prev = s;
    }
};