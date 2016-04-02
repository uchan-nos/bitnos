/*
 * window.cpp
 *
 *  Created on: 2009/08/17
 *      Author: uchan
 */

#include <bitnos/window.h>
#include <bitnos/sheet.h>
#include <bitnos/image.h>
#include <bitnos/point.h>

void Window::Make(Image* img)
{
    Make(img, "");
}

void Window::Make(Image* img, const char* title)
{
    if (img == 0 || img->GetHeight() < 26 || img->GetWidth() < 26) {
        return;
    }
    img->DrawRectangleFill(
            ConvRGB16(68, 157, 255), Point(0, 0), Point(img->GetWidth() - 1, 25));
    img->DrawRectangleFill(
            ConvRGB16(252, 71, 157),
            Point(img->GetWidth() - 28, 5),
            Point(img->GetWidth() - 13, 20)
            );
    img->DrawRectangleFill(
            ConvRGB16(0xe0, 0xe0, 0xe0),
            Point(1, 26), Point(img->GetWidth() - 2, img->GetHeight() - 2));
    img->DrawRectangleFill(
            ConvRGB16(43, 36, 104),
            Point(0, 0), Point(0, img->GetHeight() - 1));
    img->DrawRectangleFill(
            ConvRGB16(43, 36, 104),
            Point(img->GetWidth() - 1, 0), Point(img->GetWidth() - 1, img->GetHeight() - 1));
    img->DrawRectangleFill(
            ConvRGB16(43, 36, 104),
            Point(1, 0), Point(img->GetWidth() - 2, 0));
    img->DrawRectangleFill(
            ConvRGB16(43, 36, 104),
            Point(0, img->GetHeight() - 1), Point(img->GetWidth() - 1, img->GetHeight() - 1));

    img->DrawString(ConvRGB16(0, 0, 0), title, Point(8, 5), fontHankaku);
}
