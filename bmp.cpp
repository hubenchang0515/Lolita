#include <stdio.h>
#include "bmp.h"

typedef struct BitMapFileHeader 
{ 
    uint8_t  bfType[2];//'BM'
    uint32_t bfSize;//file size
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;//offset of pixel data
}BitMapFileHeader;

typedef struct BitMapInfoHeader 
{ 
    uint32_t biSize;//28h(40)
    uint32_t biWidth;
    uint32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;//bytes of pixel data
    uint32_t biXPelsPerMeter;
    uint32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
}BitMapInfoHeader; 

typedef struct BGRPalette
{ 
    uint8_t blue;
    uint8_t green;
    uint8_t red;
    uint8_t rgbReserved;
}BGRPalette; 

static void BMP_ReadFileHeader(FILE *bmpfile,BitMapFileHeader& buf) 
{ 
    fseek(bmpfile,0,SEEK_SET); 
   
    fread(buf.bfType,2,1,bmpfile); 
    fread(&buf.bfSize,4,1,bmpfile); 
    fread(&buf.bfReserved1,2,1,bmpfile); 
    fread(&buf.bfReserved2,2,1,bmpfile); 
    fread(&buf.bfOffBits,4,1,bmpfile); 
} 
  
static void BMP_ReadInfoHeader(FILE *bmpfile,BitMapInfoHeader& buf) 
{ 
    fseek(bmpfile,14,SEEK_SET);
    fread(&buf.biSize,4,1,bmpfile); 
    fread(&buf.biWidth,4,1,bmpfile); 
    fread(&buf.biHeight,4,1,bmpfile); 
    fread(&buf.biPlanes,2,1,bmpfile); 
    fread(&buf.biBitCount,2,1,bmpfile); 
    fread(&buf.biCompression,4,1,bmpfile); 
    fread(&buf.biSizeImage,4,1,bmpfile); 
    fread(&buf.biXPelsPerMeter,4,1,bmpfile); 
    fread(&buf.biYPelsPerMeter,4,1,bmpfile); 
    fread(&buf.biClrUsed,4,1,bmpfile); 
    fread(&buf.biClrImportant,4,1,bmpfile); 
} 
  

static void BMP_WriteFileHeader(FILE *bmpfile,BitMapFileHeader& buf) 
{ 
    fseek(bmpfile,0,SEEK_SET); 
    fwrite(buf.bfType,2,1,bmpfile); 
    fwrite(&buf.bfSize,4,1,bmpfile); 
    fwrite(&buf.bfReserved1,2,1,bmpfile); 
    fwrite(&buf.bfReserved2,2,1,bmpfile); 
    fwrite(&buf.bfOffBits,4,1,bmpfile); 
} 
  
static void BMP_WriteInfoHeader(FILE *bmpfile,BitMapInfoHeader& buf) 
{ 
  
    fseek(bmpfile,14,SEEK_SET); 
    fwrite(&buf.biSize,4,1,bmpfile); 
    fwrite(&buf.biWidth,4,1,bmpfile); 
    fwrite(&buf.biHeight,4,1,bmpfile); 
    fwrite(&buf.biPlanes,2,1,bmpfile); 
    fwrite(&buf.biBitCount,2,1,bmpfile); 
    fwrite(&buf.biCompression,4,1,bmpfile); 
    fwrite(&buf.biSizeImage,4,1,bmpfile); 
    fwrite(&buf.biXPelsPerMeter,4,1,bmpfile); 
    fwrite(&buf.biYPelsPerMeter,4,1,bmpfile); 
    fwrite(&buf.biClrUsed,4,1,bmpfile); 
    fwrite(&buf.biClrImportant,4,1,bmpfile); 
}  



namespace lolita
{

/* 24bit color BGR888 */
static void readBgr24(Mat& mat, FILE* fp, uint32_t offset, uint32_t w, uint32_t h)
{
    mat.resize(w,h);
    fseek(fp, offset, SEEK_SET);
    for(uint32_t i = 0; i < h; i++)
    {
        for(uint32_t j = 0; j < w; j++) 
        {
            mat[h-i-1][j].blue = fgetc(fp);
            mat[h-i-1][j].green = fgetc(fp);
            mat[h-i-1][j].red = fgetc(fp);
        }

        /* byets of line should be multiple of 4 , otherwise filled by 0 */
        /* skip these data */
        size_t skip = (w * 3 + 3)/4 * 4 - w * 3;
        fseek(fp, skip, SEEK_CUR);
    }
}

/* 16bit color RGB565 */
static void readRgb16(Mat& mat, FILE* fp, uint32_t offset, uint32_t w, uint32_t h)
{
    mat.resize(w,h);
    uint16_t color = 0;
    fseek(fp, offset, SEEK_SET);
    for(uint32_t i = 0; i < h; i++)
    {
        for(uint32_t j = 0; j < w; j++) 
        {
            fread(&color, 2, 1, fp);
            mat[h-i-1][j].red  = ((color & 0xf800) >> 11) << 3;
            mat[h-i-1][j].green = ((color & 0x07e0) >> 5) << 2;
            mat[h-i-1][j].blue   =  (color & 0x001f) << 3;
        }

        /* byets of line should be multiple of 4 , otherwise filled by 0 */
        /* skip these data */
        size_t skip = (w * 2 + 3)/4 * 4 - w * 2;
        fseek(fp, skip, SEEK_CUR);
    }
}

/* 8bit color , 256 palettes */
static void readPalette8(Mat& mat, FILE* fp, uint32_t offset, uint32_t w, uint32_t h)
{
    mat.resize(w,h);
    uint8_t index = 0;
    BGRPalette color;
    for(uint32_t i = 0; i < h; i++)
    {
        for(uint32_t j = 0; j < w; j++) 
        {
            fseek(fp, offset + i * w + j, SEEK_SET);
            index = fgetc(fp);
            fseek(fp, 14 + 40 + 4 * index, SEEK_SET);
            fread(&color, 4, 1, fp);
            mat[h-i-1][j].red = color.red;
            mat[h-i-1][j].green = color.green;
            mat[h-i-1][j].blue = color.blue;
        }

        /* byets of line should be multiple of 4 , otherwise filled by 0 */
        /* skip these data */
        size_t skip = (w + 3)/4 * 4 - w;
        fseek(fp, skip, SEEK_CUR);
    }
}

/* 4bit color , 16 palettes */
static void readPalette4(Mat& mat, FILE* fp, uint32_t offset, uint32_t w, uint32_t h)
{
    mat.resize(w,h);
    uint8_t index = 0;
    BGRPalette color;
    for(uint32_t i = 0; i < h; i++)
    {
        uint32_t j = 0;
        for(; j < w; j+=2) 
        {
            fseek(fp, offset + i * w + j, SEEK_SET);
            index = fgetc(fp);
            fseek(fp, 14 + 40 + 4 * ((index & 0xf0) >> 4), SEEK_SET);
            fread(&color, 4, 1, fp);
            mat[h-i-1][j].red = color.red;
            mat[h-i-1][j].green = color.green;
            mat[h-i-1][j].blue = color.blue;

            fseek(fp, 14 + 40 + 4 * (index & 0x0f), SEEK_SET);
            fread(&color, 4, 1, fp);
            mat[h-i-1][j+1].red = color.red;
            mat[h-i-1][j+1].green = color.green;
            mat[h-i-1][j+1].blue = color.blue;
        }

        /* byets of line should be multiple of 4 , otherwise filled by 0 */
        /* skip these data */
        size_t skip = (j / 2 + 3)/4 * 4 - j/2;
        fseek(fp, skip, SEEK_CUR);
    }
}

/* 1bit color , 2 palettes */
static void readPalette1(Mat& mat, FILE* fp, uint32_t offset, uint32_t w, uint32_t h)
{
    mat.resize(w,h);
    uint8_t index = 0;
    BGRPalette color;
    for(uint32_t i = 0; i < h; i++)
    {
        uint32_t j = 0;
        for(; j < w; j+=8) 
        {
            fseek(fp, offset + i * w + j, SEEK_SET);
            index = fgetc(fp);
            for(uint8_t k = 0; k < 8; k++)
            {
                fseek(fp, 14 + 40 + 4 * ((index >> k)  & 0x01), SEEK_SET);
                fread(&color, 4, 1, fp);
                mat[h-i-1][j+7-k].red = color.red;
                mat[h-i-1][j+7-k].green = color.green;
                mat[h-i-1][j+7-k].blue = color.blue;
            }
        }

        /* byets of line should be multiple of 4 , otherwise filled by 0 */
        /* skip these data */
        size_t skip = (j / 8 + 3)/4 * 4 - j/8;
        fseek(fp, skip, SEEK_CUR);
    }
}



bool Bmp::read(Mat& mat, std::string file)
{
    FILE* fp = fopen(file.c_str(),"rb");
    if(fp == NULL)
    {
        return false;
    }

    BitMapFileHeader fileHeader;
    BitMapInfoHeader infoHeader;
    BMP_ReadFileHeader(fp, fileHeader);
    BMP_ReadInfoHeader(fp, infoHeader);

    mat.resize(infoHeader.biWidth, infoHeader.biHeight);
     
    switch(infoHeader.biBitCount)
    {
    case 24:
        readBgr24(mat, fp, fileHeader.bfOffBits, infoHeader.biWidth, infoHeader.biHeight);
        break;

    case 16:
        readRgb16(mat, fp, fileHeader.bfOffBits, infoHeader.biWidth, infoHeader.biHeight);
        break;

    case 8:
        readPalette8(mat, fp, fileHeader.bfOffBits, infoHeader.biWidth, infoHeader.biHeight);
        break;

    case 4:
        readPalette4(mat, fp, fileHeader.bfOffBits, infoHeader.biWidth, infoHeader.biHeight);
        break;

    case 1:
        readPalette1(mat, fp, fileHeader.bfOffBits, infoHeader.biWidth, infoHeader.biHeight);
        break;

    default:
        fclose(fp);
        return false;
    }
    fclose(fp);
    return true;
}


bool Bmp::write(Mat& mat, std::string file)
{
    FILE* fp = fopen(file.c_str(),"wb");
    if(fp == NULL)
    {
        return false;
    }

    uint32_t w = mat.width();
    uint32_t h = mat.height(); 

    BitMapFileHeader fileHeader;
    BitMapInfoHeader infoHeader;

    fileHeader.bfType[0] = 'B'; 
    fileHeader.bfType[1] = 'M'; 
    fileHeader.bfReserved1 = 0; 
    fileHeader.bfReserved2 = 0; 
    fileHeader.bfOffBits = 14 + 40;
    fileHeader.bfSize = 3*w*h + fileHeader.bfOffBits;
    infoHeader.biSize = 40;
    infoHeader.biWidth =  w;
    infoHeader.biHeight = h;
    infoHeader.biPlanes = 1; 
    infoHeader.biBitCount = 24;
    infoHeader.biCompression = 0; 
    infoHeader.biSizeImage = ((w*3+3)/4*4)*h;
    infoHeader.biXPelsPerMeter = 3780; 
    infoHeader.biYPelsPerMeter = 3780; 
    infoHeader.biClrUsed = 0; 
    infoHeader.biClrImportant = 0;
    


    BMP_WriteFileHeader(fp, fileHeader);
    BMP_WriteInfoHeader(fp, infoHeader);

    for(uint32_t i = 0; i < h; i++)
    {
        for(uint32_t j = 0; j < w; j++) 
        {
            fputc(mat[h - i - 1][j].blue , fp);
            fputc(mat[h - i - 1][j].green , fp);
            fputc(mat[h - i - 1][j].red , fp);
        }

        /* byets of line should be multiple of 4 , otherwise filled by 0 */
        size_t skip = (w * 3 + 3)/4 * 4 - w * 3;
        while(skip--)
        {
            fputc(0,fp);
        }
    }


    fclose(fp);
    return true;
}

}; // namespace lolita