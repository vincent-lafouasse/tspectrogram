#include "ui.h"

#include <cstddef>
#include <iostream>

void render_mono_volume_bar(float level, float sensibility)
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
