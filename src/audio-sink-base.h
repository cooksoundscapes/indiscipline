#pragma once
#include <vector>

class AudioSinkBase {
public:
    using AudioBuffer = std::vector<float>;

    AudioSinkBase() = default;
    virtual ~AudioSinkBase() = default;

    virtual void restart() = 0;

    virtual const AudioBuffer& getBuffer(int chan) const = 0;
    virtual int getBufferSize() = 0;

protected:
    std::vector<AudioBuffer> buffers;
};