#include <iostream>
#include <cstdio>
#include <cassert>
#include <lolita.hpp>

using namespace lolita;

#define ASSERT(EXPR)                                                                                        \
do                                                                                                          \
{                                                                                                           \
    if(EXPR)                                                                                                \
    {                                                                                                       \
        std::cout << "<test> " << __FILE__ << ":" << __LINE__  << " pass { " << #EXPR << " }" << std::endl; \
    }                                                                                                       \
    else                                                                                                    \
    {                                                                                                       \
        std::cout << "<test> "  << __FILE__ << ":" << __LINE__ << " fail { " << #EXPR << " }" << std::endl; \
    }                                                                                                       \
    assert(EXPR);                                                                                           \
}while(0)

void testColorSpace()
{
    uint64_t rgb = ColorSpace::ID("RGB");
    uint64_t rgba = ColorSpace::ID("RGBA");
    uint64_t rgbx = ColorSpace::ID("RGBX");
    
    ASSERT(ColorSpace::CS_LENGTH(rgb) == 3);
    ASSERT(ColorSpace::CS_LENGTH(rgba) == 4);
    ASSERT(ColorSpace::CS_LENGTH(rgbx) == 4);

    ASSERT(ColorSpace::CS_INDEX(rgb, 'R') == 0);
    ASSERT(ColorSpace::CS_INDEX(rgb, 'G') == 1);
    ASSERT(ColorSpace::CS_INDEX(rgb, 'B') == 2);

    ASSERT(ColorSpace::CS_INDEX(rgba, 'R') == 0);
    ASSERT(ColorSpace::CS_INDEX(rgba, 'G') == 1);
    ASSERT(ColorSpace::CS_INDEX(rgba, 'B') == 2);
    ASSERT(ColorSpace::CS_INDEX(rgba, 'A') == 3);

    ASSERT(ColorSpace::CS_INDEX(rgbx, 'R') == 0);
    ASSERT(ColorSpace::CS_INDEX(rgbx, 'G') == 1);
    ASSERT(ColorSpace::CS_INDEX(rgbx, 'B') == 2);
    ASSERT(ColorSpace::CS_INDEX(rgbx, 'X') == 3);
    ASSERT(ColorSpace::CS_INDEX(rgbx, 'A') == SIZE_MAX);
}

void testPixel()
{
    ASSERT(sizeof(Pixel::RGB24) == 3);
    ASSERT(sizeof(Pixel::BGR24) == 3);
    ASSERT(sizeof(Pixel::RGBX32) == 4);
    ASSERT(sizeof(Pixel::XRGB32) == 4);
    ASSERT(sizeof(Pixel::BGRX32) == 4);
    ASSERT(sizeof(Pixel::XBGR32) == 4);
    ASSERT(sizeof(Pixel::RGBA32) == 4);
    ASSERT(sizeof(Pixel::ARGB32) == 4);
    ASSERT(sizeof(Pixel::BGRA32) == 4);
    ASSERT(sizeof(Pixel::ABGR32) == 4);

    Pixel::RGBA32 p1;
    p1.setRed(95);
    p1.setGreen(05);
    p1.setBlue(15);

    Pixel::BGR24 p2;
    PixelConvert::RGB(p2, p1);
    Pixel::GrayScale p3;
    PixelConvert::GRAY(p3, p2);
    ASSERT(p3.grayScale() == (95*299 + 05*587 + 15*114 + 500)/1000);
}

void testUtils()
{
    using namespace lolita::utils;
    ASSERT(padding(0, 4) == 0);
    ASSERT(padding(1, 4) == 3);
    ASSERT(padding(2, 4) == 2);
    ASSERT(padding(3, 4) == 1);
    ASSERT(padding(4, 4) == 0);
}

void testMat()
{
    Mat<Pixel::RGB24> image1(600, 480);
    ASSERT(image1.size() == 3 * 600 * 480);

    Mat<Pixel::BGR24> image2(600, 480, utils::padding(600 * sizeof(Pixel::BGR24), 4));
    ASSERT(image2.size() == 3 * 600 * 480);

    Mat<Pixel::BGR24> image3(1857, 1393, utils::padding(1857 * sizeof(Pixel::BGR24), 4));
    ASSERT(image3.size() == (3 * 1857 + 1) * 1393);
}

void testBMP()
{
    ASSERT(sizeof(BMP::FileHeader) == 14);
    ASSERT(sizeof(BMP::InfoHeader) == 40);
    ASSERT(sizeof(BMP::RGBPalette) == 4);

    Mat<Pixel::BGR24> source = BMP::read("input.bmp");
    BMP::write(source, "out.bmp");
    Mat<Pixel::RGB24> out1;
    Mat<Pixel::BGR24> out2;
    Mat<Pixel::GRB24> out3;
    Mat<Pixel::GBR24> out4;
    Mat<Pixel::BRG24> out5;
    Mat<Pixel::GrayScale> out6;
    MatConvert::RGB(out1, source, true);
    MatConvert::RGB(out2, source, true);
    MatConvert::RGB(out3, source, true);
    MatConvert::RGB(out4, source, true);
    MatConvert::RGB(out5, source, true);
    MatConvert::GRAY(out6, source, true);
    BMP::write(out1, "out1.bmp");
    BMP::write(out2, "out2.bmp");
    BMP::write(out3, "out3.bmp");
    BMP::write(out4, "out4.bmp");
    BMP::write(out5, "out5.bmp");
    BMP::write(out6, "out6.bmp", 8);
}

int main()
{
    testColorSpace();
    testPixel();
    testUtils();
    testMat();
    testBMP();

    return 0;
}