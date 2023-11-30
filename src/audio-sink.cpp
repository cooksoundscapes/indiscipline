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
        for (size_t s{0}; s < bufferSize; s++)
        {
            buffers[i][s] = temp[s];
        }
    }  
    return 0;
}

AudioSink::AudioSink(int chan_count)
{
    buffers.resize(chan_count);
    if((client = jack_client_open("craddle", JackNullOption, NULL)) == NULL)
    {
        std::cerr << "\nJack server not running, program will not process audio." << std::endl;        
        for (auto& buff : buffers)
        {
            buff.resize(1);
        }
        return;
    } else {
        bufferSize = jack_get_buffer_size(client);
    }

    //pre allocate & initialize audio buffers
    for (auto& buff: buffers)
    {
        buff.resize(bufferSize);
    }
    //pre allocate only the jack ports;
    ports.reserve(chan_count);

    //register audio inputs
    for (int i{0}; i < chan_count; i++) 
    {
        std::string portName = "input_"+std::to_string(i+1);
        ports.push_back(jack_port_register(
            client, 
            portName.c_str(), 
            JACK_DEFAULT_AUDIO_TYPE, 
            JackPortIsInput, 
            0
        ));
    }

    jack_set_process_callback(client, AudioSink::outside_process_call, this);

    if (jack_activate (client)) 
    {
        std::cerr << "Could not activate client";
    }

    //FOR TESTING PURPOSES ONLY! (maybe)
    jack_connect(client, "system:capture_1", "craddle:input_1");
    jack_connect(client, "system:capture_2", "craddle:input_2");
};

void AudioSink::restart() {
    if (!client) return;
    if(jack_deactivate(client)) {
        std::cerr << "Failed to close client;\n";
    }
    if (jack_activate (client)) 
    {
        std::cerr << "Could not activate client";
    }

}

AudioSink::~AudioSink()
{
    if (!client) return;
    if(jack_deactivate(client)) {
        std::cerr << "Failed to close client;\n";
    }
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