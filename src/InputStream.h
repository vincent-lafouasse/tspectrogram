#pragma once

#include <portaudio.h>

struct InputStreamConfig
{
    static InputStreamConfig default_config();
    int sample_rate;
    unsigned long buffer_size;
    int n_channels;
};

class InputStream
{
   public:
    InputStream();
    InputStream(InputStreamConfig stream_config);
    ~InputStream();
    void start();
    void stop();

    void open(int (*callback)(const void* input_buffer,
                              void* output_buffer,
                              unsigned long buffer_size,
                              const PaStreamCallbackTimeInfo* time_info,
                              PaStreamCallbackFlags status_flags,
                              void* user_data));

    InputStreamConfig cfg;

   private:
    void query_input_device();
    void query_output_device();
    void setup_params();
    int n_input_channels = 1;
    int n_output_channels = 2;
    int input_device;
    int output_device;
    PaStreamParameters input_params;
    PaStreamParameters output_params;
    PaStream* pa_stream;
};