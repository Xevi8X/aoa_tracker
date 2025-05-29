#pragma once

enum class HysteresisState
{
    LOW,
    RISING,
    HIGH,
    FALLING
};

class Hysteresis
{
public:
    Hysteresis(float threshold_rising = 0.8f, float threshold_falling = 0.2f)
        : state(false), threshold_rising(threshold_rising), threshold_falling(threshold_falling)
    {
    }

    HysteresisState process(float value)
    {
        if (state)
        {
            if (value < threshold_falling)
            {
                state = false;
                return HysteresisState::FALLING;
            }
        }
        else
        {
            if (value > threshold_rising)
            {
                state = true;
                return HysteresisState::RISING;
            }
        }
        return state ? HysteresisState::HIGH : HysteresisState::LOW;
    }

private:
    bool state;

    const float threshold_rising;
    const float threshold_falling;
};