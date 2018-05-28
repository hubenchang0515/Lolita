#ifndef LOLITA_PIXEL_H
#define LOLITA_PIXEL_H

#include <stdint.h>

namespace lolita
{

class Pixel
{
public:

    int16_t red;
    int16_t green;
    int16_t blue;
    int16_t alpha; 

    ~Pixel() = default;
    Pixel(const Pixel& other) = default;
    Pixel(Pixel&& other) = default;
    
    Pixel();
    Pixel(uint32_t ABGR);

    operator uint32_t();
    Pixel& operator = (uint32_t ABGR);
    Pixel& operator = (const Pixel& other) = default;
    Pixel& operator = (Pixel&& other) = default;

    static Pixel RGB(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha=0);
};

}; // namespace lolita

#endif