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
void middleBlur(Image& mat, uint32_t radius);
}; // namespace lolita

#endif
