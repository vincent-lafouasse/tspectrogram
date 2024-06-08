#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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
    data.plan = fftw_plan_r2r_1d(BUFFER_SIZE, data.dtft_input, data.dtft_output,
                                 kind, plan_flags);

    const double sample_ratio = BUFFER_SIZE / SAMPLE_RATE;
    const int end_index =
        min_int(BUFFER_SIZE, ceil(MAX_FREQUENCY * sample_ratio));
    data.start_index = ceil(MIN_FREQUENCY * sample_ratio);
    data.spectrogram_size = end_index - data.start_index;
}

void fft_data_destroy(FFTData* data)
{
    fftw_free(data->dtft_input);
    fftw_free(data->dtft_output);
    fftw_destroy_plan(data->plan);
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

    const float* input = (const float*)input_buffer;
    FFTData* data = (FFTData*)user_data;

    for (size_t i = 0; i < buffer_size; i++)
        data->dtft_input[i] = input[i];

    // computes dtft and fills dtft_output
    fftw_execute(data->plan);

    printf("\r");
    const size_t line_length = 100;
    double log_index;
    size_t index;
    double amplitude;
    for (size_t i = 0; i < line_length; i++)
    {
        log_index = pow(i / (double)line_length, 4);
        index =
            (size_t)(data->start_index + log_index * data->spectrogram_size);
        amplitude = SENSIBILITY * data->dtft_output[index];

        if (amplitude < 0.125)
            printf("▁");
        else if (amplitude < 0.25)
            printf("▂");
        else if (amplitude < 0.375)
            printf("▃");
        else if (amplitude < 0.5)
            printf("▄");
        else if (amplitude < 0.625)
            printf("▅");
        else if (amplitude < 0.75)
            printf("▆");
        else if (amplitude < 0.875)
            printf("▇");
        else
            printf("█");
    }
    fflush(stdout);

    return 0;
}

int main(void)
{
    const InputStreamConfig cfg = {SAMPLE_RATE, BUFFER_SIZE, N_CHANNELS};

    FFTData callback_data = fft_data_init();

    InputStream stream = input_stream_init(cfg);
    input_stream_open(&stream, &mono_spectrogram, &callback_data);
    input_stream_start(&stream);

    int capture_duration_secs = 10;
    sleep(capture_duration_secs);

    input_stream_stop(&stream);

    fft_data_destroy(&callback_data);
    input_stream_destroy(&stream);

    return EXIT_SUCCESS;
}
