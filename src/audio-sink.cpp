#include <iostream>
#include <algorithm>
#include "audio-sink.h"

extern "C" {
#include <string.h>
}

int AudioSink::outside_process_call(jack_nframes_t nframes, void* self)
{
    return static_cast<AudioSink*>(self)->process(nframes);
}

int AudioSink::process(jack_nframes_t nframes) 
{
    for (size_t i{0}; i < ports.size(); i++)
    {
        float* temp = (float*)jack_port_get_buffer(ports[i], nframes);
        if (temp == nullptr) {
            std::cerr << "Failed to get buffer from port " << jack_port_name(ports[i]) << std::endl;
            exit(1);
        }
        for (size_t s{0}; s < bufferSize; s++)
        {
            buffers[i][s] = temp[s];
        }
    }  
    return 0;
}

AudioSink::AudioSink(int chan_count)
{
    this->chan_count = chan_count;
    start();
};

void AudioSink::stop() {
    if (!client) return;
    std::cout << "Stopping audio client\n";
    if(jack_deactivate(client)) {
        std::cerr << "Failed to close client;\n";
    }
    ports.clear();
    for (auto buff : buffers) {
        buff.clear();
    }
    buffers.clear();
}

void AudioSink::start() {
    std::cout << "Starting jack client..\n";
    buffers.resize(chan_count);
    if((client = jack_client_open("craddle", JackNullOption, NULL)) == NULL)
    {
        std::cerr << "\nJack server not running, program will not process audio." << std::endl;        
        for (auto& buff : buffers) {
            buff.resize(1);
        }
        return;
    } else {
        std::cout << "Jack client started\n";
        bufferSize = jack_get_buffer_size(client);
    }
    std::cout << "Buffer size: " << bufferSize << std::endl;
    //pre allocate & initialize audio buffers
    for (auto& buff: buffers) {
        buff.resize(bufferSize);
    }
    //pre allocate only the jack ports;
    ports.reserve(chan_count);

    //register audio inputs
    for (int i{0}; i < chan_count; i++) {
        std::cout << "Registering port " << "input_"+std::to_string(i+1) << std::endl;
        std::string portName = "input_"+std::to_string(i+1);
        auto port = jack_port_register(
            client, 
            portName.c_str(), 
            JACK_DEFAULT_AUDIO_TYPE, 
            JackPortIsInput, 
            0
        );
        if (port == nullptr) {
            std::cerr << "Failed to register port " << portName << std::endl;
        } else {
            ports.push_back(port);
        }
    }
    std::cout << "Registering callback\n";
    jack_set_process_callback(client, AudioSink::outside_process_call, this);

    if (jack_activate(client)) {
        std::cerr << "Could not activate client";
    }

    //FOR TESTING PURPOSES ONLY! (maybe)
    //jack_connect(client, "system:capture_1", "craddle:input_1");
    //jack_connect(client, "system:capture_2", "craddle:input_2");
}

AudioSink::~AudioSink() {
    stop();
}

void AudioSink::connect() 
{
    // Here, it should find the pure data ports
    const char** system_ports { 
        jack_get_ports(client, NULL, NULL, JackPortIsPhysical|JackPortIsOutput)
    };

    std::vector<std::string> sysport_list;

    while( *system_ports != nullptr)
    {
        sysport_list.push_back(*system_ports++);
    }

    for (size_t i{0}; i < ports.size(); i++) 
    {
        const char* destination = jack_port_name(ports[i]);
        const char* source = sysport_list[i].c_str();
        if (jack_connect(client, source, destination)) 
        {
            std::cerr << "Can't connect " << source << " to " << destination << ".\n";
        };
    }
}