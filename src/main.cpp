#include <portaudio.h>
#include <cstdlib>
#include <cstring>
#include <iostream>

static int get_n_devices();
static int query_user_for_device();
static void log_devices(int n_devices);
static void check_error(PaError err);

constexpr int default_sample_rate = 44100;
constexpr unsigned long default_buffer_size = 512;

struct StreamConfig
{
    static StreamConfig default_config()
    {
        StreamConfig cfg;
        cfg.sample_rate = default_sample_rate;
        cfg.buffer_size = default_buffer_size;
        return cfg;
    }
    int sample_rate;
    unsigned long buffer_size;
};

class Stream
{
   public:
    int start();
    int pause();
    int close();

    static int callback(const void* input_buffer,
                        void* output_buffer,
                        unsigned long buffer_size,
                        const PaStreamCallbackTimeInfo* time_info,
                        PaStreamCallbackFlags status_flags,
                        void* user_data)
    {
        return 0;
    }

    StreamConfig cfg;

   private:
    void query_input_device();
    void query_output_device();
    int n_input_channels;
    int n_output_channels;
    int input_device;
    int output_device;
    PaStreamParameters input_params;
    PaStreamParameters output_params;
    PaStream* pa_stream;
};

int main(void)
{
    PaError err;
    err = Pa_Initialize();
    check_error(err);

    int device = query_user_for_device();
    std::cout << "you chose " << device << "\n";

    StreamConfig stream_config = StreamConfig::default_config();

    PaStreamParameters input_params;
    PaStreamParameters output_params;
    std::memset(&input_params, 0, sizeof(input_params));
    std::memset(&output_params, 0, sizeof(output_params));

    input_params.channelCount = 1;
    input_params.device = device;
    input_params.hostApiSpecificStreamInfo = NULL;
    input_params.sampleFormat = paFloat32;
    input_params.suggestedLatency =
        Pa_GetDeviceInfo(device)->defaultLowInputLatency;

    output_params.channelCount = 1;
    output_params.device = device;
    output_params.hostApiSpecificStreamInfo = NULL;
    output_params.sampleFormat = paFloat32;
    output_params.suggestedLatency =
        Pa_GetDeviceInfo(device)->defaultLowOutputLatency;

    Stream stream;
    stream.cfg = stream_config;

    PaStream* pa_stream;
    err = Pa_OpenStream(&pa_stream, &input_params, &output_params,
                        stream.cfg.sample_rate, stream.cfg.buffer_size,
                        paNoFlag, stream.callback, NULL);
    check_error(err);

    err = Pa_StartStream(pa_stream);
    check_error(err);

    int capture_time_secs = 5;
    Pa_Sleep(capture_time_secs * 1000);

    err = Pa_StopStream(pa_stream);
    check_error(err);

    err = Pa_CloseStream(pa_stream);
    check_error(err);
    err = Pa_Terminate();
    check_error(err);

    return EXIT_SUCCESS;
}

static int query_user_for_device()
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
        if (info->maxInputChannels < 1)
        {
            std::cout << "this device does not provide input channels\n";
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

static void check_error(PaError err)
{
    if (err != paNoError)
    {
        std::cout << "Portaudio error: " << Pa_GetErrorText(err) << '\n';
        exit(EXIT_FAILURE);
    }
}
