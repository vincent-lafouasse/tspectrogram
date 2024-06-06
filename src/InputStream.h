#pragma once

#include "portaudio.h"

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
                              void* user_data),
              void* data);

    InputStreamConfig cfg;

   private:
    int input_device;
    PaStreamParameters input_params;
    PaStream* pa_stream;
};
