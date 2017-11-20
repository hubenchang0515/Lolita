#ifndef LOLITA_PIXEL_H
#define LOLITA_PIXEL_H

#include <stdint.h>

namespace lolita
{

class Pixel
{
public:

    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t alpha;  // reserved

    ~Pixel() = default;
    Pixel(const Pixel&) = default;
    Pixel(Pixel&&) = default;
    
    Pixel();
    Pixel(uint32_t rgb);

    operator uint32_t();
    Pixel& operator = (uint32_t rgb);
    Pixel& operator = (const Pixel& pix) = default;
    Pixel& operator = (Pixel&& pix) = default;

    static Pixel rgb(uint8_t r, uint8_t g, uint8_t b);
};

}; // namespace lolita

#endif