/*
 * font.cpp
 *
 *  Created on: 2009/07/11
 *      Author: uchan
 */

#include <bitnos/font.h>

BitmapFont* fontHankaku;

void BitmapFont::Init(int width, int height, void* data, int dataSize, int increment)
{
    this->width = width;
    this->height = height;
    this->data = data;
    this->dataSize = dataSize;
    this->increment = increment;
}
