#include "tools.h"

namespace lolita
{

void grayScale(Image& mat)
{
    mat.map([](Pixel& pix)
    {
        pix.red = pix.green = pix.blue = pix.red*0.299 + pix.green*0.587 + pix.blue*0.114;
    });
}

void binaryzation(Image& mat, uint8_t threshold)
{
    if(threshold == 0)
    {
        // Kittler
        uintmax_t sumGrads = 0;
        uintmax_t sumGrayGrads = 0;
        for(uint32_t i = 1; i < mat.height()-1; i++)
        {
            for(uint32_t j = 1; j < mat.width()-1; j++)
            {
                uint8_t grads = std::max(std::abs(mat[i][j-1].red - mat[i][j+1].red) , std::abs(mat[i-1][j].red - mat[i+1][j].red));
                sumGrads += grads;
                sumGrayGrads += grads * mat[i][j].red;
            }
        }

        threshold = sumGrayGrads / sumGrads;
    }

    mat.map([threshold](Pixel& pix)
    {
        pix.red = pix.green = pix.blue = (pix.red >= threshold ? 0xff : 0);
    });
}

}; // namespace lolita