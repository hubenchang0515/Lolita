#ifndef LOLITA_MAT_H
#define LOLITA_MAT_H

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <functional>
#include <iostream>
#include <cstdlib>
#include <cstring>

namespace lolita
{

template<typename ElemType>
class MatRowView
{
public:
    ~MatRowView() = default;
    MatRowView(const MatRowView&) = default;
    MatRowView(MatRowView&&) = default;

    explicit MatRowView(ElemType* data):
        data_(data)
    {

    }

    ElemType& operator [] (uint32_t index)
    {
        return *(data_ + index);
    }

    const ElemType& operator [] (uint32_t index) const
    {
        return *(data_ + index);
    }

private:
    ElemType* data_;
};

template<typename ElemType>
class Mat
{
public:
    ~Mat()
    {
        if(data_ != nullptr)
        {
            free(data_);
            data_ = nullptr;
        }
    }

    Mat(uint32_t width = 0, uint32_t height = 0)
    {
        this->width_  = width;
        this->height_ = height;
        void* data = malloc(sizeof(ElemType) * width * height);
        this->data_ = reinterpret_cast<ElemType*>(data);
    }

    Mat(const Mat& another)
    {
        size_t n = sizeof(ElemType) * another.width_ * another.height_;
        void* data = malloc(n);
        if(data == nullptr)
        {
            throw std::bad_alloc();
        }
        memcpy(data, another.data_, n);
        this->data_ = reinterpret_cast<ElemType*>(data);
        this->width_ = another.width_;
        this->height_ = another.height_;
    }

    Mat(Mat&& another)
    {
        data_ = another.data_;
        this->width_ = another.width_;
        this->height_ = another.height_;
        another.data_ = nullptr;
    }

    uint32_t width() const
    {
        return width_;
    }

    uint32_t height() const
    {
        return height_;
    }

    MatRowView<ElemType> operator [] (uint32_t raw)
    {
    	return MatRowView<ElemType>(data_ + raw * width_);
    }

    const MatRowView<ElemType> operator [] (uint32_t raw) const
    {
    	return MatRowView<ElemType>(data_ + raw * width_);
    }

    void resize(uint32_t width, uint32_t height)
    {
        this->width_  = width;
        this->height_ = height;
        void* data = reinterpret_cast<void*>(data_);
        data = realloc(data, sizeof(ElemType) * width * height);
        this->data_ = reinterpret_cast<ElemType*>(data);
    }

    void map(std::function<void(ElemType&)> callback)
    {
        for(uint32_t i = 0; i < width_ * height_; i++)
        {
            callback(*(data_ + i)); 
        }
    }
    
    template<typename T>
    T reduce(std::function<T(ElemType&)> callback)
    {
        T n = 0;
        for(uint32_t i = 0; i < width_ * height_; i++)
        {
            n += callback(*(data_ + i)); 
        }
        return n;
    }

private:
    ElemType* data_;
    uint32_t width_;
    uint32_t height_;
};

/************************************************************************************************/


}; // namespace lolita


#include "pixel.h"
namespace lolita
{
    using RgbImage = Mat<RgbPixel>;
    using HsvImage = Mat<HsvPixel>;
    using Image = RgbImage;

    inline void convertRgb2Hsv(const RgbImage& src, HsvImage& dst)
    {
        dst.resize(src.width(), src.height());
        for(uint32_t y = 0; y < src.height(); y++)
        {
            for(uint32_t x = 0; x < src.width(); x++)
            {
                dst[y][x] = rgb2hsv(src[y][x]);
            }
        }
    }


    inline void convertHsv2Rgb(const HsvImage& src, RgbImage& dst)
    {
        dst.resize(src.width(), src.height());
        for(uint32_t y = 0; y < src.height(); y++)
        {
            for(uint32_t x = 0; x < src.width(); x++)
            {
                dst[y][x] = hsv2rgb(src[y][x]);
            }
        }
    }
}; // namespace lolita



#endif
