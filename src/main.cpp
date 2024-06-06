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

struct FFTData
{
    FFTData();
    std::array<double, buffer_size> input;
    std::array<double, buffer_size> output;
    fftw_plan plan;
    int start_index;
    int size;
};

FFTData::FFTData()
{
    plan = fftw_plan_r2r_1d(buffer_size, input.data(), output.data(), FFTW_HC2R,
                            FFTW_ESTIMATE);
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
