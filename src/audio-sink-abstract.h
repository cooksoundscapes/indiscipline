#pragma once
#include <vector>

class AbstractAudioSink {
public:
    using AudioBuffer = std::vector<float>;

    AbstractAudioSink() = default;
    virtual ~AbstractAudioSink() = default;

    virtual const AudioBuffer& getBuffer(int chan) const = 0;

protected:
    std::vector<AudioBuffer> buffers;
};