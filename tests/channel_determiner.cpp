#include "channel_determiner/channel_determiner.hpp"

int main() {

    ChannelDeterminer channel_determiner{Configuration{}};

    uint64_t time = 0;

    while(channel_determiner.get_state() != ChannelDeterminer::State::RUNNING) {

        auto in_window = time % 1000;
        auto sync_signal_probabilility = (in_window < 200) ? 0.9f : 0.1f; // Simulate a sync signal
        int channel = channel_determiner.determine(time, sync_signal_probabilility);
        time += 50;
    }

    std::cout << "Tuning completation time: " << time << " us" << std::endl;

    for (size_t i = 0; i < 200; i++)
    {
        auto in_window = time % 1000;
        auto sync_signal_probabilility = (in_window < 200) ? 0.9f : 0.1f; // Simulate a sync signal
        int channel = channel_determiner.determine(time, sync_signal_probabilility);
        std::cout << "Time: " << time << " us, Channel: " << channel << std::endl;
        time += 50;
    }
    

    return 0;
}