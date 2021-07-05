#include <iostream>
#include <cstdio>
#include <stdexcept>
#include <lolita.hpp>

using namespace lolita;

#define TOSTR(X) #X
#define STRING(X) TOSTR(X)
#define POSITION __FILE__ ":" STRING(__LINE__)
#define MASSERT(EXPR, MSG) do { if(!(EXPR)) throw std::runtime_error(MSG); } while(false)
#define ASSERT(EXPR) MASSERT(EXPR, POSITION " failed: " #EXPR)

void testColorSpace()
{
    uint64_t gray = ColorSpace::ID("Y");
    uint64_t rgb = ColorSpace::ID("RGB");
    uint64_t rgba = ColorSpace::ID("RGBA");
    uint64_t rgbx = ColorSpace::ID("RGBX");
    
    ASSERT(ColorSpace::_length(gray) == 1);
    ASSERT(ColorSpace::_length(rgb) == 3);
    ASSERT(ColorSpace::_length(rgba) == 4);
    ASSERT(ColorSpace::_length(rgbx) == 4);

    ASSERT(ColorSpace::_index(rgb, 'R') == 0);
    ASSERT(ColorSpace::_index(rgb, 'G') == 1);
    ASSERT(ColorSpace::_index(rgb, 'B') == 2);

    ASSERT(ColorSpace::_index(rgba, 'R') == 0);
    ASSERT(ColorSpace::_index(rgba, 'G') == 1);
    ASSERT(ColorSpace::_index(rgba, 'B') == 2);
    ASSERT(ColorSpace::_index(rgba, 'A') == 3);

    ASSERT(ColorSpace::_index(rgbx, 'R') == 0);
    ASSERT(ColorSpace::_index(rgbx, 'G') == 1);
    ASSERT(ColorSpace::_index(rgbx, 'B') == 2);
    ASSERT(ColorSpace::_index(rgbx, 'X') == 3);
    ASSERT(ColorSpace::_index(rgbx, 'A') == SIZE_MAX);

    ASSERT(ColorSpace::compatible(gray, "Y"));
    ASSERT(ColorSpace::compatible(rgb, "RGB"));
    ASSERT(ColorSpace::compatible(rgba, "RGB"));
    ASSERT(ColorSpace::compatible(rgbx, "RGB"));

    ASSERT(!ColorSpace::compatible(gray, "RGB"));
    ASSERT(!ColorSpace::compatible(rgb, "Y"));
    ASSERT(!ColorSpace::compatible(rgba, "Y"));
    ASSERT(!ColorSpace::compatible(rgbx, "Y"));

    ASSERT(ColorSpace::compatible(Pixel::RGBA32::id, "RGB"));
    ASSERT(!ColorSpace::compatible(Pixel::RGBA32::id, "Y"));
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
    ASSERT(PixelConvert::RGB(p2, p1));
    Pixel::GrayScale p3;
    ASSERT(PixelConvert::GRAY(p3, p2));
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

    Mat<uint8_t> mat;
    ASSERT(!mat.valid());

    mat.resize(0,1);
    ASSERT(!mat.valid());

    mat.resize(1,0);
    ASSERT(!mat.valid());

    mat.resize(1,1);
    ASSERT(mat.valid());

    mat.resize(0,1);
    ASSERT(!mat.valid());
}

void testBMP()
{
    ASSERT(sizeof(BMP::FileHeader) == 14);
    ASSERT(sizeof(BMP::InfoHeader) == 40);
    ASSERT(sizeof(BMP::BGRPalette) == 4);

    Mat<Pixel::BGR24> source = BMP::read("input.bmp");
    ASSERT(BMP::write(source, "16.bmp", BMP::Format::Bit16));
    Mat<Pixel::RGB24> out1;
    Mat<Pixel::BGR24> out2;
    Mat<Pixel::GRB24> out3;
    Mat<Pixel::GBR24> out4;
    Mat<Pixel::BRG24> out5;
    Mat<Pixel::GrayScale> out6;
    ASSERT(MatConvert::RGB(out1, source, true));
    ASSERT(MatConvert::RGB(out2, source, true));
    ASSERT(MatConvert::RGB(out3, source, true));
    ASSERT(MatConvert::RGB(out4, source, true));
    ASSERT(MatConvert::RGB(out5, source, true));
    ASSERT(MatConvert::GRAY(out6, source, true));
    ASSERT(BMP::write(out1, "out1.bmp"));
    ASSERT(BMP::write(out2, "out2.bmp"));
    ASSERT(BMP::write(out3, "out3.bmp"));
    ASSERT(BMP::write(out4, "out4.bmp"));
    ASSERT(BMP::write(out5, "out5.bmp"));
    ASSERT(BMP::write(out6, "out6.bmp", BMP::Format::Palette));

    Mat<Pixel::Binary> bin;
    ASSERT(MatConvert::BINARY(bin, source, 200));
    ASSERT(BMP::write(source, "bin0.bmp", BMP::Format::Binary, BMP::BGRPalette(0x9966ff), BMP::BGRPalette(0xffffff)));
    ASSERT(BMP::write(bin, "bin1.bmp", BMP::Format::Binary, BMP::BGRPalette(0x9966ff), BMP::BGRPalette(0xffffff)));
}

void testDraw()
{
    Mat<Pixel::BGR24> color{201, 201};
    Draw::Painter<Pixel::BGR24> painter(color);
    ASSERT(painter.fill(Pixel::BGR24(0)));

    ASSERT(painter.drawLine(Pixel::BGR24(0x00ff00), 0, 100, 200, 100));
    ASSERT(painter.drawLine(Pixel::BGR24(0x00ff00), 100, 0, 100, 200));
    ASSERT(painter.drawLine(Pixel::BGR24(0x00ff00), 0, 0, 200, 200));
    ASSERT(painter.drawLine(Pixel::BGR24(0x00ff00), 0, 200, 200, 0));

    ASSERT(painter.drawLine(Pixel::BGR24(0x0000ff), 0, 300, 300, 0));
    ASSERT(painter.drawLine(Pixel::BGR24(0x0000ff), 0, 100, 200, 300));
    ASSERT(painter.drawLine(Pixel::BGR24(0x0000ff), 0, 100, 100, 0));
    ASSERT(painter.drawLine(Pixel::BGR24(0x0000ff), 100, 0, 300, 200));

    ASSERT(painter.drawLine(Pixel::BGR24(0xff0000), 0, 0, 200, 400));
    ASSERT(painter.drawLine(Pixel::BGR24(0xff0000), 0, 200, 100, 0));

    ASSERT(painter.drawLine(Pixel::BGR24(0x00ffff), 0, 400, 200, 0));
    ASSERT(painter.drawLine(Pixel::BGR24(0x00ffff), 100, 0, 300, 400));

    ASSERT(painter.drawLine(Pixel::BGR24(0xffff00), 0, 200, 400, 0));
    ASSERT(painter.drawLine(Pixel::BGR24(0xffff00), 0, 100, 400, 300));

    ASSERT(painter.drawLine(Pixel::BGR24(0xff00ff), 0, 0, 400, 200));
    ASSERT(painter.drawLine(Pixel::BGR24(0xff00ff), 0, 100, 200, 0));

    ASSERT(BMP::write(color, "color24.bmp"));
    ASSERT(BMP::write(color, "color16.bmp", BMP::Format::Bit16));
    ASSERT(BMP::write(color, "palette.bmp", BMP::Format::Palette));
}

void testPalette()
{
    Mat<Pixel::BGR24> read8 = BMP::read("out6.bmp");
    ASSERT(BMP::write(read8, "read8.bmp"));

    Mat<Pixel::BGR24> read4 = BMP::read("palette.bmp");
    ASSERT(BMP::write(read4, "read4.bmp"));
}

int main()
{
    testColorSpace();
    testPixel();
    testUtils();
    testMat();
    testBMP();
    testDraw();
    testPalette();

    printf("SUCCESS\n");
    return 0;
}