#ifndef LOLITA_MAT_H
#define LOLITA_MAT_H

#include "pixel.h"
#include <stddef.h>
#include <vector>
#include <functional>

namespace lolita
{

class Mat
{
public:
    ~Mat() = default;
    Mat(const Mat&) = default;
    Mat(Mat&&) = default;

    Mat(uint32_t width = 0, uint32_t height = 0);

    uint32_t width();
    uint32_t height();

    std::vector<Pixel>& operator [] (uint32_t raw);

    void resize(uint32_t width, uint32_t height);

    void map(std::function<void(Pixel&)> callback);
    
    template<typename T>
    T reduce(std::function<T(Pixel&)> callback)
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
    std::vector< std::vector<Pixel> > mat;
    uint32_t width_;
    uint32_t height_;
};

}; // namespace lolita

#endif