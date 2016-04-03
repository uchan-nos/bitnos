/*
 * point.h
 *
 *  Created on: 2009/07/30
 *      Author: uchan
 */

#ifndef POINT_H_
#define POINT_H_

struct Point
{
    int X;
    int Y;

    Point();

    Point(int x, int y);

    Point(const Point& point);

    Point operator+(const Point& rhs) const;

    Point& operator+=(const Point& rhs);

    Point operator-(const Point& rhs) const;

    Point& operator-=(const Point& rhs);

    Point& operator=(const Point& rhs);

    bool operator==(const Point& rhs) const;

    bool operator!=(const Point& rhs) const {
        return !(*this == rhs);
    }
};

#endif /* POINT_H_ */
