#include <complex>
#include <cstdint>
#include <numbers>
#include <vector>

std::vector<std::complex<double>> mixer(const std::vector<std::complex<double>>& samples, double frequency, double sample_period, uint32_t sample_shift = 0) {
    const double phase_increment = 2.0 * std::numbers::pi_v<double> * frequency * sample_period;
    std::vector<std::complex<double>> mixed_samples(samples.size());

    for (size_t i = 0; i < samples.size(); i++) {
        mixed_samples[i] = samples[i] * std::exp(std::complex<double>(0.0f, phase_increment * (i + sample_shift)));
    }

    return mixed_samples;
}

double goertzel(std::vector<std::complex<double>>::const_iterator begin,
                std::vector<std::complex<double>>::const_iterator end,
                double frequency, double sample_period) {
    const size_t N = std::distance(begin, end);
    const size_t k = static_cast<size_t>(0.5 + (N * frequency) * sample_period);
    const double omega = 2.0 * std::numbers::pi_v<double> * k / N;
    const std::complex<double> coeff = std::exp(std::complex<double>(0.0, -omega));

    std::complex<double> s_prev(0.0, 0.0);
    std::complex<double> s_prev2(0.0, 0.0);
    std::complex<double> s(0.0, 0.0);

    for (auto it = begin; it != end; ++it) {
        s = *it + coeff * s_prev - s_prev2;
        s_prev2 = s_prev;
        s_prev = s;
    }

    return std::abs(s);
}