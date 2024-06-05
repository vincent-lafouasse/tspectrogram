#include <portaudio.h>
#include <cstdlib>
#include <iostream>

static void check_error(PaError err);

int main(void)
{
    PaError err;
    err = Pa_Initialize();
    check_error(err);

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

    err = Pa_Terminate();
    check_error(err);

    return EXIT_SUCCESS;
}

static void check_error(PaError err)
{
    if (err != paNoError)
    {
        std::cout << "Portaudio error: " << Pa_GetErrorText(err) << '\n';
        exit(EXIT_FAILURE);
    }
}
