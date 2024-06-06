#include <array>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <numeric>
#include <thread>

#include "InputStream.h"
#include "fftw3.h"

constexpr float sensibility = 0.4;
constexpr int sample_rate = 48000;
constexpr unsigned long buffer_size = 512;
constexpr int n_channels = 1;
constexpr float min_frequency = 20.0;
constexpr float max_frequency = 20000.0;

// Data for a 1d real to real fft (r2r_1d), both buffers are doubles
struct FFTData
{
    FFTData();
    ~FFTData();
    double* input;
    double* output;
    fftw_plan plan;
    int start_index;
    int size;
};

FFTData::FFTData()
{
    input = static_cast<double*>(fftw_malloc(buffer_size * sizeof(double)));
    output = static_cast<double*>(fftw_malloc(buffer_size * sizeof(double)));

    constexpr unsigned int plan_flags = FFTW_ESTIMATE;
    constexpr fftw_r2r_kind kind = FFTW_HC2R;
    plan = fftw_plan_r2r_1d(buffer_size, input, output, kind, plan_flags);
}

FFTData::~FFTData()
{
    fftw_free(input);
    fftw_free(output);
    fftw_destroy_plan(plan);
}

static void render_mono_volume_bar(float level, float sensibility);

static int mono_spectrogram(const void* input_buffer,
                            void* output_buffer,
                            unsigned long buffer_size,
                            const PaStreamCallbackTimeInfo* time_info,
                            PaStreamCallbackFlags status_flags,
                            void* user_data)
{
    (void)output_buffer;
    (void)time_info;
    (void)status_flags;
    (void)user_data;

    const float* input = static_cast<const float*>(input_buffer);
    FFTData* data = static_cast<FFTData*>(user_data);

    for (size_t i = 0; i < buffer_size; i++)
        data->input[i] = static_cast<double>(input[i]);

    const float rms = std::accumulate(input, input + buffer_size, 0.0,
                                      [](float aggregate, float current) {
                                          return aggregate + current * current;
                                      });
    std::cout << '\r';
    render_mono_volume_bar(rms, sensibility);
    std::cout.flush();

    return 0;
}

static void render_mono_volume_bar(float level, float sensibility)
{
    constexpr size_t display_length = 100;
    constexpr float threshold_unit = 1 / static_cast<float>(display_length);
    float threshold;

    for (size_t i = 0; i < display_length; i++)
    {
        threshold = i * threshold_unit;
        if (level * sensibility >= threshold)
            std::cout << "█";
        else
            std::cout << " ";
    }
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
