/* Some basic algorithm */
#ifndef LOLITA_TOOLS_H
#define LOLITA_TOOLS_H

#include "mat.hpp"

namespace lolita
{

void grayScale(Image& mat);
void binaryzation(Image& mat, uint8_t threshold = 0);

bool convolution(Image& mat, Mat<double>& kernel);

void detectEdge(Image& mat);

void averageBlur(Image& mat, uint32_t radius);
void medianBlur(Image& mat, uint32_t radius);

void erode(Image& mat, uint32_t radius);
void dilate(Image& mat, uint32_t radius);

void gaussian(Mat<double>& mat, uint32_t radius, double variance);
void gaussianBlur(Image& mat, uint32_t radius, double variance = 1);

void resize(Image& mat, uint32_t width, uint32_t height);
void bicubic(Image& mat, uint32_t width, uint32_t height);
}; // namespace lolita

#endif
