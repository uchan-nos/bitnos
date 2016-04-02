/*
 * Window.cpp
 *
 *  Created on: 2009/09/14
 *      Author: uchan
 */

#include <bitnos/sheet.h>

#include <bitnos/gui/Window.h>
#include <bitnos/gui/Control.h>

namespace GUI
{

    Window::Window(Control** controlBuffer, Sheet* sht) :
        controls(controlBuffer), numControl(0), sht(sht)
    {
    }

    Window::~Window()
    {
    }

    void Window::Render()
    {
        /*
           for (int i = 0; i < numControl; i++) {
           controls[i]->Render(
           sht->image,
           Point(0, 0),
           Point(sht->image->GetWidth() - 1, sht->image->GetHeight() - 1));
           }
           shtman->Refresh(sht);
           */
        Render(
                Point(0, 0),
                Point(sht->image->GetWidth() - 1, sht->image->GetHeight() - 1));
    }

    void Window::Render(Point pos0, Point pos1)
    {
        for (int i = 0; i < numControl; i++) {
            controls[i]->Render(
                    sht->image, pos0, pos1);
        }
        shtman->Refresh(sht);
    }

}
