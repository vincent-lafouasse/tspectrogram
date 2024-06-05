#include <chrono>
#include <cstdlib>
#include <thread>

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

    for (size_t i = 0; i < buffer_size; i += 2)
    {
        left_volume = std::max(left_volume, input[i]);
    }
    for (size_t i = 1; i < buffer_size; i += 2)
    {
        right_volume = std::max(right_volume, input[i]);
    }
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
