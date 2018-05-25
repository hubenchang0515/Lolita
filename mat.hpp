#ifndef LOLITA_MAT_H
#define LOLITA_MAT_H

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <functional>
#include <iostream>

namespace lolita
{

template<typename ElemType>
class Mat
{
public:
    ~Mat() = default;
    Mat(const Mat&) = default;
    Mat(Mat&&) = default;

    Mat(uint32_t width = 0, uint32_t height = 0);

    uint32_t width();
    uint32_t height();

    std::vector<ElemType>& operator [] (uint32_t raw)
    {
    	return mat[raw];
    }

    void resize(uint32_t width, uint32_t height);

    void map(std::function<void(ElemType&)> callback);
    
    template<typename T>
    T reduce(std::function<T(ElemType&)> callback)
    {
        T result = callback(mat[0][0]);
        for(uint32_t i = 1; i < width_; i++)
        {
            result += callback(mat[0][i]);
        }

        for(uint32_t raw = 1; raw < height_; raw++)
        {
            for(uint32_t i = 0; i < width_; i++)
            {
                result += callback(mat[raw][i]);
            }
        }
        
        return result;
    }

private:
    std::vector< std::vector<ElemType> > mat;
    uint32_t width_;
    uint32_t height_;
};

template<typename ElemType>
Mat<ElemType>::Mat(uint32_t width, uint32_t height)
{
    this->width_  = width;
    this->height_ = height;
    this->mat.resize(height);
    for(auto& raw : mat)
    {
        raw.resize(width);
    }

}

template<typename ElemType>
uint32_t Mat<ElemType>::width()
{
    return this->width_;
}

template<typename ElemType>
uint32_t Mat<ElemType>::height()
{
    return this->height_;
}

template<typename ElemType>
void Mat<ElemType>::resize(uint32_t width, uint32_t height)
{
    this->width_  = width;
    this->height_ = height;
    this->mat.resize(height);
    for(auto& raw : mat)
    {
        raw.resize(width);
    }
}

template<typename ElemType>
void Mat<ElemType>::map(std::function<void(ElemType&)> callback)
{
    for(auto& raw : mat)
    {
        for(ElemType& pix : raw)
        {
            callback(pix); 
        }
    }
}

/************************************************************************************************/


}; // namespace lolita


#include "pixel.h"
namespace lolita
{

    using Image = Mat<Pixel>;

}; // namespace lolita



#endif
