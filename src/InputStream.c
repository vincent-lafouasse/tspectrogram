#include "InputStream.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void check_error(PaError err);
static int query_input_device(int n_channels);

int do_nothing_callback(const void* input_buffer,
                        void* output_buffer,
                        unsigned long buffer_size,
                        const PaStreamCallbackTimeInfo* time_info,
                        PaStreamCallbackFlags status_flags,
                        void* user_data)
{
    (void)input_buffer;
    (void)output_buffer;
    (void)buffer_size;
    (void)time_info;
    (void)status_flags;
    (void)user_data;
    return 0;
}

InputStream input_stream_init(InputStreamConfig stream_config)
{
    check_error(Pa_Initialize());
    InputStream stream;
    stream.cfg = stream_config;
    stream.input_device = query_input_device(stream.cfg.n_channels);

    memset(&stream.input_params, 0, sizeof(stream.input_params));
    stream.input_params.channelCount = stream.cfg.n_channels;
    stream.input_params.device = stream.input_device;
    stream.input_params.hostApiSpecificStreamInfo = NULL;
    stream.input_params.sampleFormat = paFloat32;
    stream.input_params.suggestedLatency =
        Pa_GetDeviceInfo(stream.input_device)->defaultLowInputLatency;

    return stream;
}

void input_stream_open(
    InputStream* stream,
    int (*callback)(const void* input_buffer,
                    void* output_buffer,
                    unsigned long buffer_size,
                    const PaStreamCallbackTimeInfo* time_info,
                    PaStreamCallbackFlags status_flags,
                    void* user_data),
    void* data)
{
    check_error(Pa_OpenStream(&stream->pa_stream, &stream->input_params, NULL,
                              stream->cfg.sample_rate, stream->cfg.buffer_size,
                              paNoFlag, callback, data));
}

void input_stream_start(InputStream* stream)
{
    check_error(Pa_StartStream(stream->pa_stream));
}

void input_stream_stop(InputStream* stream)
{
    check_error(Pa_StopStream(stream->pa_stream));
}

void input_stream_destroy(InputStream* stream)
{
    check_error(Pa_CloseStream(stream->pa_stream));
    check_error(Pa_Terminate());
}

static int get_n_devices(void);
static void log_devices(int n_devices);

static int query_input_device(int n_channels)
{
    int n_devices = get_n_devices();
    log_devices(n_devices);

    int device;
    while (1)
    {
        printf("Which input device do you choose?\n");
        scanf("%d", &device);

        if (device >= n_devices || device < 0)
        {
            printf(
                "device not found, please select a device between 0 and %d\n",
                n_devices - 1);
            continue;
        }
        const PaDeviceInfo* info = Pa_GetDeviceInfo(device);
        if (info->maxInputChannels < n_channels)
        {
            printf("this device does not provide enough input channels\n");
            continue;
        }
        break;
    }

    return device;
}

static int get_n_devices(void)
{
    int n_devices = Pa_GetDeviceCount();
    printf("number of devices %d\n", n_devices);
    if (n_devices < 0)
    {
        printf("Error getting device count\n");
        exit(EXIT_FAILURE);
    }
    else if (n_devices == 0)
    {
        printf("No audio device found\n");
        exit(EXIT_SUCCESS);
    }
    return n_devices;
}

static void log_devices(int n_devices)
{
    for (int i = 0; i < n_devices; i++)
    {
        const PaDeviceInfo* device_info = Pa_GetDeviceInfo(i);
        printf("Device %d\n", i);
        printf("\tname %s\n", device_info->name);
        printf("\tmax input channels %d\n", device_info->maxInputChannels);
        printf("\tmax output channels %d\n", device_info->maxOutputChannels);
        printf("\tdefault sample rate %f\n", device_info->defaultSampleRate);
    }
}

static void check_error(PaError err)
{
    if (err != paNoError)
    {
        printf("Portaudio error: %s\n", Pa_GetErrorText(err));
        exit(EXIT_FAILURE);
    }
}
