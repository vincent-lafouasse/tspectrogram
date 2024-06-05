#include <portaudio.h>
#include <cstdlib>
#include <cstring>
#include <iostream>

static int get_n_devices();
static int query_user_for_device();
static void log_devices(int n_devices);
static void check_error(PaError err);

int main(void)
{
    PaError err;
    err = Pa_Initialize();
    check_error(err);

    int device = query_user_for_device();
    std::cout << "you chose " << device << "\n";

    PaStreamParameters input_params;
    PaStreamParameters output_params;
    std::memset(&input_params, 0, sizeof(input_params));
    std::memset(&output_params, 0, sizeof(output_params));

    input_params.channelCount = 2;
    input_params.device = device;
    input_params.hostApiSpecificStreamInfo = NULL;
    input_params.sampleFormat = paFloat32;
    input_params.suggestedLatency =
        Pa_GetDeviceInfo(device)->defaultLowInputLatency;

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
