#pragma once

#include <complex>
#include <memory>
#include <cmath>
#include <stdexcept>
#include <algorithm>

struct Spectrum
{
    size_t N;
    const float* frequency;
    const std::complex<float>* value;
};

class FFT
{
public:
    FFT(uint32_t sample_rate, size_t buffer_size)
        : sample_rate(sample_rate), N(buffer_size)
    {
        if ((N == 0) || (N & (N - 1)) != 0)
            throw std::invalid_argument("Buffer size must be a power of 2 and a power of 2.");

        freq_axis = std::make_unique<float[]>(N);
        shifted_buffer = std::make_unique<std::complex<float>[]>(N);

        for (size_t i = 0; i < N; ++i)
            freq_axis[i] = (static_cast<int>(i) - static_cast<int>(N / 2)) * static_cast<float>(sample_rate) / N;
    }

    void process(const std::complex<float>* input)
    {
        std::unique_ptr<std::complex<float>[]> tmp = std::make_unique<std::complex<float>[]>(N);
        std::copy(input, input + N, tmp.get());

        _fft(tmp.get());
        _fftshift(tmp.get(), shifted_buffer.get());
    }

    Spectrum get_result() const
    {
        return {
            .N = N,
            .frequency = freq_axis.get(),
            .value = shifted_buffer.get()
        };
    }

private:
    uint32_t sample_rate;
    size_t N;

    std::unique_ptr<float[]> freq_axis;
    std::unique_ptr<std::complex<float>[]> shifted_buffer;

    void _fft(std::complex<float>* data)
    {
        size_t j = 0;
        for (size_t i = 1; i < N; ++i) {
            size_t bit = N >> 1;
            while (j & bit) {
                j ^= bit;
                bit >>= 1;
            }
            j ^= bit;
            if (i < j) std::swap(data[i], data[j]);
        }

        for (size_t len = 2; len <= N; len <<= 1) {
            float angle = -2.0f * static_cast<float>(M_PI) / len;
            std::complex<float> wlen(std::cos(angle), std::sin(angle));

            for (size_t i = 0; i < N; i += len) {
                std::complex<float> w(1.0f);
                for (size_t j = 0; j < len / 2; ++j) {
                    std::complex<float> u = data[i + j];
                    std::complex<float> t = w * data[i + j + len / 2];
                    data[i + j] = u + t;
                    data[i + j + len / 2] = u - t;
                    w *= wlen;
                }
            }
        }
    }

    void _fftshift(const std::complex<float>* input, std::complex<float>* output)
    {
        size_t half = N / 2;
        for (size_t i = 0; i < half; ++i) {
            output[i] = input[i + half];
            output[i + half] = input[i];
        }
    }
};
