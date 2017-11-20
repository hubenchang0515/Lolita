#include "mat.h"

namespace lolita
{

Mat::Mat(uint32_t width, uint32_t height)
{
    this->width_  = width;
    this->height_ = height;
    this->mat.resize(height);
    for(auto& raw : mat)
    {
        raw.resize(width);
    }

}

uint32_t Mat::width()
{
    return this->width_;
}


uint32_t Mat::height()
{
    return this->height_;
}

void Mat::resize(uint32_t width, uint32_t height)
{
    this->width_  = width;
    this->height_ = height;
    this->mat.resize(height);
    for(auto& raw : mat)
    {
        raw.resize(width);
    }
}

void Mat::map(std::function<void(Pixel&)> callback)
{
    for(auto& raw : mat)
    {
        for(Pixel& pix : raw)
        {
            callback(pix); 
        }
    }
}

std::vector<Pixel>& Mat::operator [] (uint32_t raw)
{
    return mat[raw];
}

}; // namespace lolita