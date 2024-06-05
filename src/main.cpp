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

    err = Pa_Terminate();
    check_error(err);

    return EXIT_SUCCESS;
}

static void check_error(PaError err)
{
    if (err != paNoError)
    {
        std::cout << "Portaudio error: " << Pa_GetErrorText(err);
        exit(EXIT_FAILURE);
    }
}
