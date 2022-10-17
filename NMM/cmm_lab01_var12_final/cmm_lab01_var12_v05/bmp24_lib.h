#ifndef BMP24_LIB_H_INCLUDED
#define BMP24_LIB_H_INCLUDED

#include <string>

#ifdef _WIN32

#include <windows.h>

#else

#include <stdint.h>
typedef uint8_t  BYTE;
typedef uint16_t WORD;
typedef uint32_t DWORD;
typedef int32_t  LONG;

#pragma pack(push, 1)

typedef struct tagRGBTRIPLE
{
    BYTE rgbtBlue;
    BYTE rgbtGreen;
    BYTE rgbtRed;
} RGBTRIPLE;

typedef struct tagBITMAPFILEHEADER
{
    WORD bfType;
    DWORD bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfOffBits;
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{
    DWORD biSize;
    LONG biWidth;
    LONG biHeight;
    WORD biPlanes;
    WORD biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG biXPelsPerMeter;
    LONG biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
} BITMAPINFOHEADER;

#pragma pack(pop)

#endif

class bmp24_file
{
public:
    bmp24_file(WORD width, WORD height);
    bmp24_file(WORD width, WORD height, std::string filename);
    ~bmp24_file();
    void set_filename(std::string filename);
    void set_vertical_mirror(bool value);
    void write();
    void write(std::string filename);
    void set_pixel(WORD x, WORD y, RGBTRIPLE c);
    void set_pixel(WORD x, WORD y, BYTE r, BYTE g, BYTE b);
    RGBTRIPLE get_pixel(WORD x, WORD y);
    void get_pixel(WORD x, WORD y, RGBTRIPLE & c);
    void get_pixel(WORD x, WORD y, BYTE & r, BYTE & g, BYTE & b);

protected:
    BITMAPFILEHEADER file_header;
    BITMAPINFOHEADER info_header;
    WORD size_width;
    WORD size_height;
    std::string name;
    RGBTRIPLE ** color_array;
    RGBTRIPLE null_color;
    void common_init(WORD width, WORD height);
};

#endif // BMP24_LIB_H_INCLUDED
