#pragma once
#include <jack/jack.h>
#include <vector>
#include <array>
#include <string>
#include <iostream>
#include "audio-sink-base.h"

class AudioSink : public AudioSinkBase
{
public:
    using JackPorts = std::vector<jack_port_t*>;

    AudioSink(int a_ch);
    ~AudioSink();

    int process(jack_nframes_t);

    const AudioBuffer& getBuffer(int chan) const override {
        if (chan > 0 && chan <= buffers.size())
            return buffers[chan - 1];
        std::cerr << "[AudioSink] invalid channel number\n";
        static const AudioBuffer err;
        return err;
    }

    int getBufferSize() override {return (int)bufferSize;};
    void start() override;
    void stop() override;

    void connect();

private:
    jack_nframes_t bufferSize;
    jack_client_t* client;
    JackPorts ports;

    static int outside_process_call(jack_nframes_t, void*); // TODO: find a better naming
};
