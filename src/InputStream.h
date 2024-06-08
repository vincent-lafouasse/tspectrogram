#pragma once

#include "portaudio.h"

typedef struct
{
    int sample_rate;
    unsigned long buffer_size;
    int n_channels;
} InputStreamConfig;

typedef struct
{
    InputStreamConfig cfg;
    int input_device;
    PaStreamParameters input_params;
    PaStream* pa_stream;
} InputStream;

InputStream input_stream_init(InputStreamConfig stream_config);
void input_stream_destroy(InputStream* stream);

void input_stream_start(InputStream* stream);
void input_stream_stop(InputStream* stream);

void input_stream_open(
    InputStream* stream,
    int (*callback)(const void* input_buffer,
                    void* output_buffer,
                    unsigned long buffer_size,
                    const PaStreamCallbackTimeInfo* time_info,
                    PaStreamCallbackFlags status_flags,
                    void* user_data),
    void* data);
