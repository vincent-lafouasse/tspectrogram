#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include "InputStream.h"
#include "fftw3.h"

#define SAMPLE_RATE 48000.0
#define BUFFER_SIZE 512
#define N_CHANNELS 1

#define MIN_FREQUENCY 20.0
#define MAX_FREQUENCY 20000.0
#define SENSIBILITY 0.4

// Data for a 1d real to real fft (r2r_1d) hence two double buffers
typedef struct
{
    double* dtft_input;
    double* dtft_output;
    fftw_plan plan;
    int start_index;  // represents 20 Hz, ie ignore information below 20 Hz
    int spectrogram_size;  // number of frequency buckets to fetch
} FFTData;

static int min_int(int a, int b)
{
    return a > b ? b : a;
}

FFTData fft_data_init(void)
{
    FFTData data;
    data.dtft_input = (double*)fftw_malloc(BUFFER_SIZE * sizeof(double));
    data.dtft_output = (double*)fftw_malloc(BUFFER_SIZE * sizeof(double));

    const unsigned int plan_flags = FFTW_ESTIMATE;
    const fftw_r2r_kind kind = FFTW_HC2R;
    data.plan = fftw_plan_r2r_1d(BUFFER_SIZE, data.dtft_input, data.dtft_output, kind,
                            plan_flags);

    const double sample_ratio = BUFFER_SIZE / SAMPLE_RATE;
    const int end_index =
        min_int(BUFFER_SIZE, ceil(MAX_FREQUENCY * sample_ratio));
    data.start_index = ceil(MIN_FREQUENCY * sample_ratio);
    data.spectrogram_size = end_index - data.start_index;
}

void fft_data_destroy(FFTData* data)
{
    fftw_free(data.dtft_input);
    fftw_free(data.dtft_output);
    fftw_destroy_plan(data.plan);
}

static int mono_spectrogram(const void* input_buffer,
                            void* _output_buffer,
                            unsigned long buffer_size,
                            const PaStreamCallbackTimeInfo* _time_info,
                            PaStreamCallbackFlags _status_flags,
                            void* user_data)
{
    (void)_output_buffer;
    (void)_time_info;
    (void)_status_flags;

    const float* input = static_cast<const float*>(input_buffer);
    FFTData* data = static_cast<FFTData*>(user_data);

    for (size_t i = 0; i < buffer_size; i++)
        data->dtft_input[i] = static_cast<double>(input[i]);

    // computes dtft and fills dtft_output
    fftw_execute(data->plan);

    std::cout << '\r';
    constexpr size_t line_length = 100;
    double log_index;
    size_t index;
    double amplitude;
    for (size_t i = 0; i < line_length; i++)
    {
        log_index = std::pow(i / static_cast<float>(line_length), 4);
        index = static_cast<size_t>(data->start_index +
                                    log_index * data->spectrogram_size);
        amplitude = sensibility * data->dtft_output[index];

        if (amplitude < 0.125)
            std::cout << "▁";
        else if (amplitude < 0.25)
            std::cout << "▂";
        else if (amplitude < 0.375)
            std::cout << "▃";
        else if (amplitude < 0.5)
            std::cout << "▄";
        else if (amplitude < 0.625)
            std::cout << "▅";
        else if (amplitude < 0.75)
            std::cout << "▆";
        else if (amplitude < 0.875)
            std::cout << "▇";
        else
            std::cout << "█";
    }

    std::cout.flush();

    return 0;
}

int main(void)
{
    constexpr InputStreamConfig cfg = {sample_rate, buffer_size, n_channels};

    FFTData callback_data;

    InputStream stream(cfg);
    stream.open(mono_spectrogram, &callback_data);
    stream.start();

    auto capture_duration = std::chrono::seconds(10);
    std::this_thread::sleep_for(capture_duration);

    stream.stop();

    return EXIT_SUCCESS;
}
