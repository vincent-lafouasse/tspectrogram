#include "ui.h"

#include <cstddef>
#include <iostream>

void render_mono_volume_bar(float volume, float sensibility, size_t line_length)
{
    const float volume_unit = 1 / static_cast<float>(line_length);
    float threshold;

    for (size_t i = 0; i < line_length; i++)
    {
        threshold = i * volume_unit;
        if (volume * sensibility >= threshold)
            std::cout << "█";
        else
            std::cout << " ";
    }
}
