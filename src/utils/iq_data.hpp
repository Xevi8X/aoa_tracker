#pragma once

#include <complex>
#include <memory>

struct IQData
{
    uint64_t time;
    std::unique_ptr<std::complex<float>[]> buffer;
};