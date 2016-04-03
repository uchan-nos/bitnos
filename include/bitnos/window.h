/*
 * window.h
 *
 *  Created on: 2009/08/17
 *      Author: uchan
 */

#ifndef WINDOW_H_
#define WINDOW_H_

class Image;

class Window
{
    public:
        static void Make(Image* img);
        static void Make(Image* img, const char* title);
};

#endif /* WINDOW_H_ */
