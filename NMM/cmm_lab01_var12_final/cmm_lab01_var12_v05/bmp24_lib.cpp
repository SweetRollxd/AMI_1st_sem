#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "bmp24_lib.h"
#include <cstdio>
#include <cstring>
#include <cmath>
#include <iostream>

bmp24_file::bmp24_file(WORD width, WORD height)
{
    common_init(width, height);
}

bmp24_file::bmp24_file(WORD width, WORD height, std::string filename)
{
    common_init(width, height);
    set_filename(filename);
}

bmp24_file::~bmp24_file()
{
    for(unsigned int i = 0; i < size_height; i++)
        delete [] color_array[i];
    delete [] color_array;
}

void bmp24_file::common_init(WORD width, WORD height)
{
    size_width = width;
    size_height = height;
    color_array = new RGBTRIPLE * [height];
    for(unsigned int i = 0; i < height; i++)
    {
        color_array[i] = new RGBTRIPLE [width];
        memset(color_array[i], 0, sizeof(RGBTRIPLE) * width);
    }

    memset(&file_header, 0, sizeof(BITMAPFILEHEADER));
    file_header.bfType = 0x4d42;
    file_header.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    file_header.bfSize = file_header.bfOffBits + sizeof(RGBTRIPLE) * size_width * size_height + size_height * ((sizeof(RGBTRIPLE) * size_width) % 4);

    memset(&info_header, 0, sizeof(BITMAPINFOHEADER));
    info_header.biSize = sizeof(BITMAPINFOHEADER);
    info_header.biWidth = size_width;
    info_header.biHeight = size_height;
    info_header.biPlanes = 1;
    info_header.biBitCount = 24;
    info_header.biCompression = 0;

    memset(&null_color, 0, sizeof(RGBTRIPLE));
}

void bmp24_file::set_filename(std::string filename)
{
    name = filename;
}

void bmp24_file::set_vertical_mirror(bool value)
{
    if((value && info_header.biHeight > 0) || (!value && info_header.biHeight < 0))
        info_header.biHeight *= -1;
}

void bmp24_file::set_pixel(WORD x, WORD y, RGBTRIPLE c)
{
    if(x >= size_width || y >= size_height)
        return;
    color_array[y][x] = c;
}

void bmp24_file::set_pixel(WORD x, WORD y, BYTE r, BYTE g, BYTE b)
{
    RGBTRIPLE c;
    c.rgbtBlue = b;
    c.rgbtGreen = g;
    c.rgbtRed = r;
    set_pixel(x, y, c);
}

RGBTRIPLE bmp24_file::get_pixel(WORD x, WORD y)
{
    if(x >= size_width || y >= size_height)
        return null_color;
    return color_array[y][x];
}

void bmp24_file::get_pixel(WORD x, WORD y, RGBTRIPLE & c)
{
    c = get_pixel(x, y);
}

void bmp24_file::get_pixel(WORD x, WORD y, BYTE & r, BYTE & g, BYTE & b)
{
    RGBTRIPLE c = get_pixel(x, y);
    r = c.rgbtRed;
    g = c.rgbtGreen;
    b = c.rgbtBlue;
}

void bmp24_file::write()
{
    FILE * f = fopen(name.c_str(), "wb");
    if(!f)
    {
        if(name.length() == 0)
            std::cerr << "Error: need to set name of file!" << std::endl;
        else
            std::cerr << "Error: unable to open file \"" << name << "\"!" << std::endl;
        return;
    }
    fwrite(&file_header, sizeof(BITMAPFILEHEADER), 1, f);
    fwrite(&info_header, sizeof(BITMAPINFOHEADER), 1, f);
    for(unsigned int i = 0; i < size_height; i++)
    {
        fwrite(color_array[i], sizeof(RGBTRIPLE), size_width, f);
        fwrite(&null_color, (sizeof(RGBTRIPLE) * size_width) % 4, 1, f);
    }
    fflush(f);
    fclose(f);
}

void bmp24_file::write(std::string filename)
{
    set_filename(filename);
    write();
}
