#pragma once

class Hysteresis
{
public:
    Hysteresis(float threshold_rising = 0.8f, float threshold_falling = 0.2f)
        : state(false), threshold_rising(threshold_rising), threshold_falling(threshold_falling)
    {
    }

    bool process(float value)
    {
        if (state)
        {
            if (value < threshold_falling)
            {
                state = false;
            }
        }
        else
        {
            if (value > threshold_rising)
            {
                state = true;
            }
        }
        return state;
    }

private:
    bool state;

    const float threshold_rising;
    const float threshold_falling;
};