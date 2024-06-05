#include <chrono>
#include <cstdlib>
#include <thread>

#include "Stream.h"

int main(void)
{
    Stream stream;
    stream.start();

    auto capture_duration = std::chrono::milliseconds(5000);
    std::this_thread::sleep_for(capture_duration);

    stream.stop();

    return EXIT_SUCCESS;
}
