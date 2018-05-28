#include "tools.h"
#include <cmath>
#include <functional>

namespace lolita
{


/**[Private]***********************************************************************************************/
static Pixel convolutionElement(Image& mat, uint32_t row, uint32_t column, Mat<double>& kernel);
static Pixel traverse(Image& mat, uint32_t row, uint32_t column, uint32_t radius, std::function<Pixel(std::vector<Pixel>&)> callback);
static double bicubicCoefficient(double offset);

/******************************************************************************************
 * Name       : grayScale
 * 
 * Input      : mat - source image
 * 
 * Output     : mat - gray-scale image
 * 
 * Return     : void
 * 
 * Function   : Convert a image to gray-scale image
 ******************************************************************************************/
void grayScale(Image& mat)
{
    mat.map([](Pixel& pix)
    {
        pix.red = pix.green = pix.blue = pix.red*0.299 + pix.green*0.587 + pix.blue*0.114;
    });
}



/******************************************************************************************
 * Name       : binaryzation
 * 
 * Input      : mat - source gray-scale image
 * 
 *              threshold - pixel in range of [threshold, 255] will be set as 255
 *                          pixel in range of [0, threshold)  will be set as 0
 *                          if threshold is 0 , this function will calculate a threshold by 
 *                          Kittler Algorithm
 * 
 * Output     : mat - binaryzation image
 * 
 * Return     : void
 * 
 * Function   : Convert a gray-scale image to binaryzation image
 ******************************************************************************************/
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


/******************************************************************************************
 * Name       : convolution
 * 
 * Input      : mat - source image
 * 
 *              kernel - a real matrix 
 * 
 * Output     : mat - convoluted image
 * 
 * Return     : bool
 * 
 * Function   : mat convolute kernel
 ******************************************************************************************/
bool convolution(Image& mat, Mat<double>& kernel)
{
    if( kernel.width() != kernel.height() ||    // not a square
        (kernel.width() & 1) != 1 ||              // length of side is not a odd number
        kernel.width() > mat.width() ||         // kernel is bigger than mat
        kernel.width() > mat.height())
    {
        return false;
    }


    Image backup = mat;

    for(uint32_t x = 0; x < mat.width(); x++)
    {
        for(uint32_t y = 0; y < mat.height(); y++)
        {
            mat[y][x] = convolutionElement(backup, y, x, kernel);
        }
    }

    return true;
}

/******************************************************************************************
 * Name       : detectEdge
 * 
 * Input      : mat - source image
 * 
 * Output     : mat - treated image
 * 
 * Return     : void
 * 
 * Function   : edge detector
 ******************************************************************************************/
void detectEdge(Image& mat)
{
    Mat<double> kernel(3,3);
    kernel.map([](double& element){element = 1;});
    kernel[1][1] = -8;

    convolution(mat, kernel);
}



/******************************************************************************************
 * Name       : averageBlur
 * 
 * Input      : mat - source image
 * 
 *              radius - radius of convolution kernel
 * 
 * Output     : mat - treated image
 * 
 * Return     : void
 * 
 * Function   : blur image by average
 ******************************************************************************************/
void averageBlur(Image& mat, uint32_t radius)
{
    Mat<double> kernel(2*radius + 1, 2*radius + 1);
    kernel.map([radius](double& pix){pix = 1.0/(2*radius + 1)/(2*radius + 1);});
    convolution(mat, kernel);
}



/******************************************************************************************
 * Name       : medianBlur
 * 
 * Input      : mat - source image
 * 
 *              radius - radius of convolution kernel
 * 
 * Output     : mat - treated image
 * 
 * Return     : void
 * 
 * Function   : blur image by median value
 ******************************************************************************************/
void medianBlur(Image& mat, uint32_t radius)
{
    Image backup = mat;
    for(uint32_t y = 0 ; y < mat.height(); y++)
    {
        for(uint32_t x = 0; x < mat.width(); x++)
        {
            mat[y][x] = traverse(backup, y, x, radius,
                            [](std::vector<Pixel>& pixels)->Pixel
                            {
                                std::sort(pixels.begin(), pixels.end());
                                return pixels[pixels.size()/2];
                            });
        }
    }
}



/******************************************************************************************
 * Name       : erode
 * 
 * Input      : mat - source image
 * 
 *              radius - radius of convolution kernel
 * 
 * Output     : mat - treated image
 * 
 * Return     : void
 * 
 * Function   : erode image by minimum value of area
 ******************************************************************************************/
void erode(Image& mat, uint32_t radius)
{
    Image backup = mat;
    for(uint32_t y = 0 ; y < mat.height(); y++)
    {
        for(uint32_t x = 0; x < mat.width(); x++)
        {
            mat[y][x] = traverse(backup, y, x, radius,
                            [](std::vector<Pixel>& pixels)->Pixel
                            {
                                return *std::min_element(pixels.begin(), pixels.end());
                            });
        }
    }
}



/******************************************************************************************
 * Name       : dilate
 * 
 * Input      : mat - source image
 * 
 *              radius - radius of convolution kernel
 * 
 * Output     : mat - treated image
 * 
 * Return     : void
 * 
 * Function   : dilate image by maximum value of area
 ******************************************************************************************/
void dilate(Image& mat, uint32_t radius)
{
    Image backup = mat;
    for(uint32_t y = 0 ; y < mat.height(); y++)
    {
        for(uint32_t x = 0; x < mat.width(); x++)
        {
            mat[y][x] = traverse(backup, y, x, radius,
                            [](std::vector<Pixel>& pixels)->Pixel
                            {
                                return *std::max_element(pixels.begin(), pixels.end());
                            });
        }
    }
}





/******************************************************************************************
 * Name       : gaussian
 * 
 * Input      : mat - empty matrix
 * 
 *              radius - radius of convolution kernel
 * 
 *              variance - sigma of Gaussian distribution
 * 
 * Output     : mat - Gaussian distribution Matrix
 * 
 * Return     : void
 * 
 * Function   : 1 / (2 * pi * variance * variance)  *  exp(- (x * x + y * y) / (2 * variance * variance))
 ******************************************************************************************/
void gaussian(Mat<double>& mat, uint32_t radius, double variance)
{
    double K = 1.0 / 2 / 3.1415926 / variance / variance;
    mat.resize(2*radius + 1, 2*radius + 1);
    for(int64_t y = -(int64_t)radius; y <= radius; y++)
    {
        for(int64_t x = -(int64_t)radius; x <= radius; x++)
        {
            mat[radius + y][radius + x] = K * exp( - ((double)(x)*x + y*y) / (2*variance*variance));
        }
    }

    double sum = mat.reduce<double>([](double& element){return element;});
    mat.map([sum](double& element){element /= sum;});
}



/******************************************************************************************
 * Name       : gaussianBlur
 * 
 * Input      : mat - source image
 * 
 *              radius - radius of convolution kernel
 * 
 *              variance - sigma of Gaussian distribution
 * 
 * Output     : mat - treated image
 * 
 * Return     : void
 * 
 * Function   : blur image by Gaussian distribution
 ******************************************************************************************/
void gaussianBlur(Image& mat, uint32_t radius, double variance)
{
    Mat<double> kernel;
    gaussian(kernel, radius, variance);
    convolution(mat, kernel);
}



/******************************************************************************************
 * Name       : resize
 * 
 * Input      : mat - source image
 * 
 *              width - width of new image
 * 
 *              height - height of new image
 * 
 * Output     : mat - treated image
 * 
 * Return     : void
 * 
 * Function   : resize a image
 ******************************************************************************************/
void resize(Image& mat, uint32_t width, uint32_t height)
{
    Image temp = mat;
    mat.resize(width, height);
    double kx = static_cast<double>(temp.width()) / mat.width();
    double ky = static_cast<double>(temp.height()) / mat.height();

    for(uint32_t y = 0; y < mat.height(); y++)
    {
        for(uint32_t x = 0; x < mat.width(); x++)
        {
            double x_real = x * kx;
            double y_real = y * ky;

            uint32_t x_src = (uint32_t)x_real;
            uint32_t y_src = (uint32_t)y_real;

            double x_offset = x_real - x_src;
            double y_offset = y_real - y_src;

            int x_another = x_src + 1 < temp.width() ? x_src + 1 : x_src - 1;
            int y_another = y_src + 1 < temp.height() ? y_src + 1 : y_src - 1;

            mat[y][x].red =   y_offset * x_offset * temp[y_src][x_src].red 
                            + (1 - y_offset) * x_offset * temp[y_another][x_src].red
                            + y_offset * (1 - x_offset) * temp[y_src][x_another].red
                            + (1 - y_offset) * (1 - x_offset) *temp[y_another][x_another].red;

            mat[y][x].green =   y_offset * x_offset * temp[y_src][x_src].green 
                            + (1 - y_offset) * x_offset * temp[y_another][x_src].green
                            + y_offset * (1 - x_offset) * temp[y_src][x_another].green
                            + (1 - y_offset) * (1 - x_offset) *temp[y_another][x_another].green;

            mat[y][x].blue =   y_offset * x_offset * temp[y_src][x_src].blue 
                            + (1 - y_offset) * x_offset * temp[y_another][x_src].blue
                            + y_offset * (1 - x_offset) * temp[y_src][x_another].blue
                            + (1 - y_offset) * (1 - x_offset) *temp[y_another][x_another].blue;
        }
    }
}



/******************************************************************************************
 * Name       : resize
 * 
 * Input      : mat - source image
 * 
 *              width - width of new image
 * 
 *              height - height of new image
 * 
 * Output     : mat - treated image
 * 
 * Return     : void
 * 
 * Function   : resize a image by bicubic interpolation
 ******************************************************************************************/
void bicubic(Image& mat, uint32_t width, uint32_t height)
{
    Image temp = mat;
    mat.resize(width, height);
    double kx = static_cast<double>(temp.width()) / mat.width();
    double ky = static_cast<double>(temp.height()) / mat.height();

    for(uint32_t y = 0; y < mat.height(); y++)
    {
        for(uint32_t x = 0; x < mat.width(); x++)
        {
            double x_real = x * kx;
            double y_real = y * ky;


            /* find the 16 nearest pixel */
            uint32_t x_begin, x_end;
            uint32_t y_begin, y_end;

            x_begin = x_real - 1 > 0 ? x_real - 1 : 0;
            x_end   = x_real + 3 < temp.width() ? x_real + 3 : temp.width();

            y_begin = y_real - 1 > 0 ? y_real - 1 : 0;
            y_end   = y_real + 3 < temp.height() ? y_real + 3 : temp.height();

            mat[y][x] = 0;
            for(uint32_t i = y_begin; i < y_end; i++)
            {
                for(uint32_t j = x_begin; j < x_end; j++)
                {
                    double y_offset = i > y_real ? i - y_real : y_real - i;
                    double x_offset = j > x_real ? j - x_real : x_real - j;
                    double k = bicubicCoefficient(x_offset) * bicubicCoefficient(y_offset);
                    mat[y][x].red = mat[y][x].red + k * temp[i][j].red;
                    mat[y][x].green = mat[y][x].green + k * temp[i][j].green;
                    mat[y][x].blue = mat[y][x].blue + k * temp[i][j].blue;

                }
            }

            mat[y][x].red = mat[y][x].red < 0 ? 0 : mat[y][x].red > 255 ? 255 : mat[y][x].red;
            mat[y][x].green = mat[y][x].green < 0 ? 0 : mat[y][x].green > 255 ? 255 : mat[y][x].green;
            mat[y][x].blue = mat[y][x].blue < 0 ? 0 : mat[y][x].blue > 255 ? 255 : mat[y][x].blue;
        }
    }
}










/**[Private]***********************************************************************************************/
static Pixel convolutionElement(Image& mat, uint32_t row, uint32_t column, Mat<double>& kernel)
{
    Pixel result = 0;
    uint32_t radius = (kernel.width() - 1 ) / 2;

    uint32_t row_begin = (row > radius) ? (row - radius) : 0;
    uint32_t row_end   = (mat.height() >= radius + row + 1) ? (radius + row + 1) : mat.height();

    uint32_t column_begin = (column > radius) ? (column - radius) : 0;
    uint32_t column_end   = (mat.width() >= radius + column + 1) ? (radius + column + 1) : mat.width();
    
    double red   = 0;
    double green = 0;
    double blue  = 0;

    for(uint32_t y = row_begin; y < row_end; y++)
    {
        for(uint32_t x = column_begin; x < column_end; x++)
        {
            red += (double)(mat[y][x].red) * kernel[radius - row + y][radius - column + x];
            green += (double)(mat[y][x].green) * kernel[radius - row + y][radius - column + x];
            blue += (double)(mat[y][x].blue) * kernel[radius - row + y][radius - column + x];
        }
    }
  
    result.red = red < 0 ? 0 : red > 255 ? 255 : red;
    result.green = green < 0 ? 0 : green > 255 ? 255 : green;
    result.blue = blue < 0 ? 0 : blue > 255 ? 255 : blue;
    //std::cout << red << " " << green << " " << blue << std::endl;

    return result;
}


static Pixel traverse(Image& mat, uint32_t row, uint32_t column, uint32_t radius, std::function<Pixel(std::vector<Pixel>&)> callback)
{
    std::vector<Pixel> temp;
    
    uint32_t row_begin = (row > radius) ? (row - radius) : 0;
    uint32_t row_end   = (mat.height() >= radius + row + 1) ? (radius + row + 1) : mat.height();

    uint32_t column_begin = (column > radius) ? (column - radius) : 0;
    uint32_t column_end   = (mat.width() >= radius + column + 1) ? (radius + column + 1) : mat.width();
    
    for(uint32_t y = row_begin; y < row_end; y++)
    {
        for(uint32_t x = column_begin; x < column_end; x++)
        {
            temp.push_back(mat[y][x]); 
        }
    }

    return callback(temp);
}

static double bicubicCoefficient(double offset)
{
    double a = -0.5;
    if(offset <= 1)
    {
        return (a+2) * offset * offset * offset - (a+3) * offset * offset + 1;
    }
    else if(offset < 2)
    {
        return a * offset * offset * offset - 5*a * offset * offset + 8*a * offset - 4*a;
    }
    else
    {
        return 0;
    }
}

}; // namespace lolita
