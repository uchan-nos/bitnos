/*
 * image.cpp
 *
 *  Created on: 2009/07/06
 *      Author: uchan
 */

#include <string.h>
#include <bitnos/image.h>

uint16_t ConvRGB16(uint8_t r, uint8_t g, uint8_t b)
{
    return
        (((uint16_t)r << 8) & 0xf800) |
        (((uint16_t)g << 3) & 0x07e0) |
        (((uint16_t)b >> 3) & 0x001f);
}

uint16_t ConvRGB16(uint32_t col)
{
    return ConvRGB16((uint8_t)(col >> 16), (uint8_t)(col >> 8), (uint8_t)col);
}


Image::Image(uint16_t* buf, int width, int height) :
    buf(buf), width(width), height(height), allocated(false)
{
}

Image::Image(int width, int height) :
    buf(0), width(width), height(height), allocated(false)
{
    this->buf = new uint16_t[this->width * this->height];
    this->allocated = true;
}

Image::Image(const Image& rhs) :
    buf(0), width(rhs.width), height(rhs.height), allocated(false)
{
    if (this != &rhs) {
        this->buf = new uint16_t[this->width * this->height];
        this->allocated = true;

        memcpy(this->buf, rhs.buf, this->width * this->height * sizeof(uint16_t));
    }
}

Image::~Image()
{
    if (allocated) {
        delete[] this->buf;
    }
}

int Image::GetWidth() const
{
    return this->width;
}

int Image::GetHeight() const
{
    return this->height;
}

void Image::DrawRectangle(uint16_t col, int xPos0, int yPos0, int xPos1, int yPos1)
{
    int x, y;
    int temp;
    if (xPos0 > xPos1) {
        temp = xPos0;
        xPos0 = xPos1;
        xPos1 = temp;
    }
    if (yPos0 > yPos1) {
        temp = yPos0;
        yPos0 = yPos1;
        yPos1 = temp;
    }
    for (y = yPos0; y <= yPos1; y++) {
        //this->buf[y * this->width + xPos0] = col;
        //this->buf[y * this->width + xPos1] = col;
        DrawPoint(col, xPos0, y);
        DrawPoint(col, xPos1, y);
    }
    for (x = xPos0; x <= xPos1; x++) {
        //this->buf[yPos0 * this->width + x] = col;
        //this->buf[yPos1 * this->width + x] = col;
        DrawPoint(col, x, yPos0);
        DrawPoint(col, x, yPos1);
    }
}

void Image::DrawRectangleFill(uint16_t col, int xPos0, int yPos0, int xPos1, int yPos1)
{
    int x, y;
    int temp;

    if (xPos0 > xPos1) {
        temp = xPos0;
        xPos0 = xPos1;
        xPos1 = temp;
    }
    if (yPos0 > yPos1) {
        temp = yPos0;
        yPos0 = yPos1;
        yPos1 = temp;
    }

    for (y = yPos0; y <= yPos1; y++) {
        for (x = xPos0; x <= xPos1; x++) {
            DrawPoint(col, x, y);
        }
    }
}

void Image::DrawLine(uint16_t col, int xPos0, int yPos0, int xPos1, int yPos1)
{
    int dx = xPos1 - xPos0;
    int dy = yPos1 - yPos0;

    if (dx < 0) {
        dx = -dx;
    }
    if (dy < 0) {
        dy = -dx;
    }

    if (dx == 0 && dy == 0) {
        DrawPoint(col, xPos0, yPos0);
    } else {
        int len;
        if (dx >= dy) {
            len = dx + 1;
            if (xPos0 <= xPos1) {
                dx = (1 << 16);
            } else {
                dx = -(1 << 16);
            }
            if (yPos0 <= yPos1) {
                dy = ((yPos1 - yPos0 + 1) << 16) / len;
            } else {
                dy = ((yPos1 - yPos0 - 1) << 16) / len;
            }
        } else {
            len = dy + 1;
            if (yPos0 <= yPos1) {
                dy = (1 << 16);
            } else {
                dy = -(1 << 16);
            }
            if (xPos0 <= xPos1) {
                dx = ((xPos1 - xPos0 + 1) << 16) / len;
            } else {
                dx = ((xPos1 - xPos0 - 1) << 16) / len;
            }
        }
        int x = xPos0 << 16;
        int y = yPos0 << 16;
        for (int i = 0; i < len; i++) {
            DrawPoint(col, x >> 16, y >> 16);
            x += dx;
            y += dy;
        }
    }
}

void Image::DrawChar(uint16_t col, char ch, int xPos, int yPos, BitmapFont* font)
{
    // 1文字の描画
    int i, x, y, w;
    uint8_t line;
    uint8_t* pdata;
    int width_byte = ((font->GetWidth() + 7) >> 3);

    pdata = (uint8_t*)font->GetDataPtr(ch);
    for (y = 0; y < font->GetHeight(); y++) {
        for (w = 0; w < width_byte; w++) {
            line = *((uint8_t*)pdata + y * width_byte + w);
            for (x = 0; x < 8; x++) {
                if ((line & 0x80) != 0) {
                    DrawPoint(col, xPos + (w << 8) + x, yPos + y);
                }
                line <<= 1;
            }
        }
    }
}

void Image::DrawString(uint16_t col, const char* str, int xPos, int yPos, BitmapFont* font)
{
    /*
    // 文字列の描画
    int i, x, y, w;
    uint8_t line;
    uint8_t* pdata;
    int width_byte = ((font->GetWidth() + 7) >> 3);

    for (i = 0; str[i] != '\0'; i++) {
    pdata = (uint8_t*)font->GetDataPtr(str[i]);
    for (y = 0; y < font->GetHeight(); y++) {
    for (w = 0; w < width_byte; w++) {
    line = *((uint8_t*)pdata + y * width_byte + w);
    for (x = 0; x < 8; x++) {
    if ((line & 0x80) != 0) {
    DrawPoint(col, xPos + i * font->GetWidth() + (w << 8) + x, yPos + y);
    }
    line <<= 1;
    }
    }
    }
    }
    */
    for (int i = 0; str[i] != '\0'; i++) {
        DrawChar(col, str[i], xPos + font->GetWidth() * i, yPos, font);
    }
}


void Image::DrawPoint(uint16_t col, Point pos)
{
    DrawPoint(col, pos.X, pos.Y);
}

void Image::DrawRectangle(uint16_t col, Point pos0, Point pos1)
{
    DrawRectangle(col, pos0.X, pos0.Y, pos1.X, pos1.Y);
}
void Image::DrawRectangleFill(uint16_t col, Point pos0, Point pos1)
{
    DrawRectangleFill(col, pos0.X, pos0.Y, pos1.X, pos1.Y);
}

void Image::DrawLine(uint16_t col, Point pos0, Point pos1)
{
    DrawLine(col, pos0.X, pos0.Y, pos1.X, pos1.Y);
}

void Image::DrawChar(uint16_t col, char ch, Point pos, BitmapFont* font)
{
    DrawChar(col, ch, pos.X, pos.Y, font);
}

void Image::DrawString(uint16_t col, const char* str, Point pos, BitmapFont* font)
{
    DrawString(col, str, pos.X, pos.Y, font);
}


