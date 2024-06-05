#include <chrono>
#include <cstdlib>
#include <thread>
#include <iostream>

#include "Stream.h"

void render_volume_bar(float left, float right)
{
    constexpr float display_length = 100;
    constexpr float threshold_unit = 1 / display_length;
    float threshold;

    for (size_t i = 0; i < display_length; i++)
    {
        threshold = i * threshold_unit;
        if (left>= threshold && right>= threshold) 
            std::cout << "█";
        else if (left>= threshold)
            std::cout << "▀";
        else if (right>= threshold)
            std::cout << "▄";
        else
            std::cout << " ";
    }
}

int my_callback(const void* input_buffer,
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

    for (size_t i = 0; i < buffer_size; i++) 
    {
        std::cout << input[i] << " ";
    }

    std::cout.flush();

    return 0;
}

int main(void)
{
    Stream stream;
    stream.open(my_callback);
    stream.start();

    auto capture_duration = std::chrono::milliseconds(3000);
    std::this_thread::sleep_for(capture_duration);

    stream.stop();

    return EXIT_SUCCESS;
}
