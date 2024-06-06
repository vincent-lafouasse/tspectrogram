#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <numeric>
#include <thread>

#include "InputStream.h"
#include "fftw3.h"

constexpr int sample_rate = 48000;
constexpr unsigned long buffer_size = 512;
constexpr int n_channels = 1;

constexpr float min_frequency = 20.0;
constexpr float max_frequency = 20000.0;
constexpr float sensibility = 0.4;

// Data for a 1d real to real fft (r2r_1d) hence two double buffers
struct FFTData
{
    FFTData();
    ~FFTData();
    double* dtft_input;
    double* dtft_output;
    fftw_plan plan;
    int start_index;  // represents 20 Hz, ie ignore information below 20 Hz
    int spectrogram_size;  // number of frequency buckets to fetch
};

FFTData::FFTData()
{
    dtft_input =
        static_cast<double*>(fftw_malloc(buffer_size * sizeof(double)));
    dtft_output =
        static_cast<double*>(fftw_malloc(buffer_size * sizeof(double)));

    constexpr unsigned int plan_flags = FFTW_ESTIMATE;
    constexpr fftw_r2r_kind kind = FFTW_HC2R;
    plan = fftw_plan_r2r_1d(buffer_size, dtft_input, dtft_output, kind,
                            plan_flags);

    constexpr double sample_ratio =
        buffer_size / static_cast<double>(sample_rate);
    start_index = std::ceil(min_frequency * sample_ratio);
    const int end_index =
        std::min(static_cast<int>(buffer_size),
                 static_cast<int>(std::ceil(max_frequency * sample_ratio)));
    spectrogram_size = end_index - start_index;
}

FFTData::~FFTData()
{
    fftw_free(dtft_input);
    fftw_free(dtft_output);
    fftw_destroy_plan(plan);
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
