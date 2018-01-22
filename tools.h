/* Some basic algorithm */
#ifndef LOLITA_TOOLS_H
#define LOLITA_TOOLS_H

#include "mat.hpp"

namespace lolita
{

void grayScale(Image& mat);
void binaryzation(Image& mat, uint8_t threshold = 0);

}; // namespace lolita

#endif
