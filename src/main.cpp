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
    (void)input_buffer;
    (void)output_buffer;
    (void)buffer_size;
    (void)time_info;
    (void)status_flags;
    (void)user_data;
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
