#include <chrono>
#include <cstdlib>
#include <thread>
#include <iostream>

#include "Stream.h"

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

    float left_volume = 0;
    float right_volume = 0;

    for (size_t i = 0; i < 2 * buffer_size; i += 2)
    {
        left_volume = std::max(left_volume, std::abs(input[i]));
        right_volume = std::max(right_volume, std::abs(input[i + 1]));
    }
    std::cout << '\r';

    std::cout << "l\t" << left_volume << "\tr\t" << right_volume;

    /*
    constexpr float display_length = 100;
    constexpr float threshold_unit = 1 / display_length;
    float threshold;

    for (size_t i = 0; i < display_length; i++)
    {
        threshold = i * threshold_unit;
        if (left_volume >= threshold && right_volume >= threshold) 
            std::cout << "█";
        else if (left_volume >= threshold)
            std::cout << "▀";
        else if (right_volume >= threshold)
            std::cout << "▄";
        else
            std::cout << " ";
    }
    */
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
