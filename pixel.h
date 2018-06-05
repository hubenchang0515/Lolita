#ifndef LOLITA_PIXEL_H
#define LOLITA_PIXEL_H

#include <stdint.h>

namespace lolita
{

class RgbPixel
{
public:

    int16_t red;
    int16_t green;
    int16_t blue;
    int16_t alpha; 

    ~RgbPixel() = default;
    RgbPixel(const RgbPixel& other) = default;
    RgbPixel(RgbPixel&& other) = default;
    
    RgbPixel();
    RgbPixel(uint32_t ABGR);

    operator uint32_t();
    RgbPixel& operator = (uint32_t ABGR);
    RgbPixel& operator = (const RgbPixel& other) = default;
    RgbPixel& operator = (RgbPixel&& other) = default;

    static RgbPixel RGB(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha=0);
};


class HsvPixel
{
public:

        int16_t hue;
        int16_t saturation;
        int16_t value;
};

HsvPixel rgb2hsv(RgbPixel color);
RgbPixel hsv2rgb(HsvPixel color);
uintmax_t distance(HsvPixel p1, HsvPixel p2);

template<typename T>
T maximum(T arg1, T arg2)
{
    if(arg1 >= arg2)
    {
        return arg1;
    }
    else
    {
        return arg2;
    }
}

template<typename T, typename... TP>
T maximum(T first, T second, TP... args)
{
    return maximum(maximum(first, second), args...);
}


template<typename T>
T minimum(T arg1, T arg2)
{
    if(arg1 <= arg2)
    {
        return arg1;
    }
    else
    {
        return arg2;
    }
}

template<typename T, typename... TP>
T minimum(T first, T second, TP... args)
{
    return minimum(minimum(first, second), args...);
}


}; // namespace lolita

#endif