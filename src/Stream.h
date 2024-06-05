#pragma once

#include <portaudio.h>

struct StreamConfig
{
    static StreamConfig default_config();
    int sample_rate;
    unsigned long buffer_size;
};

class Stream
{
   public:
    Stream();
    ~Stream();
    void start();
    void stop();

    int (*callback)(const void* input_buffer,
                    void* output_buffer,
                    unsigned long buffer_size,
                    const PaStreamCallbackTimeInfo* time_info,
                    PaStreamCallbackFlags status_flags,
                    void* user_data);

    StreamConfig cfg;

   private:
    void query_input_device();
    void query_output_device();
    void setup_params();
    int n_input_channels = 2;
    int n_output_channels = 2;
    int input_device;
    int output_device;
    PaStreamParameters input_params;
    PaStreamParameters output_params;
    PaStream* pa_stream;
};
