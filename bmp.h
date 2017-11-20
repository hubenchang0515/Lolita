#ifndef LOLITA_BMP_H
#define LOLITA_BMP_H

#include <stdint.h>
#include <string>
#include "mat.h"

namespace lolita
{

class Bmp
{
public:
    static std::string error();
    static bool read(Mat& mat, std::string file);
    static bool write(Mat& mat, std::string file);

private:
    static std::string errorMessage;
};

}; // namespace lolita

#endif