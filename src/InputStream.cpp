#include "InputStream.h"

#include <cstdlib>
#include <iostream>

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
    std::cout << 'a';
    std::cout.flush();
    return 0;
}

InputStream::InputStream()
{
    check_error(Pa_Initialize());
    cfg = InputStreamConfig::default_config();
    input_device = query_input_device(cfg.n_channels);

    std::memset(&input_params, 0, sizeof(input_params));
    input_params.channelCount = cfg.n_channels;
    input_params.device = input_device;
    input_params.hostApiSpecificStreamInfo = NULL;
    input_params.sampleFormat = paFloat32;
    input_params.suggestedLatency =
        Pa_GetDeviceInfo(input_device)->defaultLowInputLatency;
}

InputStream::InputStream(InputStreamConfig stream_config)
{
    check_error(Pa_Initialize());
    cfg = stream_config;
    input_device = query_input_device(cfg.n_channels);

    std::memset(&input_params, 0, sizeof(input_params));
    input_params.channelCount = cfg.n_channels;
    input_params.device = input_device;
    input_params.hostApiSpecificStreamInfo = NULL;
    input_params.sampleFormat = paFloat32;
    input_params.suggestedLatency =
        Pa_GetDeviceInfo(input_device)->defaultLowInputLatency;
}

void InputStream::open(
    int (*callback)(const void* input_buffer,
                    void* output_buffer,
                    unsigned long buffer_size,
                    const PaStreamCallbackTimeInfo* time_info,
                    PaStreamCallbackFlags status_flags,
                    void* user_data))
{
    check_error(Pa_OpenStream(&pa_stream, &input_params, NULL, cfg.sample_rate,
                              cfg.buffer_size, paNoFlag, callback, NULL));
}

void InputStream::start()
{
    check_error(Pa_StartStream(pa_stream));
}

void InputStream::stop()
{
    check_error(Pa_StopStream(pa_stream));
}

InputStream::~InputStream()
{
    check_error(Pa_CloseStream(pa_stream));
    check_error(Pa_Terminate());
}

static int get_n_devices();
static void log_devices(int n_devices);

static int query_input_device(int n_channels)
{
    int n_devices = get_n_devices();
    log_devices(n_devices);

    int device;
    while (1)
    {
        std::cout << "Which input device do you choose?\n";
        std::cin >> device;

        if (device >= n_devices || device < 0)
        {
            std::cout
                << "device not found, please select a device between 0 and "
                << n_devices - 1 << '\n';
            continue;
        }
        const PaDeviceInfo* info = Pa_GetDeviceInfo(device);
        if (info->maxInputChannels < n_channels)
        {
            std::cout << "this device does not provide enough input channels\n";
            continue;
        }
        break;
    }

    return device;
}

static int get_n_devices()
{
    int n_devices = Pa_GetDeviceCount();
    std::cout << "number of devices: " << n_devices << '\n';
    if (n_devices < 0)
    {
        std::cout << "Error getting device count\n";
        exit(EXIT_FAILURE);
    }
    else if (n_devices == 0)
    {
        std::cout << "No audio device found\n";
        exit(EXIT_SUCCESS);
    }
    return n_devices;
}

static void log_devices(int n_devices)
{
    for (int i = 0; i < n_devices; i++)
    {
        const PaDeviceInfo* device_info = Pa_GetDeviceInfo(i);
        std::cout << "Device " << i << '\n';
        std::cout << "\tname " << device_info->name << '\n';
        std::cout << "\tmax input channels " << device_info->maxInputChannels
                  << '\n';
        std::cout << "\tmax output channels " << device_info->maxOutputChannels
                  << '\n';
        std::cout << "\tdefault sample rate " << device_info->defaultSampleRate
                  << "\n\n";
    }
}

InputStreamConfig InputStreamConfig::default_config()
{
    constexpr int default_sample_rate = 44100;
    constexpr unsigned long default_buffer_size = 512;

    InputStreamConfig cfg;
    cfg.sample_rate = default_sample_rate;
    cfg.buffer_size = default_buffer_size;
    return cfg;
}

static void check_error(PaError err)
{
    if (err != paNoError)
    {
        std::cout << "Portaudio error: " << Pa_GetErrorText(err) << '\n';
        exit(EXIT_FAILURE);
    }
}
